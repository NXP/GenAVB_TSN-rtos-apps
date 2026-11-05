/*
 * Copyright 2023-2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fp.h"

#ifdef CONFIG_APP_FP

#include "shell.h"
#include "shell_config.h"
#include "rtos_apps/shell/fp.h"

SHELL_WRAPPER(cmd_fp_get);
SHELL_WRAPPER(cmd_fp_set);

SHELL_COMMAND_DEFINE(fp_set,
                     "\nfp_set " CMD_FP_SET_HELP "\n",
                    &_cmd_fp_set,
                    SHELL_IGNORE_PARAMETER_COUNT);

SHELL_COMMAND_DEFINE(fp_get,
                     "\nfp_get " CMD_FP_GET_HELP "\n",
                    &_cmd_fp_get,
                    SHELL_IGNORE_PARAMETER_COUNT);

void help_config_fp(shell_handle_t shell) {
    shell_printf(shell, "fp config\n\n");
    shell_printf(shell, "path: /fp\n");
    shell_printf(shell, "   description: directory used by fp_set/fp_get commands\n");
    shell_printf(shell, "path: /fp/portN (N: logical port index, 0 to %u)\n", CONFIG_APP_LOGICAL_PORTS - 1);
    shell_printf(shell, "   commands (logical port N):\n");
    shell_printf(shell, "   ");
    shell_printf(shell, (SHELL_COMMAND(fp_set))->pcHelpString);
    shell_printf(shell, "   ");
    shell_printf(shell, (SHELL_COMMAND(fp_get))->pcHelpString);
}

void fp_init(shell_handle_t shell)
{
    SHELL_RegisterCommand(shell, SHELL_COMMAND(fp_set));
    SHELL_RegisterCommand(shell, SHELL_COMMAND(fp_get));

    cmd_fp_init(shell);
}

#else
void fp_init(shell_handle_t shell) {return;}
#endif /* CONFIG_APP_FP */
