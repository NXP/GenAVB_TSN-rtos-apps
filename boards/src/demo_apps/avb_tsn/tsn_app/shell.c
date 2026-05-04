/*
 * Copyright 2020-2021, 2023-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>
#include <stdio.h>
#include <getopt.h>

#include "rtos_apps/tsn/tsn_tasks_config.h"
#include "rtos_apps/tsn/user_button.h"
#include "rtos_apps/types.h"

#include "shell.h"
#include "avb_tsn/common/genavb.h"
#include "avb_tsn/tsn_app/shell.h"
#include "avb_tsn/common/storage.h"
#include "avb_tsn/common/system_config.h"

#if (defined(CONFIG_APP_QBV) || defined(CONFIG_APP_FP))

#include "avb_tsn/common/genavb.h"
#include "genavb/helpers.h"
#include "genavb/ether.h"

#include "avb_tsn/common/fp.h"
#include "avb_tsn/common/qbv.h"

#define ST_TX_TIME_MARGIN 1000 /* Additional margin to account for drift between MAC and gPTP clocks */
#define ST_TX_TIME_FACTOR 2 /* Factor applied to critical time interval, to avoid frames getting stuck */
#define ST_LIST_LEN       2

static shell_status_t default_qos_set(shell_handle_t shell, int32_t argc, char **argv);

/*
 * Returns the complete transmit time including MAC framing and physical
 * layer overhead (802.3).
 * \return transmit time in nanoseconds
 * \param frame_size frame size without any framing
 * \param speed_mbps link speed in Mbps
 */
static unsigned int frame_tx_time_ns(unsigned int frame_size, int speed_mbps)
{
    unsigned int eth_size;

    eth_size = sizeof(struct eth_hdr) + frame_size + ETHER_FCS;

    if (eth_size < ETHER_MIN_FRAME_SIZE)
        eth_size = ETHER_MIN_FRAME_SIZE;

    eth_size += ETHER_IFG + ETHER_PREAMBLE;

    return (((1000 / speed_mbps) * eth_size * 8) + ST_TX_TIME_MARGIN);
}

SHELL_COMMAND_DEFINE(default_qos_set,
                    "\ndefault_qos_set <port_id> [-l <link speed>] [-s <st enabled>] [-b <base time>] [-c <cycle time>] [-f <fp enabled>]\n"
                     "  parameters:\n"
                     "      port_id: logical port index\n"
                     "  options:\n"
                     "      -l <link speed>: set link speed in Mbps, default: 1000 Mbps\n"
                     "      -s <st enabled>: enable st, 1: enabled, 0: disabled, default: enabled\n"
                     "      -b <base_time>: 64 bit absolute gPTP time. value: 0 to (2^64 - 1), default: 35000 (controller) or 85000 (io_device)\n"
                     "      -c <cycle time>: application period, default: 100000 ns\n"
                     "      -f <fp enabled>: enable fp, 1: enabled, 0: disabled, default: disabled\n",
                    &default_qos_set,
                    SHELL_IGNORE_PARAMETER_COUNT);

static void print_default_qos_set_usage(shell_handle_t shell)
{
    shell_printf(shell, "Usage: ");
    shell_printf(shell, (SHELL_COMMAND(default_qos_set))->pcHelpString);
}

