/*
 * Copyright 2024-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "genavb/helpers.h"

#include <stdint.h>
#include <string.h>

#include "shared_config.h"
#include "avb_tsn/common/shell.h"
#include "avb_tsn/common/storage.h"

/* These values need to be aligned with M7 application definitions */
#define M7_STACK_LEN 0x400
#define M7_DTCM_END 0x20440000
#define SHARED_CONFIG_OFFSET 0x200

static struct system_config_m7 *shared_system_cfg = (struct system_config_m7 *)((uintptr_t)(M7_DTCM_END - M7_STACK_LEN - SHARED_CONFIG_OFFSET));

extern struct system_config_m7 m7_system_cfg;

void help_config_params_multicore(shell_handle_t shell)
{
    shell_printf(shell, "multicore parameters config\n\n");
    shell_printf(shell, "path: /m7/tsn_app\n");
    shell_printf(shell, "parameters:\n");
    shell_printf(shell, "    mode:\n");
    shell_printf(shell, "        value: 0 to 3 (default 2)\n");
    shell_printf(shell, "        description: application mode\n");
    shell_printf(shell, "                     0 MOTOR_NETWORK, only if compiled with BUILD MOTOR\n");
    shell_printf(shell, "                     2 NETWORK_ONLY\n");
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
    shell_printf(shell, "        description: application period in nanoseconds\n");
    shell_printf(shell, "    offset:\n");
    shell_printf(shell, "        value: 0 to 1 (default 0)\n");
    shell_printf(shell, "        description: application processing offset\n");
    shell_printf(shell, "                     0 Half period\n");
    shell_printf(shell, "                     1 One period\n");
    shell_printf(shell, "    network_budget_ns:\n");
    shell_printf(shell, "        description: network budget in nanoseconds\n");
    shell_printf(shell, "    priority:\n");
    shell_printf(shell, "        value: 0 to 7 (default 5)\n");
    shell_printf(shell, "        description: TSN traffic priority (PCP)\n");
    shell_printf(shell, "    tx_time_offset_ns:\n");
    shell_printf(shell, "        value: 0 to (2^32 - 1) (default 0)\n");
    shell_printf(shell, "        description: packet transmit time offset in nanoseconds\n");
    shell_printf(shell, "    tx_time_enabled:\n");
    shell_printf(shell, "        value: 0 or 1 (default 0)\n");
    shell_printf(shell, "        description: enable/disable packet transmit time\n");
    shell_printf(shell, "    motor_offset:\n");
    shell_printf(shell, "        condition: if role is IO_DEVICE_X\n");
    shell_printf(shell, "        format: float\n");
    shell_printf(shell, "        description: motor position offset\n");
    shell_printf(shell, "    control_strategy:\n");
    shell_printf(shell, "        condition: if role is CONTROLLER\n");
    shell_printf(shell, "        value: 0 to 3 (default 0)\n");
    shell_printf(shell, "        description: motor control strategy\n");
    shell_printf(shell, "                     0 SYNCHRONIZED\n");
    shell_printf(shell, "                     1 FOLLOW\n");
    shell_printf(shell, "                     2 HOLD_INDEX\n");
    shell_printf(shell, "                     3 INTERLACED\n");
    shell_printf(shell, "    port_id: logical port id\n");
    shell_printf(shell, "        condition: if device has more than 1 network interface available for use\n");
    shell_printf(shell, "        value: min 0 (default), max %u\n", CONFIG_APP_LOGICAL_PORTS_M7 - 1);
    shell_printf(shell, "        description: application logical port id\n");
    shell_printf(shell, "    zero_copy: enable 0-copy mode\n");
    shell_printf(shell, "        value: true or false\n");
    shell_printf(shell, "        description: enable/disable packets 0-copy mechanism\n");
    shell_printf(shell, "    rx_tc_mask: receive traffic class mask\n");
    shell_printf(shell, "        value: 0 to 0xFF (default 0, depends on number of available traffic classes)\n");
    shell_printf(shell, "        description: skips traffic classes in cyclic task socket receive processing, one bit per traffic class\n");
    shell_printf(shell, "path :/m7/portN (N: logical port index, 0 to %u)\n", CONFIG_APP_LOGICAL_PORTS_M7 - 1);
    shell_printf(shell, "parameters (logical port N):\n");
    shell_printf(shell, "    hw_addr: \n");
    shell_printf(shell, "        format: XX:XX:XX:XX:XX:XX (X is an hexadecimal digit)\n");
    shell_printf(shell, "        description: MAC address\n");
    shell_printf(shell, "    ip_addr: \n");
    shell_printf(shell, "        format: a.b.c.d (dotted decimal format)\n");
    shell_printf(shell, "        description: IP address\n");
    shell_printf(shell, "    gw_addr: \n");
    shell_printf(shell, "        format: a.b.c.d (dotted decimal format)\n");
    shell_printf(shell, "        description: Gateway IP address\n");
    shell_printf(shell, "    net_mask: \n");
    shell_printf(shell, "        format: a.b.c.d (dotted decimal format)\n");
    shell_printf(shell, "        description: Network mask\n");
}

static int system_cfg_get_m7_net_params(unsigned int port_id, struct net_config *net)
{
    if (port_id >= CONFIG_APP_LOGICAL_PORTS_M7)
        goto err;

    __system_config_get_net("/m7/port", port_id, net);

    return 0;

err:
    return -1;
}

static void system_cfg_get_m7_tsn_params(struct rtos_apps_tsn_config *tsn)
{
    __system_config_get_tsn_app("/m7/tsn_app", tsn);
}

static void system_cfg_get_m7_params(struct system_config_m7 *sys_cfg)
{
    for (int i = 0; i < CONFIG_APP_LOGICAL_PORTS_M7; i++)
        system_cfg_get_m7_net_params(i, sys_cfg->net);

    system_cfg_get_m7_tsn_params(&sys_cfg->app.tsn_app_config);
}

void shared_system_config_set(void)
{
    struct system_config_m7 *__sys_cfg = &m7_system_cfg;

    system_cfg_get_m7_params(__sys_cfg);

    memcpy(shared_system_cfg, __sys_cfg, sizeof(*__sys_cfg));
}
