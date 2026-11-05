/*
 * Copyright 2023-2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "shell.h"
#include "shell_config.h"
#include "frer.h"
#include "rtos_apps/shell/frer.h"

SHELL_WRAPPER(cmd_seqg_update);
SHELL_WRAPPER(cmd_seqg_delete);
SHELL_WRAPPER(cmd_seqg_read);
SHELL_WRAPPER(cmd_seqr_update);
SHELL_WRAPPER(cmd_seqr_delete);
SHELL_WRAPPER(cmd_seqr_read);
SHELL_WRAPPER(cmd_seqi_update);
SHELL_WRAPPER(cmd_seqi_delete);
SHELL_WRAPPER(cmd_seqi_read);

SHELL_COMMAND_DEFINE(seqg_update,
                     "\nseqg_update " CMD_SEQG_UPDATE_HELP "\n",
                     &_cmd_seqg_update,
                     SHELL_IGNORE_PARAMETER_COUNT);

SHELL_COMMAND_DEFINE(seqg_delete,
                     "\nseqg_delete " CMD_SEQG_DELETE_HELP "\n",
                     &_cmd_seqg_delete,
                     SHELL_IGNORE_PARAMETER_COUNT);

SHELL_COMMAND_DEFINE(seqg_read,
                     "\nseqg_read " CMD_SEQG_READ_HELP "\n",
                     &_cmd_seqg_read,
                     SHELL_IGNORE_PARAMETER_COUNT);

SHELL_COMMAND_DEFINE(seqr_update,
                     "\nseqr_update " CMD_SEQR_UPDATE_HELP "\n",
                     &_cmd_seqr_update,
                     SHELL_IGNORE_PARAMETER_COUNT);

SHELL_COMMAND_DEFINE(seqr_delete,
                     "\nseqr_delete " CMD_SEQR_DELETE_HELP "\n",
                     &_cmd_seqr_delete,
                     SHELL_IGNORE_PARAMETER_COUNT);

SHELL_COMMAND_DEFINE(seqr_read,
                     "\nseqr_read " CMD_SEQR_READ_HELP "\n",
                     &_cmd_seqr_read,
                     SHELL_IGNORE_PARAMETER_COUNT);

SHELL_COMMAND_DEFINE(seqi_update,
                     "\nseqi_update " CMD_SEQI_UPDATE_HELP "\n",
                     &_cmd_seqi_update,
                     SHELL_IGNORE_PARAMETER_COUNT);

SHELL_COMMAND_DEFINE(seqi_delete,
                     "\nseqi_delete " CMD_SEQI_DELETE_HELP "\n",
                     &_cmd_seqi_delete,
                     SHELL_IGNORE_PARAMETER_COUNT);

SHELL_COMMAND_DEFINE(seqi_read,
                     "\nseqi_read " CMD_SEQI_READ_HELP "\n",
                     &_cmd_seqi_read,
                     SHELL_IGNORE_PARAMETER_COUNT);

void help_config_frer(shell_handle_t shell)
{
    shell_printf(shell, (SHELL_COMMAND(seqg_update))->pcHelpString);
    shell_printf(shell, (SHELL_COMMAND(seqg_delete))->pcHelpString);
    shell_printf(shell, (SHELL_COMMAND(seqg_read))->pcHelpString);

    shell_printf(shell, (SHELL_COMMAND(seqr_update))->pcHelpString);
    shell_printf(shell, (SHELL_COMMAND(seqr_delete))->pcHelpString);
    shell_printf(shell, (SHELL_COMMAND(seqr_read))->pcHelpString);

    shell_printf(shell, (SHELL_COMMAND(seqi_update))->pcHelpString);
    shell_printf(shell, (SHELL_COMMAND(seqi_delete))->pcHelpString);
    shell_printf(shell, (SHELL_COMMAND(seqi_read))->pcHelpString);
}

void frer_init_shell(shell_handle_t shell)
{
    SHELL_RegisterCommand(shell, SHELL_COMMAND(seqg_update));
    SHELL_RegisterCommand(shell, SHELL_COMMAND(seqg_delete));
    SHELL_RegisterCommand(shell, SHELL_COMMAND(seqg_read));

    SHELL_RegisterCommand(shell, SHELL_COMMAND(seqr_update));
    SHELL_RegisterCommand(shell, SHELL_COMMAND(seqr_delete));
    SHELL_RegisterCommand(shell, SHELL_COMMAND(seqr_read));

    SHELL_RegisterCommand(shell, SHELL_COMMAND(seqi_update));
    SHELL_RegisterCommand(shell, SHELL_COMMAND(seqi_delete));
    SHELL_RegisterCommand(shell, SHELL_COMMAND(seqi_read));

    cmd_frer_init(shell);
}
