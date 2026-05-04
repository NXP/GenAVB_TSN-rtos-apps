/*
 * Copyright 2018-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

#include "rtos_apps/tsn/tsn_tasks_config.h"
#include "rtos_apps/tsn/tsn_entry.h"
#include "rtos_apps/tsn/user_button.h"

#ifdef CONFIG_APP_SERIAL
#include "fsl_lpuart.h"
#endif

#include "board.h"
#include "avb_tsn/common/fp.h"
#include "avb_tsn/common/genavb.h"
#include "avb_tsn/common/log.h"
#include "lwip.h"
#include "lwip_iperf.h"
#include "avb_tsn/common/qbv.h"
#include "shell.h"
#include "avb_tsn/common/storage.h"
#include "avb_tsn/common/stats_task.h"
#include "avb_tsn/common/system_config.h"
#include "tsn_app.h"

#include "genavb/helpers.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define MAIN_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE + 512)
#define MAIN_TASK_PRIORITY   1

#define STATS_PERIOD_MS 2000

static const char *task_id_names[] = {"CONTROLLER_0", "IO_DEVICE_0", "IO_DEVICE_1", "MAX_TASK_ID"};
static struct gavb_pps pps;

struct rtos_apps_user_button *user_button;

/*******************************************************************************
 * Code
 ******************************************************************************/
void BOARD_USER_BUTTON_IRQ_HANDLER(void)
{
    if (user_button)
        rtos_apps_user_button_irq(user_button);
}

static void init(shell_handle_t shell)
{
    default_qos_init(shell);
    fp_init(shell);
    qbv_init(shell);
    lwip_stack_init();
    lwip_iperf_start(shell);
}

#ifdef CONFIG_APP_MOTOR_CONTROLLER
static void tsn_app_motor_params_init(struct rtos_apps_tsn_motor_params *params, unsigned int id)
{
    char buf[20];

    h_snprintf(buf, 20, "/tsn_app/motor%u:%u", (id >> 8) & 0xff, id & 0xff);

    if (storage_cd(buf, true))
        goto out;

    storage_read_float("max_vel", &params->max_vel_rpm);
    storage_read_float("max_accel", &params->max_accel_rpm_p_s);

    storage_read_float("J", &params->J);
    storage_read_float("b", &params->b);
    storage_read_float("Tm", &params->Tm);

    storage_cd("-", true);

out:
    return;
}
#endif

static void main_task(void *data)
{
    struct cyclic_task_config *c_cfg;
    struct rtos_apps_tsn_config *config;
    struct rtos_apps_async *async;
    struct tsn_app_ctx *ctx;
    int conf_task_id;
    shell_handle_t shell;
#ifdef CONFIG_APP_SERIAL
    struct rtos_apps_tsn_serial_iodevice_config serial_cfg = {
        .baseaddr = BOARD_IODEV_UART_BASEADDR,
        .baudrate = BOARD_IODEV_UART_BAUDRATE,
        .clk_freq = BOARD_IODEV_UART_CLK_FREQ,
        .irq_mask = BOARD_IODEV_UART_INTERRUPT_MASK,
    };
#endif
    struct rtos_apps_user_button_config user_button_cfg = {
#ifdef CONFIG_APP_USER_BUTTON
        .irq_n = BOARD_USER_BUTTON_IRQ,
        .init = &BOARD_InitGPIO,
        .exit = NULL,
        .active = &BOARD_GPIO_Active,
        .clear = &BOARD_GPIO_Clear,
#endif
    };

    if (STATS_TaskInit(NULL, NULL, STATS_PERIOD_MS, &async) < 0) {
        log_err("STATS_TaskInit() failed\n");
        goto exit;
    }

    user_button = rtos_apps_user_button_init(&user_button_cfg);
    if (!user_button) {
        log_err("rtos_apps_user_button_init() failed\n");
        goto exit;
    }

    storage_init();

    config = system_config_get_tsn_app();
    if (!config) {
        log_err("system_config_get_tsn_app() failed\n");
        goto exit;
    }

#ifdef CONFIG_APP_SERIAL
    config->serial_cfg = &serial_cfg;
#endif

    config->log_update_time = &app_log_update_time;
    config->async = async;
    config->user_button = user_button;
#ifdef CONFIG_APP_MOTOR_CONTROLLER
    config->app_motor_params_init = tsn_app_motor_params_init;
#endif

    conf_task_id = config->role;

    shell = tsn_init_shell(task_id_names[conf_task_id]);
    if (shell == NULL) {
        log_err("tsn_init_shell() failed\n");
        goto exit;
    }

    if (gavb_stack_init()) {
        log_err("gavb_stack_init() failed\n");
        goto exit;
    }

    shell_start(&init);

    c_cfg = tsn_conf_get_cyclic_task(config->role);
    if (!c_cfg) {
        log_err("tsn_conf_get_cyclic_task() failed\n");
        goto exit;
    }

    if (gavb_pps_init(&pps, c_cfg->params.clk_id) < 0)
        log_err("gavb_pps_init() failed: pps timer could not be started\n");

    rtos_apps_tsn_init(config, &ctx);

exit:
    /*
     * For now nothing more to do, delete task.
     */
    vTaskDelete(NULL);
}

/*!
 * @brief TSN Main function
 */
int tsn_app_main(void)
{
    (void)xTaskCreate(&main_task, "main task", MAIN_TASK_STACK_SIZE,
                    NULL, MAIN_TASK_PRIORITY, NULL);

    vTaskStartScheduler();

    return 0;
}
