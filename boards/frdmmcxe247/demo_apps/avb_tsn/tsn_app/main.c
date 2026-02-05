/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017, 2024-2025 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

#include "rtos_apps/log.h"

#include "board.h"

#include "avb_tsn/common/genavb.h"
#include "avb_tsn/common/storage.h"
#include "avb_tsn/common/stats_task.h"
#include "avb_tsn/common/shell.h"

#include "lwip.h"
#include "lwip_iperf.h"

static const char *prompt = "ENDPOINT";

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define MAIN_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE + 512)
#define MAIN_TASK_PRIORITY   1

#define STATS_PERIOD_MS 2000

extern bool idle_hook_enabled;

static void init(shell_handle_t shell)
{
    lwip_stack_init();
    lwip_iperf_start(shell);
}

static void stack_init_task(void *pvParameters)
{
    struct rtos_apps_async *async;
    shell_handle_t shell;

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

    idle_hook_enabled = true;

err_stack_init:
err_shell_init:
err_stats_task_init:
    vTaskDelete(NULL);
}

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Main function
 */

int main(void)
{
    BOARD_InitHardware();

    (void)xTaskCreate(stack_init_task, "Stack_Init", MAIN_TASK_STACK_SIZE, NULL, MAIN_TASK_PRIORITY, NULL);

    vTaskStartScheduler();
    for (;;)
        ;
}