static shell_status_t default_qos_set(shell_handle_t shell, int32_t argc, char **argv)
{
    bool use_fp = false, use_st = true, custom_base_time = false;
    struct genavb_st_gate_control_entry gate_list[ST_LIST_LEN];
    struct genavb_fp_config config_fp_8021q = {0};
    struct genavb_fp_config config_fp_8023 = {0};
    struct genavb_st_config config_qbv = {0};
    struct rtos_apps_tsn_config *config;
    unsigned int offset_ns, cycle_time;
    unsigned int port_id, link_speed = 1000;
    unsigned int iso_tx_time = 0;
    unsigned long long tmp2;
    uint64_t base_time;
    unsigned long tmp;
    uint8_t tclass;
    int i, opt;

    config = system_config_get_tsn_app();
    port_id = config->port_id;
    cycle_time = config->period_ns;

    if (argc < 2) {
        print_default_qos_set_usage(shell);
        goto err;
    }

    h_strtoul(&tmp, argv[1], NULL, 0);
    port_id = tmp;
    if (port_id >= CONFIG_APP_LOGICAL_PORTS) {
        shell_printf(shell, "invalid port_id %u\n", port_id);
        goto err;
    }

    optind = 2;
    while ((opt = getopt(argc, argv, "l:s:b:c:f:")) != -1) {
        switch (opt) {
        case 'l':
            h_strtoul(&tmp, optarg, NULL, 0);
            link_speed = tmp;
            break;
        case 's':
            h_strtoul(&tmp, optarg, NULL, 0);
            use_st = (bool)tmp ? true : false;
            break;
        case 'b':
            h_strtoull(&tmp2, optarg, NULL, 0);
            custom_base_time = true;
            base_time = tmp2;
            break;
        case 'c':
            h_strtoul(&tmp, optarg, NULL, 0);
            cycle_time = tmp;
            break;
        case 'f':
            h_strtoul(&tmp, optarg, NULL, 0);
            use_fp = (bool)tmp ? true : false;
            break;
        default:
            break;
        }
    }

    if (!custom_base_time) {
        offset_ns = (config->offset + 1) * cycle_time / 2;
        base_time = offset_ns - config->network_budget_ns;

        if (config->role != CONTROLLER_0)
            base_time += offset_ns;
    }

    iso_tx_time = frame_tx_time_ns(PACKET_SIZE, link_speed) * ST_TX_TIME_FACTOR;
    config_qbv.cycle_time_p = cycle_time;
    config_qbv.cycle_time_q = NSECS_PER_SEC;
    tclass = genavb_priority_to_traffic_class(port_id, config->priority);

    if (use_st) {
        config_qbv.enable = 1;
        gate_list[0].gate_states = 1 << tclass;

        if (use_fp) {
            gate_list[0].operation = GENAVB_ST_SET_AND_HOLD_MAC;

            /*
             * Keep preemptable queues always open.
             * Match configuration done in tsn_net_fp_config_enable().
             */
            for (i = 0; i < tclass; i++)
                gate_list[0].gate_states |= 1 << i;
        } else {
            gate_list[0].operation = GENAVB_ST_SET_GATE_STATES;
        }

        gate_list[0].time_interval = iso_tx_time;

        if (use_fp)
            gate_list[1].operation = GENAVB_ST_SET_AND_RELEASE_MAC;
        else
            gate_list[1].operation = GENAVB_ST_SET_GATE_STATES;

        gate_list[1].gate_states = (~(1 << tclass));
        gate_list[1].time_interval = cycle_time - iso_tx_time;

        config_qbv.base_time = base_time;

        config_qbv.list_length = ST_LIST_LEN;
        config_qbv.control_list = gate_list;
    } else {
        config_qbv.enable = 0;
    }

#ifdef CONFIG_APP_STORAGE
    if (qbv_write_permanent(shell, port_id, config_qbv) < 0)
        goto err;
#endif /* CONFIG_APP_STORAGE */
    if (use_fp) {

        for (i = 0; i < QOS_PRIORITY_MAX; i++) {
            if (genavb_priority_to_traffic_class(port_id, i) >= tclass)
                config_fp_8021q.u.cfg_802_1Q.admin_status[i] = GENAVB_FP_ADMIN_STATUS_EXPRESS;
            else
                config_fp_8021q.u.cfg_802_1Q.admin_status[i] = GENAVB_FP_ADMIN_STATUS_PREEMPTABLE;
        }

        config_fp_8023.u.cfg_802_3.enable_tx = 1;
        config_fp_8023.u.cfg_802_3.verify_disable_tx = 0;
        config_fp_8023.u.cfg_802_3.verify_time = 100;
        config_fp_8023.u.cfg_802_3.add_frag_size = 0;
    } else {
        for (i = 0; i < QOS_PRIORITY_MAX; i++) {
            config_fp_8021q.u.cfg_802_1Q.admin_status[i] = GENAVB_FP_ADMIN_STATUS_EXPRESS;
        }

        config_fp_8023.u.cfg_802_3.enable_tx = 0;
        config_fp_8023.u.cfg_802_3.verify_disable_tx = 0;
        config_fp_8023.u.cfg_802_3.verify_time = 10;
        config_fp_8023.u.cfg_802_3.add_frag_size = 0;
    }

#ifdef CONFIG_APP_STORAGE
    if (fp_write_802_1q_permanent(shell, port_id, config_fp_8021q) < 0)
        goto err;

    if (fp_write_802_3_permanent(shell, port_id, config_fp_8023) < 0)
        goto err;

    if (!use_st)
        qbv_apply_permanent(shell, port_id);

    if (!use_fp)
        fp_apply_permanent(shell, port_id);

    if (fp_apply_permanent(shell, port_id) < 0)
        goto err;

    if (qbv_apply_permanent(shell, port_id) < 0)
        goto err;
#else
    if (!use_st)
        qbv_apply(shell, port_id, &config_qbv);

    if (!use_fp)
        fp_apply(shell, port_id, &config_fp_8021q, &config_fp_8023);

    if (fp_apply(shell, port_id, &config_fp_8021q, &config_fp_8023) < 0)
        goto err;

    if (qbv_apply(shell, port_id, &config_qbv) < 0)
        goto err;
#endif /* CONFIG_APP_STORAGE */
    return kStatus_SHELL_Success;
err:
    return kStatus_SHELL_Error;
}

void default_qos_init(shell_handle_t shell)
{
    SHELL_RegisterCommand(shell, SHELL_COMMAND(default_qos_set));
}
#else
void default_qos_init(shell_handle_t shell) {return;}
#endif /* (defined(CONFIG_APP_QBV) || defined(CONFIG_APP_FP)) */


extern struct rtos_apps_user_button *user_button;

