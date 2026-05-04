/*
 * Copyright 2018-2020, 2023, 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifdef CONFIG_APP_LWIP
#include "FreeRTOS.h"
#include "lwip/opt.h"
#include "lwip/tcpip.h"
#include "lwip/apps/lwiperf.h"
#include "rtos_apps/log.h"
#include "avb_tsn/common/shell.h"
#include <stdio.h>

/* Report state => string */
static const char *report_type_str[] = {
    "TCP_DONE_SERVER",             /* LWIPERF_TCP_DONE_SERVER,*/
    "TCP_DONE_CLIENT",             /* LWIPERF_TCP_DONE_CLIENT,*/
    "TCP_ABORTED_LOCAL",           /* LWIPERF_TCP_ABORTED_LOCAL, */
    "TCP_ABORTED_LOCAL_DATAERROR", /* LWIPERF_TCP_ABORTED_LOCAL_DATAERROR, */
    "TCP_ABORTED_LOCAL_TXERROR",   /* LWIPERF_TCP_ABORTED_LOCAL_TXERROR, */
    "TCP_ABORTED_REMOTE",          /* LWIPERF_TCP_ABORTED_REMOTE, */
    "UDP_STARTED",                 /* LWIPERF_UDP_STARTED, */
    "UDP_DONE",                    /* LWIPERF_UDP_DONE, */
    "UDP_ABORTED_LOCAL",           /* LWIPERF_UDP_ABORTED_LOCAL, */
    "UDP_ABORTED_REMOTE"           /* LWIPERF_UDP_ABORTED_REMOTE */
};

static void *tcp_client_session = NULL;

#define print(shell, args...) do { if (shell) SHELL_Printf(shell, args); else log_raw_info(args); } while(0)

/** Prototype of a report function that is called when a session is finished.
    This report function shows the test results. */
static void lwiperf_report(void *arg,
                           enum lwiperf_report_type report_type,
                           const ip_addr_t *local_addr,
                           u16_t local_port,
                           const ip_addr_t *remote_addr,
                           u16_t remote_port,
                           u64_t bytes_transferred,
                           u32_t ms_duration,
                           u32_t bandwidth_kbitpsec)
{
    shell_handle_t shell = arg;

    print(shell, "-------------------------------------------------\r\n");
    if ((report_type < (sizeof(report_type_str) / sizeof(report_type_str[0]))) && local_addr && remote_addr) {
        print(shell, " %s \r\n", report_type_str[report_type]);
        print(shell, " Local address : %u.%u.%u.%u ", ((u8_t *)local_addr)[0], ((u8_t *)local_addr)[1],
               ((u8_t *)local_addr)[2], ((u8_t *)local_addr)[3]);
        print(shell, " Port %d \r\n", local_port);
        print(shell, " Remote address : %u.%u.%u.%u ", ((u8_t *)remote_addr)[0], ((u8_t *)remote_addr)[1],
               ((u8_t *)remote_addr)[2], ((u8_t *)remote_addr)[3]);
        print(shell, " Port %d \r\n", remote_port);
        print(shell, " Bytes Transferred %llu \r\n", bytes_transferred);
        print(shell, " Duration (ms) %d \r\n", ms_duration);
        print(shell, " Bandwidth (kbitpsec) %d \r\n", bandwidth_kbitpsec);
    } else {
        print(shell, " IPERF Report error\r\n");
    }
}


static shell_status_t tcp_client(shell_handle_t shell, int32_t argc, char **argv);
static shell_status_t tcp_client_stop(shell_handle_t shell, int32_t argc, char **argv);

SHELL_COMMAND_DEFINE(tcp_client,
                     "\ntcp_client <remote_ip_addr>\n",
                     &tcp_client,
                     1);

SHELL_COMMAND_DEFINE(tcp_client_stop,
                     "\ntcp_client_stop\n",
                     &tcp_client_stop,
                     0);

static shell_status_t tcp_client(shell_handle_t shell, int32_t argc, char **argv)
{
    unsigned int tmp[4];
    ip_addr_t addr;

    if (sscanf(argv[1], "%u.%u.%u.%u", &tmp[0], &tmp[1], &tmp[2], &tmp[3]) != 4)
        return kStatus_SHELL_Error;

    if (tcp_client_session) {
        lwiperf_abort(tcp_client_session);
        tcp_client_session = NULL;
    }

    IP_ADDR4(&addr, tmp[0], tmp[1], tmp[2], tmp[3]);

#ifndef LWIPERF_TOS_DEFAULT
    tcp_client_session = lwiperf_start_tcp_client(&addr, LWIPERF_TCP_PORT_DEFAULT, LWIPERF_CLIENT, -100000000, &lwiperf_report, shell);
#else
    tcp_client_session = lwiperf_start_tcp_client(&addr, LWIPERF_TCP_PORT_DEFAULT, LWIPERF_CLIENT, -100000000, 0, LWIPERF_TOS_DEFAULT, &lwiperf_report, shell);
#endif

    SHELL_Printf(shell, "IPERF TCP client %s: server %u.%u.%u.%u\n", tcp_client_session ? "started" : "failed", tmp[0], tmp[1], tmp[2], tmp[3]);

    return kStatus_SHELL_Success;
}

static shell_status_t tcp_client_stop(shell_handle_t shell, int32_t argc, char **argv)
{
    if (tcp_client_session) {
        lwiperf_abort(tcp_client_session);
        tcp_client_session = NULL;
    }

    return kStatus_SHELL_Success;
}

/*!
 * @brief starts iperf server
 */
void lwip_iperf_start(void *shell)
{

    if (lwiperf_start_tcp_server_default(&lwiperf_report, NULL)) {
        log_raw_info("IPERF TCP server started\r\n");
    } else
        log_raw_info("IPERF TCP server failed\r\n");

    if (shell) {
        SHELL_RegisterCommand(shell, SHELL_COMMAND(tcp_client));
        SHELL_RegisterCommand(shell, SHELL_COMMAND(tcp_client_stop));
    }
}
#else
void lwip_iperf_start(void *shell) {return;}
#endif
