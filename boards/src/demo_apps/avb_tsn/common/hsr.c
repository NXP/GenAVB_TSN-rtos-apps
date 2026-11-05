/*
 * Copyright 2023-2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "shell.h"
#include "shell_config.h"
#include "hsr.h"
#include "rtos_apps/shell/hsr.h"

SHELL_WRAPPER(cmd_hsr_mode_set);

SHELL_COMMAND_DEFINE(hsr_mode_set,
                     "\nhsr_mode_set " CMD_HSR_MODE_SET_HELP "\n",
                     &_cmd_hsr_mode_set,
                     SHELL_IGNORE_PARAMETER_COUNT);

void help_config_hsr(shell_handle_t shell)
{
    shell_printf(shell, (SHELL_COMMAND(hsr_mode_set))->pcHelpString);
}

void hsr_init_shell(shell_handle_t shell)
{
    SHELL_RegisterCommand(shell, SHELL_COMMAND(hsr_mode_set));

    cmd_hsr_init(shell);
}