/* @brief Shell command that can be used instead of user button */
static shell_status_t shell_user_button(shell_handle_t shell, int32_t argc, char **argv)
{
    rtos_apps_user_button_event(user_button);

    return kStatus_SHELL_Success;
}

SHELL_COMMAND_DEFINE(button,
                     "\r\nbutton\r\n",
                     &shell_user_button,
                     0);

/* The specific config help for application */
void help_config_app(shell_handle_t shell)
{
    shell_printf(shell, "app config\n");
    shell_printf(shell, "path: /tsn_app\n");
    shell_printf(shell, "parameters:\n");
    shell_printf(shell, "    mode:\n");
    shell_printf(shell, "        value: 0 to 3 (default 2)\n");
    shell_printf(shell, "        description: application mode\n");
#if (defined(CONFIG_APP_MOTOR_CONTROLLER) || defined(CONFIG_APP_MOTOR_IO_DEVICE))
    shell_printf(shell, "                     0 MOTOR_NETWORK\n");
#endif /* CONFIG_APP_MOTOR */
    shell_printf(shell, "                     2 NETWORK_ONLY\n");
#ifdef CONFIG_APP_SERIAL
    shell_printf(shell, "                     3 SERIAL\n");
#endif /* CONFIG_APP_SERIAL */
    shell_printf(shell, "    role:\n");
    shell_printf(shell, "        value: 0 to 2 (default 0)\n");
    shell_printf(shell, "        description: endpoint role\n");
    shell_printf(shell, "                     0 CONTROLLER\n");
    shell_printf(shell, "                     1 IO_DEVICE_0\n");
    shell_printf(shell, "                     2 IO_DEVICE_1\n");
    shell_printf(shell, "    num_io_devices:\n");
    shell_printf(shell, "        condition: if role is CONTROLLER\n");
    shell_printf(shell, "        value: 1 to 2 (default 1)\n");
    shell_printf(shell, "        description: number of IO devices\n");
    shell_printf(shell, "    period_ns:\n");
    shell_printf(shell, "        value: minimum %u ns (default %u ns or %u ns in serial mode)\n",
                 APP_PERIOD_MIN, APP_PERIOD_DEFAULT, APP_PERIOD_SERIAL_DEFAULT);
    shell_printf(shell, "               only 100000 ns or 250000 ns are available for motor modes\n");
    shell_printf(shell, "        description: application period in nanoseconds\n");
    shell_printf(shell, "    offset:\n");
    shell_printf(shell, "        value: 0 to 1 (default %u)\n", APP_OFFSET_DEFAULT);
    shell_printf(shell, "        description: application processing offset\n");
    shell_printf(shell, "                     0 Half period\n");
    shell_printf(shell, "                     1 One period\n");
    shell_printf(shell, "    network_budget_ns:\n");
    shell_printf(shell, "        description: network budget in nanoseconds\n");
    shell_printf(shell, "    priority:\n");
    shell_printf(shell, "        value: 0 to 7 (default 5)\n");
    shell_printf(shell, "        description: TSN traffic priority (PCP)\n");
#ifdef CONFIG_APP_MOTOR_IO_DEVICE
    shell_printf(shell, "    motor_offset:\n");
    shell_printf(shell, "        condition: if role is IO_DEVICE_X\n");
    shell_printf(shell, "        format: float\n");
    shell_printf(shell, "        description: motor position offset\n");
#endif
#ifdef CONFIG_APP_MOTOR_CONTROLLER
    shell_printf(shell, "    control_strategy:\n");
    shell_printf(shell, "        condition: if role is CONTROLLER\n");
    shell_printf(shell, "        value: 0 to 3 (default 0)\n");
    shell_printf(shell, "        description: motor control strategy\n");
    shell_printf(shell, "                     0 SYNCHRONIZED\n");
    shell_printf(shell, "                     1 FOLLOW\n");
    shell_printf(shell, "                     2 HOLD_INDEX\n");
    shell_printf(shell, "                     3 INTERLACED\n");
#endif
    shell_printf(shell, "    port_id: logical port id\n");
    shell_printf(shell, "        condition: if device has more than one network interface available for use\n");
    shell_printf(shell, "        value: min 0 (default), max %u\n", CONFIG_APP_LOGICAL_PORTS - 1);
    shell_printf(shell, "        description: application logical port id\n");
    shell_printf(shell, "    zero_copy: enable 0-copy mode\n");
    shell_printf(shell, "        value: 0: disable, 1: enable (default)\n");
    shell_printf(shell, "        description: enable/disable socket 0-copy mechanism\n");
    shell_printf(shell, "    rx_tc_mask: receive traffic class mask\n");
    shell_printf(shell, "        value: 0 to 0xFF (default 0, depends on number of available traffic classes)\n");
    shell_printf(shell, "        description: skips traffic classes in cyclic task socket receive processing, one bit per traffic class\n");
}

shell_handle_t tsn_init_shell(const char *tsn_task_id)
{
    shell_handle_t shell;

    shell = shell_init(tsn_task_id);

    storage_set_shell(shell);

    if (shell)
        SHELL_RegisterCommand(shell, SHELL_COMMAND(button));

    return shell;
}
