/*
 * Copyright 2022-2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "qbv.h"

#ifdef CONFIG_APP_QBV

#include "shell.h"
#include "shell_config.h"
#include "rtos_apps/shell/qbv.h"
#include "genavb_sdk.h"
#include "genavb/qos.h"

SHELL_WRAPPER(cmd_qbv_set);
SHELL_WRAPPER(cmd_qbv_get);
SHELL_WRAPPER(cmd_qbv_disable);
SHELL_WRAPPER(cmd_qbv_set_max_sdu);
SHELL_WRAPPER(cmd_qbv_get_max_sdu);

SHELL_COMMAND_DEFINE(qbv_set,
                     "\nqbv_set " CMD_QBV_SET_HELP "\n",
                     &_cmd_qbv_set,
                     SHELL_IGNORE_PARAMETER_COUNT);
SHELL_COMMAND_DEFINE(qbv_get,
                     "\nqbv_get " CMD_QBV_GET_HELP "\n",
                     &_cmd_qbv_get,
                     SHELL_IGNORE_PARAMETER_COUNT);
SHELL_COMMAND_DEFINE(qbv_disable,
                     "\nqbv_disable " CMD_QBV_DISABLE_HELP "\n",
                     &_cmd_qbv_disable,
                     SHELL_IGNORE_PARAMETER_COUNT);
SHELL_COMMAND_DEFINE(qbv_set_max_sdu,
                     "\nqbv_set_max_sdu " CMD_QBV_SET_MAX_SDU_HELP "\n",
                     &_cmd_qbv_set_max_sdu,
                     SHELL_IGNORE_PARAMETER_COUNT);
SHELL_COMMAND_DEFINE(qbv_get_max_sdu,
                     "\nqbv_get_max_sdu " CMD_QBV_GET_MAX_SDU_HELP "\n",
                     &_cmd_qbv_get_max_sdu,
                     SHELL_IGNORE_PARAMETER_COUNT);

void help_config_qbv(shell_handle_t shell)
{
    shell_printf(shell, "qbv config\n\n");
    shell_printf(shell, "path: /qbv\n");
    shell_printf(shell, "    description: directory used by qbv_set/qbv_get commands\n");
    shell_printf(shell, "path: /qbv/portN (N: logical port index, 0 to %u)\n", CONFIG_APP_LOGICAL_PORTS - 1);
    shell_printf(shell, "parameters (logical port N):\n");
    shell_printf(shell, "    base_time:\n");
    shell_printf(shell, "        value: 0 to (2^64 - 1)\n");
    shell_printf(shell, "        description: 64 bit absolute gPTP time. If in the past, actual base time' = base time + N * cycle_time\n");
    shell_printf(shell, "    cycle_time:\n");
    shell_printf(shell, "        value: 0 to 4294967295\n");
    shell_printf(shell, "        description: 32bit cycle time in nanoseconds\n");
    shell_printf(shell, "    cycle_time_ext:\n");
    shell_printf(shell, "        value: 0 to 4294967295\n");
    shell_printf(shell, "        description: 32bit cycle time extension in nanoseconds\n");
    shell_printf(shell, "    entryM: (M: entry id, 0 to %u)\n", QBV_LIST_MAX_ENTRIES - 1);
    shell_printf(shell, "        format: mask,interval<,operation> (e.g f4,20000<,0>)\n");
    shell_printf(shell, "        value: 00 to ff, 0 to 4294967295<, 0 to 2>\n");
    shell_printf(shell, "        description: gate list entry\n");
    shell_printf(shell, "            mask: gate state bitmask in hexadecimal\n");
    shell_printf(shell, "            interval: 32bit gate interval in nanoseconds\n");
    shell_printf(shell, "            <operation>: gate operation, 0: SetGateStates, 1: Set-And-Hold-MAC, 2: Set-And-Release-MAC, default: 0\n");
    shell_printf(shell, "\n");
    shell_printf(shell, "path: /qbv/portN/max_sdu\n");
    shell_printf(shell, "    description: directory used by qbv_set_max_sdu/qbv_get_max_sdu commands\n");
    shell_printf(shell, "parameters (logical port N):\n");
    shell_printf(shell, "    qM (M: traffic class number, 0 to %u):\n", QOS_TRAFFIC_CLASS_MAX - 1);
    shell_printf(shell, "        value: 0 to 2000\n");
    shell_printf(shell, "        description: 32bit traffic class maximum Service Data Unit size for each queue\n");
}

void qbv_init(shell_handle_t shell)
{
    SHELL_RegisterCommand(shell, SHELL_COMMAND(qbv_set));
    SHELL_RegisterCommand(shell, SHELL_COMMAND(qbv_get));
    SHELL_RegisterCommand(shell, SHELL_COMMAND(qbv_disable));
    SHELL_RegisterCommand(shell, SHELL_COMMAND(qbv_set_max_sdu));
    SHELL_RegisterCommand(shell, SHELL_COMMAND(qbv_get_max_sdu));

    cmd_qbv_init(shell);
}

#else
void qbv_init(shell_handle_t shell) {return;}
#endif /* CONFIG_APP_QBV */
