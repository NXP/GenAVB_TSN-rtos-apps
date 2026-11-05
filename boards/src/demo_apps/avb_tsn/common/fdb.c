/*
 * Copyright 2022-2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "shell.h"
#include "shell_config.h"
#include "fdb.h"
#include "rtos_apps/shell/fdb.h"

SHELL_WRAPPER(cmd_fdb_update);
SHELL_WRAPPER(cmd_fdb_delete);
SHELL_WRAPPER(cmd_fdb_read);
SHELL_WRAPPER(cmd_fdb_dump);

SHELL_COMMAND_DEFINE(fdb_update,
                     "\nfdb_update " CMD_FDB_UPDATE_HELP "\n",
                     &_cmd_fdb_update,
                     SHELL_IGNORE_PARAMETER_COUNT);
SHELL_COMMAND_DEFINE(fdb_delete,
                     "\nfdb_delete " CMD_FDB_DELETE_HELP "\n",
                     &_cmd_fdb_delete,
                     SHELL_IGNORE_PARAMETER_COUNT);
SHELL_COMMAND_DEFINE(fdb_read,
                     "\nfdb_read " CMD_FDB_READ_HELP "\n",
                     &_cmd_fdb_read,
                     SHELL_IGNORE_PARAMETER_COUNT);
SHELL_COMMAND_DEFINE(fdb_dump,
                     "\nfdb_dump " CMD_FDB_DUMP_HELP "\n",
                     &_cmd_fdb_dump,
                     SHELL_IGNORE_PARAMETER_COUNT);

void help_config_fdb(shell_handle_t shell)
{
    shell_printf(shell, (SHELL_COMMAND(fdb_update))->pcHelpString);
    shell_printf(shell, (SHELL_COMMAND(fdb_delete))->pcHelpString);
    shell_printf(shell, (SHELL_COMMAND(fdb_read))->pcHelpString);
    shell_printf(shell, (SHELL_COMMAND(fdb_dump))->pcHelpString);
}

void fdb_init_shell(shell_handle_t shell)
{
    SHELL_RegisterCommand(shell, SHELL_COMMAND(fdb_update));
    SHELL_RegisterCommand(shell, SHELL_COMMAND(fdb_delete));
    SHELL_RegisterCommand(shell, SHELL_COMMAND(fdb_read));
    SHELL_RegisterCommand(shell, SHELL_COMMAND(fdb_dump));

    cmd_fdb_init(shell);
}
