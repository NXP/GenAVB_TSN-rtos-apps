/*
 * Copyright 2019-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <getopt.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

#include "fsl_debug_console.h"
#include "fsl_shell.h"

#include "genavb/helpers.h"
#include "genavb/stats.h"

#include "board.h"
#include "genavb.h"
#include "log.h"
#include "shell.h"
#include "storage.h"

#define SHELL_TASK_NAME         "shell"
#define __SHELL_TASK_STACK_SIZE BOARD_SHELL_TASK_STACK_SIZE
#define __SHELL_TASK_PRIORITY   4U

#define SHELL_PROMPT_LEN 20

struct shell_ctx {
    uint8_t shell_handle[SHELL_HANDLE_SIZE];
    TaskHandle_t task_handle;
    void (*init)(shell_handle_t shell);
};

static struct shell_ctx shell_ctx;
static char prompt[SHELL_PROMPT_LEN];

static shell_status_t write(shell_handle_t shell, int32_t argc, char **argv);
static shell_status_t cat(shell_handle_t shell, int32_t argc, char **argv);
static shell_status_t ls(shell_handle_t shell, int32_t argc, char **argv);
static shell_status_t rm(shell_handle_t shell, int32_t argc, char **argv);
static shell_status_t cd(shell_handle_t shell, int32_t argc, char **argv);
static shell_status_t pwd(shell_handle_t shell, int32_t argc, char **argv);
static shell_status_t mkdir(shell_handle_t shell, int32_t argc, char **argv);
static shell_status_t help_config(shell_handle_t shell, int32_t argc, char **argv);
static shell_status_t log(shell_handle_t shell, int32_t argc, char **argv);
static shell_status_t port_stats(shell_handle_t shell, int32_t argc, char **argv);

SHELL_COMMAND_DEFINE(write,
                     "\nwrite file string\n",
                     &write,
                     2);

SHELL_COMMAND_DEFINE(cat,
                     "\ncat file\n",
                     &cat,
                     1);

SHELL_COMMAND_DEFINE(ls,
                     "\nls [<file/dir>]\n",
                     &ls,
                     SHELL_IGNORE_PARAMETER_COUNT);

SHELL_COMMAND_DEFINE(rm,
                     "\nrm [-r] [-f] <file/dir>\n",
                     &rm,
                     SHELL_IGNORE_PARAMETER_COUNT);

SHELL_COMMAND_DEFINE(cd,
                     "\ncd dir\n",
                     &cd,
                     1);

SHELL_COMMAND_DEFINE(pwd,
                     "\npwd\n",
                     &pwd,
                     0);

SHELL_COMMAND_DEFINE(mkdir,
                     "\nmkdir [-p] <dir>\n",
                     &mkdir,
                     SHELL_IGNORE_PARAMETER_COUNT);

SHELL_COMMAND_DEFINE(help_config,
                     "\nhelp_config [multicore | params_multicore | app | port | management | fgptp | srp | avdecc | qbv | fdb | vlan | si | psfp | frer | fp]\n",
                     &help_config,
                     1);

SHELL_COMMAND_DEFINE(log,
                     "\nlog <component_id> <level>\n"
                     "    <component_id>\n"
                     "        all, app, avtp, avdecc, srp, maap, common, os, fgptp, api or mgmt\n"
                     "    <level>\n"
                     "        crit, err, init, info or dbg\n",
                     &log,
                     2);

SHELL_COMMAND_DEFINE(port_stats,
                     "\nport_stats <port_id>\n"
                     "    parameters:\n"
                     "        port_id: logical port index\n",
                     &port_stats,
                     1);

static shell_status_t cd(shell_handle_t shell, int32_t argc, char **argv)
{
    storage_cd(argv[1], false);

    return kStatus_SHELL_Success;
}


static shell_status_t pwd(shell_handle_t shell, int32_t argc, char **argv)
{
    storage_pwd();

    return kStatus_SHELL_Success;
}


static shell_status_t mkdir(shell_handle_t shell, int32_t argc, char **argv)
{
    bool parent = false;
    int opt;

    optind = 1;
    while ((opt = getopt(argc, argv, "p")) != -1) {
        switch (opt) {
        case 'p':
            parent = true;
            break;
        default:
            break;
        }
    }

    if (argc > optind) {
        storage_mkdir(argv[optind], parent);
    } else {
        shell_printf(shell, "Usage:");
        shell_printf(shell, (SHELL_COMMAND(mkdir))->pcHelpString);
    }

    return kStatus_SHELL_Success;
}


static shell_status_t rm(shell_handle_t shell, int32_t argc, char **argv)
{
    bool recursive = false, force = false;
    int opt;

    optind = 1;
    while ((opt = getopt(argc, argv, "rf")) != -1) {
        switch (opt) {
        case 'r':
            recursive = true;
            break;
        case 'f':
            force = true;
            break;
        default:
            break;
        }
    }

    if (argc > optind) {
        storage_rm(argv[optind], recursive, force);
    } else {
        shell_printf(shell, "Usage:");
        shell_printf(shell, (SHELL_COMMAND(rm))->pcHelpString);
    }

    return kStatus_SHELL_Success;
}

static shell_status_t ls(shell_handle_t shell, int32_t argc, char **argv)
{
    if (argc > 1)
        storage_ls(argv[1]);
    else
        storage_ls(".");

    return kStatus_SHELL_Success;
}


static shell_status_t write(shell_handle_t shell, int32_t argc, char **argv)
{
    storage_write(argv[1], argv[2], strlen(argv[2]) + 1);

    return kStatus_SHELL_Success;
}

static shell_status_t cat(shell_handle_t shell, int32_t argc, char **argv)
{
    storage_cat(argv[1]);

    return kStatus_SHELL_Success;
}

static void help_config_management(shell_handle_t shell)
{
    shell_printf(shell, "management config\n\n");
    shell_printf(shell, "path: /management\n");
    shell_printf(shell, "parameters:\n");
    shell_printf(shell, "    is_bridge:\n");
    shell_printf(shell, "        value: 0 or 1\n");
    shell_printf(shell, "        description: Set to 1 if the stack operates in bridge mode\n");
}

static void help_config_fgptp(shell_handle_t shell)
{
    shell_printf(shell, "fgptp config\n\n");
    shell_printf(shell, "path: /fgptp\n");
    shell_printf(shell, "parameters:\n");
    shell_printf(shell, "    is_bridge:\n");
    shell_printf(shell, "        value: 0 or 1\n");
    shell_printf(shell, "        description: Set to 1 if the stack operates in bridge mode\n");
    shell_printf(shell, "    force_2011:\n");
    shell_printf(shell, "        value: 0 or 1\n");
    shell_printf(shell, "        description: Set to 1 if the stack operates per IEEE 802.1AS-2011 standard (no multi domains support)\n");
    shell_printf(shell, "path: /fgptp\n");
    shell_printf(shell, "parameters (domain 0):\n");
    shell_printf(shell, "    gmCapable: \n");
    shell_printf(shell, "        value: 0 or 1\n");
    shell_printf(shell, "        description: Set to 1 if the device has grandmaster\n");
    shell_printf(shell, "                     capability\n");
    shell_printf(shell, "    priority1:\n");
    shell_printf(shell, "        value: 0 to 255\n");
    shell_printf(shell, "        description: Set the priority1 of this clock\n");
    shell_printf(shell, "    priority2:\n");
    shell_printf(shell, "        value: 0 to 255\n");
    shell_printf(shell, "        description: Set the priority2 of this clock\n");
    shell_printf(shell, "    clockClass:\n");
    shell_printf(shell, "        value: 0 to 255\n");
    shell_printf(shell, "        description: Set the class value of this clock\n");
    shell_printf(shell, "    clockAccuracy:\n");
    shell_printf(shell, "        value: 0x0 to 0xff\n");
    shell_printf(shell, "        description: Set the accuracy value of this clock\n");
    shell_printf(shell, "    offsetScaledLogVariance:\n");
    shell_printf(shell, "        value: 0x0 to 0xffff\n");
    shell_printf(shell, "        description: Set the offset scaled log variance value of this clock\n");
    shell_printf(shell, "    neighborPropDelayThreshold:\n");
    shell_printf(shell, "        value: 0 to 10000000\n");
    shell_printf(shell, "        description: Set the neighbor propagation delay threshold expressed in ns\n");
    shell_printf(shell, "\n");
    shell_printf(shell, "path: /fgptp/domainN (N: instance index, 1 to %u)\n", CFG_MAX_GPTP_DOMAINS - 1);
    shell_printf(shell, "parameters (domain N):\n");
    shell_printf(shell, "    domain_number:\n");
    shell_printf(shell, "        value: gPTP domain number (1 to 127, or -1 to disable)\n");
    shell_printf(shell, "        description: Maps the domain instance <N> to the gPTP\n");
    shell_printf(shell, "                     domain number <value>\n");
    shell_printf(shell, "    gmCapable, priority1, etc...(same as above):\n");
    shell_printf(shell, "        See domain 0 parameters section above for details.\n");
    shell_printf(shell, "\n");
    shell_printf(shell, "path: /fgptp/portN (N: port index, 0 to %u)\n", CFG_MAX_NUM_PORT - 1);
    shell_printf(shell, "parameters (port N):\n");
    shell_printf(shell, "    rxDelayCompensation:\n");
    shell_printf(shell, "        value: -1000000 to 1000000\n");
    shell_printf(shell, "        description: Set Rx timestamp compensation, substracted from receive timestamp, in nanoseconds\n");
    shell_printf(shell, "    txDelayCompensation:\n");
    shell_printf(shell, "        value: -1000000 to 1000000\n");
    shell_printf(shell, "        description: Set Tx timestamp compensation, added to transmit timestamp, in nanoseconds\n");
    shell_printf(shell, "    initialLogPdelayReqInterval:\n");
    shell_printf(shell, "        value: 0 to 3\n");
    shell_printf(shell, "        description: Set pdelay request initial interval in log2 unit\n");
    shell_printf(shell, "    initialLogSyncInterval:\n");
    shell_printf(shell, "        value: -5 to 0\n");
    shell_printf(shell, "        description: Set sync transmit initial interval in log2 unit\n");
    shell_printf(shell, "    initialLogAnnounceInterval:\n");
    shell_printf(shell, "        value: 0 to 3\n");
    shell_printf(shell, "        description: Set initial announce transmit interval in log2 unit\n");
    shell_printf(shell, "    operLogPdelayReqInterval:\n");
    shell_printf(shell, "        value: 0 to 3\n");
    shell_printf(shell, "        description: Set operational pdelay request interval in log2 unit\n");
    shell_printf(shell, "    operLogSyncInterval:\n");
    shell_printf(shell, "        value: -5 to 0\n");
    shell_printf(shell, "        description: Set operational sync transmit interval in log2 unit\n");
    shell_printf(shell, "    allowedLostResponses:\n");
    shell_printf(shell, "        value: 1 to 255\n");
    shell_printf(shell, "        description: Set the number of Pdelay_Req messages without valid responses allowed\n");
    shell_printf(shell, "    delayMechanism:\n");
    shell_printf(shell, "        values for allowed mechanisms for domain number 0 are: 2 (\"P2P\"), 3 (\"COMMON_P2P\") \n");
    shell_printf(shell, "        description: Set peer delay mechanism associated to this port\n");
    shell_printf(shell, "\n");
    shell_printf(shell, "path: /fgptp/domainX/portN (X: instance index, 1 to %u),(N: port index, 0 to %u)\n", CFG_MAX_GPTP_DOMAINS - 1, CFG_MAX_NUM_PORT - 1);
    shell_printf(shell, "    delayMechanism:\n");
    shell_printf(shell, "        value for allowed mechanism for domain number greater than 0 is: 3 (\"COMMON_P2P\") \n");
    shell_printf(shell, "        description: Set peer delay mechanism associated to this port\n");
}

static void help_config_srp(shell_handle_t shell)
{
    shell_printf(shell, "srp config\n\n");
    shell_printf(shell, "path: /srp\n");
    shell_printf(shell, "parameters:\n");
    shell_printf(shell, "    is_bridge:\n");
    shell_printf(shell, "        value: 0 or 1\n");
    shell_printf(shell, "        description: Set to 1 if the stack operates in bridge mode\n");
}

static void help_config_hsr(shell_handle_t shell)
{
    shell_printf(shell, "hsr config\n\n");
    shell_printf(shell, "path: /hsr\n");
    shell_printf(shell, "parameters:\n");
    shell_printf(shell, "    hsr_enabled:\n");
    shell_printf(shell, "        value: 0 or 1\n");
    shell_printf(shell, "        description: Set to 1 if want to enable running the hsr stack\n");
    shell_printf(shell, "path: /hsr/portN (N: logical port index, 0 to %u)\n", CONFIG_APP_LOGICAL_PORTS - 1);
    shell_printf(shell, "parameters (port N):\n");
    shell_printf(shell, "    type:\n");
    shell_printf(shell, "        value: 0 to 5\n");
    shell_printf(shell, "        description: 0: External endpoint port\n");
    shell_printf(shell, "                     1: Internal endpoint port\n");
    shell_printf(shell, "                     2: HSR ring port\n");
    shell_printf(shell, "                     3: HSR ring 2 port of quardbox\n");
    shell_printf(shell, "                     4: External switch port\n");
    shell_printf(shell, "                     5: Internal switch port\n");
}

static void help_config_port(shell_handle_t shell)
{
    shell_printf(shell, "port config\n");
    shell_printf(shell, "path: /portN (N: logical port index, 0 to %u)\n", CONFIG_APP_LOGICAL_PORTS - 1);
    shell_printf(shell, "parameters (logical port N):\n");
    shell_printf(shell, "    hw_addr: \n");
    shell_printf(shell, "        format: XX:XX:XX:XX:XX:XX (X is an hexadecimal digit)\n");
    shell_printf(shell, "        description: MAC address\n");
    shell_printf(shell, "    ip_addr: \n");
    shell_printf(shell, "        format: a.b.c.d (dotted decimal format)\n");
    shell_printf(shell, "        description: IP address\n");
    shell_printf(shell, "    gw_addr: \n");
    shell_printf(shell, "        format: a.b.c.d (dotted decimal format)\n");
    shell_printf(shell, "        description: Gateway IP address\n");
    shell_printf(shell, "    net_mask: \n");
    shell_printf(shell, "        format: a.b.c.d (dotted decimal format)\n");
    shell_printf(shell, "        description: Network mask\n");
}

__WEAK void help_config_avdecc(shell_handle_t shell)
{
    shell_printf(shell, "not supported\n");
}

__WEAK void help_config_qbv(shell_handle_t shell)
{
    shell_printf(shell, "not supported\n");
}

__WEAK void help_config_fdb(shell_handle_t shell)
{
    shell_printf(shell, "not supported\n");
}

__WEAK void help_config_vlan(shell_handle_t shell)
{
    shell_printf(shell, "not supported\n");
}

__WEAK void help_config_si(shell_handle_t shell)
{
    shell_printf(shell, "not supported\n");
}

__WEAK void help_config_psfp(shell_handle_t shell)
{
    shell_printf(shell, "not supported\n");
}

__WEAK void help_config_frer(shell_handle_t shell)
{
    shell_printf(shell, "not supported\n");
}

__WEAK void help_config_multicore(shell_handle_t shell)
{
    shell_printf(shell, "not supported\n");
}

__WEAK void help_config_params_multicore(shell_handle_t shell)
{
    shell_printf(shell, "not supported\n");
}

__WEAK void help_config_app(shell_handle_t shell)
{
    shell_printf(shell, "not supported\n");
}

__WEAK void help_config_fp(shell_handle_t shell)
{
    shell_printf(shell, "not supported\n");
}

static shell_status_t help_config(shell_handle_t shell, int32_t argc, char **argv)
{
    if (!strcmp(argv[1], "params_multicore"))
        help_config_params_multicore(shell);
    else if (!strcmp(argv[1], "multicore"))
        help_config_multicore(shell);
    else if (!strcmp(argv[1], "app"))
        help_config_app(shell);
    else if (!strcmp(argv[1], "port"))
        help_config_port(shell);
    else if (!strcmp(argv[1], "management"))
        help_config_management(shell);
    else if (!strcmp(argv[1], "fgptp"))
        help_config_fgptp(shell);
    else if (!strcmp(argv[1], "srp"))
        help_config_srp(shell);
    else if (!strcmp(argv[1], "hsr"))
        help_config_hsr(shell);
    else if (!strcmp(argv[1], "avdecc"))
        help_config_avdecc(shell);
    else if (!strcmp(argv[1], "qbv"))
        help_config_qbv(shell);
    else if (!strcmp(argv[1], "fdb"))
        help_config_fdb(shell);
    else if (!strcmp(argv[1], "vlan"))
        help_config_vlan(shell);
    else if (!strcmp(argv[1], "si"))
        help_config_si(shell);
    else if (!strcmp(argv[1], "psfp"))
        help_config_psfp(shell);
    else if (!strcmp(argv[1], "frer"))
        help_config_frer(shell);
    else if (!strcmp(argv[1], "fp"))
        help_config_fp(shell);
    else
        shell_printf(shell, (SHELL_COMMAND(help_config))->pcHelpString);

    return kStatus_SHELL_Success;
}

static shell_status_t log(shell_handle_t shell, int32_t argc, char **argv)
{
    if (!strcmp(argv[1], "app") || !strcmp(argv[1], "all")) {
        if (app_log_level_set(argv[2]) < 0)
            goto usage;
    }

    if (strcmp(argv[1], "app")) {
        if (!get_genavb_handle())
            shell_printf(shell, "genAVB stack is not ready\n");
        else if (gavb_log_level(argv[1], argv[2]) < 0)
            goto usage;
    }

    return kStatus_SHELL_Success;

usage:
    shell_printf(shell, (SHELL_COMMAND(log))->pcHelpString);

    return kStatus_SHELL_Success;
}

static int __port_stats(shell_handle_t shell, unsigned int port_id, int n)
{
    const char *names[n];
    uint64_t values[n];
    int i;

    if (genavb_port_stats_get_strings(port_id, names, n * sizeof(char *)) < 0) {
        log_err("genavb_port_stats_get_strings() failed\n");
        goto err;
    }

    if (genavb_port_stats_get(port_id, values, n * sizeof(uint64_t)) < 0) {
        log_err("genavb_port_stats_get() failed\n");
        goto err;
    }

    for (i = 0; i < n; i++) {
        shell_printf(shell, "%-32s %10llu\n", names[i], values[i]);
    }

    return 0;

err:
   return -1;
}

static shell_status_t port_stats(shell_handle_t shell, int32_t argc, char **argv)
{
    unsigned int port_id;
    unsigned long tmp;
    int n;

    h_strtoul(&tmp, argv[1], NULL, 0);
    port_id = tmp;

    n = genavb_port_stats_get_number(port_id);
    if (n < 0) {
        log_err("genavb_port_stats_get_number() error %d\n", n);
        goto exit;
    }

    __port_stats(shell, port_id, n);

exit:
    return kStatus_SHELL_Success;
}

#if defined(SHELL_INSERT_KEY_ENABLE) && defined(DEBUG_CONSOLE_RUNTIME_CONTROL)
static void shell_insert_key_handler(shell_handle_t shell, bool enabled)
{
    if (shell) {
        if (enabled)
            DbgConsole_SetState(false);
        else
            DbgConsole_SetState(true);
    }
}
#endif

static void shell_task(void *pvParameters)
{
    struct shell_ctx *ctx = pvParameters;
    shell_handle_t shell = &ctx->shell_handle[0];

    if (ctx->init)
        ctx->init(shell);

    SHELL_Task(shell);

    /* Should never exit */

    vTaskDelete(NULL);
}

