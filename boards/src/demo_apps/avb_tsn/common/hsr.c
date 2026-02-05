/*
 * Copyright 2023-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <ctype.h>
#include <getopt.h>

#include "genavb/helpers.h"
#include "genavb/error.h"
#include "genavb/hsr.h"
#include "storage.h"
#include "genavb.h"
#include "hsr.h"

#define BUF_MAX_SIZE 5

static shell_status_t hsr_mode_set(shell_handle_t shell, int32_t argc, char **argv);

SHELL_COMMAND_DEFINE(hsr_mode_set,
                     "\nhsr_mode_set <mode> [-p]\n"
                     "    parameters:\n"
                     "        mode: HSR node operation mode\n"
                     "        	    0: Mode H(HSR-tagged forwarding)\n"
                     "        	    1: Mode N(No forwarding)\n"
                     "        	    2: Mode T(Transparent forwarding)\n"
                     "        	    3: Mode U(Unicast forwarding)\n"
                     "    options:\n"
                     "        -p: update mode in permanent database\n",
                     &hsr_mode_set,
                     SHELL_IGNORE_PARAMETER_COUNT);

void help_config_hsr(shell_handle_t shell)
{
    shell_printf(shell, (SHELL_COMMAND(hsr_mode_set))->pcHelpString);
}

static void print_hsr_mode_set_usage(shell_handle_t shell)
{
    shell_printf(shell, "Usage: ");
    shell_printf(shell, (SHELL_COMMAND(hsr_mode_set))->pcHelpString);
}

static int hsr_mode_update_permanent(unsigned long mode)
{
    char buf[BUF_MAX_SIZE];

    if (h_snprintf_strict(buf, BUF_MAX_SIZE, "%u", mode) < 0)
        goto err;

    if (storage_mkdir("/hsr", true) < 0)
        goto err;

    if (storage_cd("/hsr", true) < 0)
        goto err;

    storage_write("mode", buf, strlen(buf));

    storage_cd("-", true);

    return 0;

err:
    return -1;
}

static void hsr_mode_apply_permanent(shell_handle_t shell)
{
    struct genavb_handle *genavb_handle = get_genavb_handle();
    genavb_hsr_mode_t mode;
    uint32_t tmp = 0;
    int ret;

    if (storage_read_u32("/hsr/mode", &tmp) < 0)
        return;

    if (tmp > 3) {
        shell_printf(shell, "invalid mode value in permanent\n");
        return;
    }

    mode = (genavb_hsr_mode_t)tmp;

    ret = genavb_hsr_operation_mode_set(genavb_handle, mode);
    if (ret < 0) {
        shell_printf(shell, "genavb_hsr_operation_mode_set(%u) failed %s\n", mode, genavb_strerror(ret));
        return;
    }

    shell_printf(shell, "HSR operation mode %u set success\n", mode);
}

static shell_status_t hsr_mode_set(shell_handle_t shell, int32_t argc, char **argv)
{
    struct genavb_handle *genavb_handle = get_genavb_handle();
    bool permanent = false;
    genavb_hsr_mode_t mode;
    unsigned long tmp;
    int ret, opt;

    if (argc < 2)
        goto err_usage;

    h_strtoul(&tmp, argv[1], NULL, 0);
    if (tmp > 3) {
        shell_printf(shell, "invalid mode value\n");
        goto err_usage;
    }

    optind = 2;
    while ((opt = getopt(argc, argv, "p")) != -1) {
        switch (opt) {
        case 'p':
            permanent = true;
            break;
        default:
            break;
        }
    }

    mode = (genavb_hsr_mode_t)tmp;

    ret = genavb_hsr_operation_mode_set(genavb_handle, mode);
    if (ret < 0) {
        shell_printf(shell, "genavb_hsr_operation_mode_set(%u) failed %s\n", mode, genavb_strerror(ret));
        goto err;
    }

    if (permanent)
        if (hsr_mode_update_permanent(tmp) < 0)
            shell_printf(shell, "hsr_mode_update_permanent(%u) failed\n", tmp);

    shell_printf(shell, "HSR operation mode %u set success\n", mode);

    return kStatus_SHELL_Success;

err_usage:
    print_hsr_mode_set_usage(shell);
err:
    return kStatus_SHELL_Error;
}

void hsr_init_shell(shell_handle_t shell)
{
    SHELL_RegisterCommand(shell, SHELL_COMMAND(hsr_mode_set));

    hsr_mode_apply_permanent(shell);
}
