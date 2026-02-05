/*
 * Copyright 2021-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FreeRTOS.h"
#include "task.h"

#include "rtos_apps/tsn/tsn_tasks_config.h"
#include "rtos_apps/tsn/tsn_entry.h"
#include "rtos_apps/tsn/user_button.h"

#include "board.h"
#include "clock_config.h"
#include "fsl_ele_base_api.h"
#include "fsl_debug_console.h"

#include "avb_tsn/common/fdb.h"
#include "avb_tsn/common/fp.h"
#include "avb_tsn/common/frer.h"
#include "avb_tsn/common/genavb.h"
#include "avb_tsn/common/log.h"
#include "lwip.h"
#include "lwip_iperf.h"
#include "avb_tsn/common/qbv.h"
#include "avb_tsn/common/stats_task.h"
#include "avb_tsn/common/storage.h"
#include "avb_tsn/common/system_config.h"
#include "avb_tsn/common/vlan.h"
#include "avb_tsn/common/stream_identification.h"
#include "avb_tsn/common/psfp.h"
#include "multicore.h"
#include "avb_tsn/common/hsr.h"

#define MAIN_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE + 640)
#define MAIN_TASK_PRIORITY   3
#define MAIN_TASK_DELAY_MS   1000

#define STATS_PERIOD_MS 2000
#define ELE_PING_TIMEOUT_COUNT ((12 * 60 * 60 * 1000) / MAIN_TASK_DELAY_MS) /* 12 hours */

extern bool idle_hook_enabled;

#ifdef CONFIG_APP_TSN_ENDPOINT
static const char *prompt = "HYBRID";
extern struct rtos_apps_user_button *user_button;

void default_qos_init(shell_handle_t shell);
#else
static const char *prompt = "BRIDGE";
#endif

static void init(shell_handle_t shell)
{
#ifdef CONFIG_APP_TSN_ENDPOINT
    default_qos_init(shell);
#endif

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

#ifdef CONFIG_APP_TSN_ENDPOINT
static void tsn_app_init(struct rtos_apps_async *async)
{
    struct rtos_apps_user_button_config user_button_cfg = {
#ifdef CONFIG_APP_USER_BUTTON
        .irq_n = BOARD_USER_BUTTON_IRQ,
        .init = &BOARD_InitGPIO,
        .exit = NULL,
        .active = &BOARD_GPIO_Active,
        .clear = &BOARD_GPIO_Clear,
#endif
    };
    struct rtos_apps_tsn_config *config;
    struct tsn_app_ctx *ctx;

    user_button = rtos_apps_user_button_init(&user_button_cfg);
    if (!user_button) {
        log_err("rtos_apps_user_button_init() failed\n");
        goto exit;
    }

    config = system_config_get_tsn_app();
    if (!config) {
        log_err("system_config_get_tsn_app() failed\n");
        goto exit;
    }

    config->log_update_time = &app_log_update_time;
    config->async = async;
    config->user_button = user_button;

    rtos_apps_tsn_init(config, &ctx);

exit:
    return;
}
#else
static void tsn_app_init(struct rtos_apps_async *async) { return; }
#endif

static void main_task(void *data)
{
    shell_handle_t shell;
    struct gavb_pps pps_timer;
    unsigned int ele_ping_timer_count = 0;
    struct rtos_apps_async *async;

    log_info("main_task started\n");

    if (STATS_TaskInit(NULL, NULL, STATS_PERIOD_MS, &async) < 0) {
        log_err("STATS_TaskInit() failed\n");
        goto err_stats_task_init;
    }

    storage_init();
    shell = shell_init(prompt);
    if (shell == NULL) {
        log_err("shell_init() failed\n");
        goto err_shell_init;
    }

    storage_set_shell(shell);

    if (gavb_stack_init()) {
        log_err("gavb_stack_init() failed\n");
        goto err_stack_init;
    }

    shell_start(&init);

    multicore_init();

    gavb_pps_init(&pps_timer, GENAVB_CLOCK_BR_0_0);

    tsn_app_init(async);

    idle_hook_enabled = true;

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

err_stack_init:
err_shell_init:
err_stats_task_init:
    vTaskDelete(NULL);
}

int main(void)
{
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
