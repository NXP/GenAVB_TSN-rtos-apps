/*
 * Copyright 2023-2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "shell.h"
#include "shell_config.h"
#include "genavb/psfp.h"
#include "psfp.h"
#include "rtos_apps/shell/psfp.h"

SHELL_WRAPPER(cmd_sf_update);
SHELL_WRAPPER(cmd_sf_delete);
SHELL_WRAPPER(cmd_sf_read);
SHELL_WRAPPER(cmd_sg_update);
SHELL_WRAPPER(cmd_sg_delete);
SHELL_WRAPPER(cmd_sg_read);
SHELL_WRAPPER(cmd_fm_update);
SHELL_WRAPPER(cmd_fm_delete);
SHELL_WRAPPER(cmd_fm_read);

SHELL_COMMAND_DEFINE(sf_update,
                     "\nsf_update " CMD_SF_UPDATE_HELP "\n",
                     &_cmd_sf_update,
                     SHELL_IGNORE_PARAMETER_COUNT);
SHELL_COMMAND_DEFINE(sf_delete,
                     "\nsf_delete " CMD_SF_DELETE_HELP "\n",
                     &_cmd_sf_delete,
                     SHELL_IGNORE_PARAMETER_COUNT);
SHELL_COMMAND_DEFINE(sf_read,
                     "\nsf_read " CMD_SF_READ_HELP "\n",
                     &_cmd_sf_read,
                     SHELL_IGNORE_PARAMETER_COUNT);

SHELL_COMMAND_DEFINE(sg_update,
                     "\nsg_update " CMD_SG_UPDATE_HELP "\n",
                     &_cmd_sg_update,
                     SHELL_IGNORE_PARAMETER_COUNT);
SHELL_COMMAND_DEFINE(sg_delete,
                     "\nsg_delete " CMD_SG_DELETE_HELP "\n",
                     &_cmd_sg_delete,
                     SHELL_IGNORE_PARAMETER_COUNT);
SHELL_COMMAND_DEFINE(sg_read,
                     "\nsg_read " CMD_SG_READ_HELP "\n",
                     &_cmd_sg_read,
                     SHELL_IGNORE_PARAMETER_COUNT);

SHELL_COMMAND_DEFINE(fm_update,
                     "\nfm_update " CMD_FM_UPDATE_HELP "\n",
                     &_cmd_fm_update,
                     SHELL_IGNORE_PARAMETER_COUNT);
SHELL_COMMAND_DEFINE(fm_delete,
                     "\nfm_delete " CMD_FM_DELETE_HELP "\n",
                     &_cmd_fm_delete,
                     SHELL_IGNORE_PARAMETER_COUNT);
SHELL_COMMAND_DEFINE(fm_read,
                     "\nfm_read " CMD_FM_READ_HELP "\n",
                     &_cmd_fm_read,
                     SHELL_IGNORE_PARAMETER_COUNT);

void help_config_sg_storage(shell_handle_t shell)
{
    shell_printf(shell, "\nstream gate storage configuration\n\n");
    shell_printf(shell, "path: /sg\n");
    shell_printf(shell, "    description: directory used by sg_update/sg_read commands\n");
    shell_printf(shell, "path: /sg/N (N: stream gate instance index, 0 to %u)\n", genavb_stream_gate_get_max_entries() - 1 );
    shell_printf(shell, "parameters (instance index N):\n");
    shell_printf(shell, "    base_time:\n");
    shell_printf(shell, "        value: 0 to (2^64 - 1)\n");
    shell_printf(shell, "        description: 64bits absolute gPTP time. If in the past, actual base time' = base time + N * cycle_time\n");
    shell_printf(shell, "    admin_state:\n");
    shell_printf(shell, "        value: 0 to 1\n");
    shell_printf(shell, "        description: administrative gate state 0: closed, 1: opened\n");
    shell_printf(shell, "    admin_ipv:\n");
    shell_printf(shell, "        value: 0 to 7 or 0xff\n");
    shell_printf(shell, "        description: administrative internal priority value in decimal\n");
    shell_printf(shell, "    cycle_time:\n");
    shell_printf(shell, "        value: 0 to 4294967295\n");
    shell_printf(shell, "        description: 32bits cycle time in nanoseconds\n");
    shell_printf(shell, "    cycle_time_ext:\n");
    shell_printf(shell, "        value: 0 to 4294967295\n");
    shell_printf(shell, "        description: 32bits cycle time extension in nanoseconds\n");
    shell_printf(shell, "    gate_closed_due_to_invalid_rx_enable:\n");
    shell_printf(shell, "        value: 0 or 1\n");
    shell_printf(shell, "        description: gate closed due to invalid rx enable\n");
    shell_printf(shell, "    gate_closed_due_to_octets_exceeded_enable:\n");
    shell_printf(shell, "        value: 0 or 1\n");
    shell_printf(shell, "        description: gate closed due to octets exceeded enable\n");
    shell_printf(shell, "    list_length:\n");
    shell_printf(shell, "        value: 0 to %u\n", genavb_stream_gate_control_get_max_entries());
    shell_printf(shell, "        description: gate control list length\n");
    shell_printf(shell, "    entryM: (M: entry id, 0 to %u)\n", genavb_stream_gate_control_get_max_entries() - 1);
    shell_printf(shell, "        format: state,ipv,interval,octet (e.g 1,7,20000,1000000)\n");
    shell_printf(shell, "        value: 0 to 1, 0 to 7 or 0xff, 0 to 4294967295, 0 to 4294967295\n");
    shell_printf(shell, "        description: gate list entry\n");
    shell_printf(shell, "            state: gate state 0: closed, 1: opened\n");
    shell_printf(shell, "            ipv: internal priority value in decimal\n");
    shell_printf(shell, "            interval: 32bits gate interval in nanoseconds\n");
    shell_printf(shell, "            octet: 32bits max octet interval in bytes\n");
}

void help_config_fm_storage(shell_handle_t shell)
{
    shell_printf(shell, "\nflow meter storage configuration\n\n");
    shell_printf(shell, "path: /fm\n");
    shell_printf(shell, "    description: directory used by fm_update/fm_read commands\n");
    shell_printf(shell, "path: /fm/N (N: flow meter instance index, 0 to %u)\n", genavb_flow_meter_get_max_entries() - 1 );
    shell_printf(shell, "parameters (instance index N):\n");
    shell_printf(shell, "    committed_information_rate:\n");
    shell_printf(shell, "        value: 0 to (2^64 - 1)\n");
    shell_printf(shell, "        description: committed rate in unit of bits per sec\n");
    shell_printf(shell, "    committed_burst_size:\n");
    shell_printf(shell, "        value: 0 to 4294967295\n");
    shell_printf(shell, "        description: committed burst size in bytes\n");
    shell_printf(shell, "    excess_information_rate:\n");
    shell_printf(shell, "        value: 0 to (2^64 - 1)\n");
    shell_printf(shell, "        description: excess rate in unit of bits per sec\n");
    shell_printf(shell, "    excess_burst_size:\n");
    shell_printf(shell, "        value: 0 to 4294967295\n");
    shell_printf(shell, "        description: excess burst size in bytes\n");
    shell_printf(shell, "    coupling_flag:\n");
    shell_printf(shell, "        value: 0 or 1\n");
    shell_printf(shell, "        description: coupling flag\n");
    shell_printf(shell, "    color_mode:\n");
    shell_printf(shell, "        value: 0 or 1\n");
    shell_printf(shell, "        description: color mode\n");
    shell_printf(shell, "    drop_on_yellow:\n");
    shell_printf(shell, "        value: 0 or 1\n");
    shell_printf(shell, "        description: drop on yellow\n");
    shell_printf(shell, "    mark_all_frames_red_enable:\n");
    shell_printf(shell, "        value: 0 or 1\n");
    shell_printf(shell, "        description: mark all frames red enable\n");
}

void help_config_psfp(shell_handle_t shell)
{
    shell_printf(shell, (SHELL_COMMAND(sf_update))->pcHelpString);
    shell_printf(shell, (SHELL_COMMAND(sf_delete))->pcHelpString);
    shell_printf(shell, (SHELL_COMMAND(sf_read))->pcHelpString);

    shell_printf(shell, (SHELL_COMMAND(sg_update))->pcHelpString);
    shell_printf(shell, (SHELL_COMMAND(sg_delete))->pcHelpString);
    shell_printf(shell, (SHELL_COMMAND(sg_read))->pcHelpString);

    help_config_sg_storage(shell);

    shell_printf(shell, (SHELL_COMMAND(fm_update))->pcHelpString);
    shell_printf(shell, (SHELL_COMMAND(fm_delete))->pcHelpString);
    shell_printf(shell, (SHELL_COMMAND(fm_read))->pcHelpString);

    help_config_fm_storage(shell);
}

void psfp_init_shell(shell_handle_t shell)
{
    SHELL_RegisterCommand(shell, SHELL_COMMAND(sf_update));
    SHELL_RegisterCommand(shell, SHELL_COMMAND(sf_delete));
    SHELL_RegisterCommand(shell, SHELL_COMMAND(sf_read));

    SHELL_RegisterCommand(shell, SHELL_COMMAND(sg_update));
    SHELL_RegisterCommand(shell, SHELL_COMMAND(sg_delete));
    SHELL_RegisterCommand(shell, SHELL_COMMAND(sg_read));

    SHELL_RegisterCommand(shell, SHELL_COMMAND(fm_update));
    SHELL_RegisterCommand(shell, SHELL_COMMAND(fm_delete));
    SHELL_RegisterCommand(shell, SHELL_COMMAND(fm_read));

    cmd_psfp_init(shell);
}
