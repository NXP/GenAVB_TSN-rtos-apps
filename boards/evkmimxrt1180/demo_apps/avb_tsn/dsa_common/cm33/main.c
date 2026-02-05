/*
 * Copyright 2021-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FreeRTOS.h"
#include "task.h"

#include "board.h"
#include "clock_config.h"
#include "fsl_ele_base_api.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "pin_mux_lpspi.h"

#include "avb_tsn/common/fdb.h"
#include "avb_tsn/common/fp.h"
#include "avb_tsn/common/frer.h"
#include "avb_tsn/common/genavb.h"
#include "avb_tsn/common/log.h"
#include "lwip.h"
#include "lwip_iperf.h"
#include "avb_tsn/common/qbv.h"
#include "avb_tsn/common/shell.h"
#include "avb_tsn/common/stats_task.h"
#include "avb_tsn/common/storage.h"
#include "avb_tsn/common/vlan.h"
#include "avb_tsn/common/stream_identification.h"
#include "avb_tsn/common/psfp.h"
#include "multicore.h"
#include "avb_tsn/common/hsr.h"
#include "dsa_control.h"
#include "dsa_virtual_switch.h"

#define MAIN_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE + 640)
#define MAIN_TASK_PRIORITY   3
#define MAIN_TASK_DELAY_MS   1000

#define STATS_PERIOD_MS 2000
#define ELE_PING_TIMEOUT_COUNT ((12 * 60 * 60 * 1000) / MAIN_TASK_DELAY_MS) /* 12 hours */

static void init(shell_handle_t shell)
{
    fdb_init_shell(shell);
    vlan_init_shell(shell);
    fp_init(shell);
    stream_identification_init_shell(shell);
    psfp_init_shell(shell);
    frer_init_shell(shell);
    qbv_init(shell);
    hsr_init_shell(shell);

    lwip_stack_init();
    lwip_iperf_start(shell);
}

static void main_task(void *data)
{
    shell_handle_t shell;
    struct gavb_pps pps_timer;
    unsigned int ele_ping_timer_count = 0;

#if defined(DSA_CPU_PORT_ENETC)
    log_info("DSA ENETC main_task started\n");
#else
    log_info("DSA SWITCH main_task started\n");
#endif

    if (STATS_TaskInit(NULL, NULL, STATS_PERIOD_MS, NULL) < 0)
        log_err("STATS_TaskInit() failed\n");

    if (dsa_ctrl_task_init() < 0)
        log_err("dsa_ctrl_task_init() failed\n");

    storage_init();
    shell = shell_init("BRIDGE");
    if (shell == NULL) {
        log_err("shell_init() failed\n");
        goto err_shell_init;
    }

    storage_set_shell(shell);

    if (gavb_stack_init()) {
        log_err("gavb_stack_init() failed\n");
        goto err_stack_init;
    }

    if (dsa_virtual_switch_init()) {
        log_err("dsa_virtual_switch_init() failed\n");
        goto err_virtual_switch_init;
    }

    shell_start(&init);

    multicore_init();

    gavb_pps_init(&pps_timer, GENAVB_CLOCK_BR_0_0);

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(MAIN_TASK_DELAY_MS));

        /* Every day (24 hours), the SoC application has to ping the ELE Active Timer
            to prevent Reset of chip */

        ele_ping_timer_count += 1;

        /* Ping ELE every 12 hours */
        if (ele_ping_timer_count == ELE_PING_TIMEOUT_COUNT) {
            if (ELE_BaseAPI_Ping(MU_RT_S3MUA) != kStatus_Success) {
                log_err("Ping ELE failed\n");
            } else {
                log_info("Ping ELE successfull\n");
            }
            ele_ping_timer_count = 0;
        }
    }

err_virtual_switch_init:
    gavb_stack_exit();

err_stack_init:
err_shell_init:
    vTaskDelete(NULL);
}

int main(void)
{
    BOARD_ConfigMPU();
    BOARD_InitBootPins();
    BOARD_InitLpspiPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();
    BOARD_InitDebug();
    BOARD_NETC_Init();
    BOARD_InitNVIC();

    (void)xTaskCreate(&main_task, "main task", MAIN_TASK_STACK_SIZE,
                    "main1", MAIN_TASK_PRIORITY, NULL);

    vTaskStartScheduler();

    while (1) {
    }
}