int shell_start(void (*init)(shell_handle_t shell))
{
    shell_ctx.init = init;

    if (xTaskCreate(&shell_task, SHELL_TASK_NAME,
        __SHELL_TASK_STACK_SIZE, &shell_ctx,
        __SHELL_TASK_PRIORITY, &shell_ctx.task_handle) != pdPASS) {
        log_err("xTaskCreate(%s) failed\n", SHELL_TASK_NAME);
        goto err;
    }

    return 0;

err:
    return -1;
}

shell_handle_t shell_init(const char *prompt_name)
{
    shell_handle_t shell = &shell_ctx.shell_handle[0];
    shell_status_t status;
    char *prompt_end = ">>";

    if (strlen(prompt_name) >= (SHELL_PROMPT_LEN - strlen(prompt_end)))
        goto err;

    strcpy(prompt, prompt_name);
    strcat(prompt, prompt_end);

    status = SHELL_Init(shell, g_serialHandle, prompt);
    if (status != kStatus_SHELL_Success)
        goto err;

#if defined(SHELL_INSERT_KEY_ENABLE) && defined(DEBUG_CONSOLE_RUNTIME_CONTROL)
    SHELL_RegisterStateHandler(shell, shell_insert_key_handler);
#endif

    SHELL_RegisterCommand(shell, SHELL_COMMAND(write));
    SHELL_RegisterCommand(shell, SHELL_COMMAND(cat));
    SHELL_RegisterCommand(shell, SHELL_COMMAND(ls));
    SHELL_RegisterCommand(shell, SHELL_COMMAND(rm));
    SHELL_RegisterCommand(shell, SHELL_COMMAND(cd));
    SHELL_RegisterCommand(shell, SHELL_COMMAND(pwd));
    SHELL_RegisterCommand(shell, SHELL_COMMAND(mkdir));
    SHELL_RegisterCommand(shell, SHELL_COMMAND(help_config));
    SHELL_RegisterCommand(shell, SHELL_COMMAND(log));
    SHELL_RegisterCommand(shell, SHELL_COMMAND(port_stats));

    return shell;

err:
    return NULL;
}
