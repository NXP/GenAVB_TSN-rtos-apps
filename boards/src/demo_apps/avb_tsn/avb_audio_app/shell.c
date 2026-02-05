/*
 * Copyright 2020, 2022, 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "avb_tsn/common/shell.h"

/* The specific config help for application */
void help_config_avdecc(shell_handle_t shell)
{
    shell_printf(shell, "avdecc config\n\n");
    shell_printf(shell, "path: /avdecc\n");
    shell_printf(shell, "parameters:\n");
    shell_printf(shell, "    btb_mode: \n");
    shell_printf(shell, "        value: 0 or 1\n");
    shell_printf(shell, "        description: Set 1 to enable fast-connect btb mode\n");
    shell_printf(shell, "    talker_id: \n");
    shell_printf(shell, "        format: 0xaaaabbbbccccdddd (64 bits hexadecimal)\n");
    shell_printf(shell, "        description: Talker entity id, if fast-connect mode is\n");
    shell_printf(shell, "                     enabled, this field selects talker endpoint\n");
}

void help_config_app(shell_handle_t shell)
{
    shell_printf(shell, "app config\n");
    shell_printf(shell, "path: /avb_app\n");
    shell_printf(shell, "parameters:\n");
    shell_printf(shell, "    mclock_role: \n");
    shell_printf(shell, "        value: 0 to 1\n");
    shell_printf(shell, "        description: media clock role\n");
    shell_printf(shell, "                     0 MEDIA_CLOCK_MASTER\n");
    shell_printf(shell, "                     1 MEDIA_CLOCK_SLAVE\n");
}
