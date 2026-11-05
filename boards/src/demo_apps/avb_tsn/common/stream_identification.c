/*
 * Copyright 2023-2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "shell.h"
#include "shell_config.h"
#include "stream_identification.h"
#include "rtos_apps/shell/stream_identification.h"

SHELL_WRAPPER(cmd_si_read);
SHELL_WRAPPER(cmd_si_delete);
SHELL_WRAPPER(cmd_si_update);

SHELL_COMMAND_DEFINE(si_update,
                     "\nsi_update " CMD_SI_UPDATE_HELP "\n",
                     &_cmd_si_update,
                     SHELL_IGNORE_PARAMETER_COUNT);

SHELL_COMMAND_DEFINE(si_read,
                     "\nsi_read " CMD_SI_READ_HELP "\n",
                     &_cmd_si_read,
                     SHELL_IGNORE_PARAMETER_COUNT);

SHELL_COMMAND_DEFINE(si_delete,
                     "\nsi_delete " CMD_SI_DELETE_HELP "\n",
                     &_cmd_si_delete,
                     SHELL_IGNORE_PARAMETER_COUNT);

void help_config_si(shell_handle_t shell)
{
    shell_printf(shell, (SHELL_COMMAND(si_update))->pcHelpString);
    shell_printf(shell, (SHELL_COMMAND(si_delete))->pcHelpString);
    shell_printf(shell, (SHELL_COMMAND(si_read))->pcHelpString);
}

void stream_identification_init_shell(shell_handle_t shell)
{
    SHELL_RegisterCommand(shell, SHELL_COMMAND(si_update));
    SHELL_RegisterCommand(shell, SHELL_COMMAND(si_read));
    SHELL_RegisterCommand(shell, SHELL_COMMAND(si_delete));

    cmd_stream_identification_init(shell);
}
