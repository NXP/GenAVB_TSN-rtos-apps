/*
 * Copyright 2022-2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "shell.h"
#include "shell_config.h"
#include "vlan.h"
#include "rtos_apps/shell/vlan.h"

SHELL_WRAPPER(cmd_vlan_update);
SHELL_WRAPPER(cmd_vlan_delete);
SHELL_WRAPPER(cmd_vlan_read);
SHELL_WRAPPER(cmd_vlan_dump);
SHELL_WRAPPER(cmd_vlan_set_pvid);
SHELL_WRAPPER(cmd_vlan_get_pvid);

SHELL_COMMAND_DEFINE(vlan_update,
                     "\nvlan_update " CMD_VLAN_UPDATE_HELP "\n",
                     &_cmd_vlan_update,
                     SHELL_IGNORE_PARAMETER_COUNT);
SHELL_COMMAND_DEFINE(vlan_delete,
                     "\nvlan_delete " CMD_VLAN_DELETE_HELP "\n",
                     &_cmd_vlan_delete,
                     SHELL_IGNORE_PARAMETER_COUNT);
SHELL_COMMAND_DEFINE(vlan_read,
                     "\nvlan_read " CMD_VLAN_READ_HELP "\n",
                     &_cmd_vlan_read,
                     SHELL_IGNORE_PARAMETER_COUNT);
SHELL_COMMAND_DEFINE(vlan_dump,
                     "\nvlan_dump " CMD_VLAN_DUMP_HELP "\n",
                     &_cmd_vlan_dump,
                     SHELL_IGNORE_PARAMETER_COUNT);
SHELL_COMMAND_DEFINE(vlan_set_pvid,
                     "\nvlan_set_pvid " CMD_VLAN_SET_PVID_HELP "\n",
                     &_cmd_vlan_set_pvid,
                     SHELL_IGNORE_PARAMETER_COUNT);
SHELL_COMMAND_DEFINE(vlan_get_pvid,
                     "\nvlan_get_pvid " CMD_VLAN_GET_PVID_HELP "\n",
                     &_cmd_vlan_get_pvid,
                     SHELL_IGNORE_PARAMETER_COUNT);

void help_config_vlan(shell_handle_t shell)
{
    shell_printf(shell, (SHELL_COMMAND(vlan_update))->pcHelpString);
    shell_printf(shell, (SHELL_COMMAND(vlan_delete))->pcHelpString);
    shell_printf(shell, (SHELL_COMMAND(vlan_read))->pcHelpString);
    shell_printf(shell, (SHELL_COMMAND(vlan_dump))->pcHelpString);
    shell_printf(shell, (SHELL_COMMAND(vlan_set_pvid))->pcHelpString);
    shell_printf(shell, (SHELL_COMMAND(vlan_get_pvid))->pcHelpString);
}

void vlan_init_shell(shell_handle_t shell)
{
    SHELL_RegisterCommand(shell, SHELL_COMMAND(vlan_update));
    SHELL_RegisterCommand(shell, SHELL_COMMAND(vlan_delete));
    SHELL_RegisterCommand(shell, SHELL_COMMAND(vlan_read));
    SHELL_RegisterCommand(shell, SHELL_COMMAND(vlan_dump));
    SHELL_RegisterCommand(shell, SHELL_COMMAND(vlan_set_pvid));
    SHELL_RegisterCommand(shell, SHELL_COMMAND(vlan_get_pvid));

    cmd_vlan_init(shell);
}
