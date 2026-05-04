/*
 * Copyright 2024-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

#include "rtos_apps/audio/audio_app.h"
#include "rtos_apps/audio/audio_entry.h"
#include "rtos_apps/log.h"

#include "rtos_abstraction_layer.h"

#include "audio_app.h"
#include "audio_shell.h"
#include "rtos_mqueue.h"
#include "avb_tsn/common/genavb.h"
#include "avb_tsn/common/stats_task.h"
#include "avb_tsn/common/storage.h"
#include "avb_tsn/common/codec_config.h"

extern const struct play_pipeline_config *play_config[][AUDIO_APP_MAX_RUN_MODES];
extern uint32_t max_play_configs;

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DATA_TASK_PRIORITY   (configMAX_PRIORITIES - 2)
#define CTRL_TASK_PRIORITY   (configMAX_PRIORITIES - 8)
#define MAIN_TASK_PRIORITY   (configMAX_PRIORITIES - 10)

#define DATA_THREADS          1
#define STATS_PERIOD_MS       10000
#define DATA_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE + 200)
#define CTRL_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE + 800)
#define MAIN_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE + 800)

const int audio_app_supported_period[] = {2, 4};

struct audio_ctx audio_ctx;

/*******************************************************************************
 * Code
 ******************************************************************************/

int audio_app_ctrl_send(void *ctrl_handle, void *data, uint32_t len)
{
    struct audio_ctx *ctx = (struct audio_ctx *)ctrl_handle;
    struct audio_app_resp resp;
    int ret = 0;

    memcpy(&resp, data, len);
    resp.len = len;

    if (rtos_mqueue_send(&(ctx->queue_resp), &resp, RTOS_NO_WAIT) < 0) {
        ret = -1;
    }

    return ret;
}

int audio_app_ctrl_recv(void *ctrl_handle, void *data, uint32_t *len)
{
    struct audio_ctx *ctx = (struct audio_ctx *)ctrl_handle;
    struct audio_app_cmd cmd;
    int ret = 0;

    if (rtos_mqueue_receive(&(ctx->queue_cmd), &cmd, RTOS_NO_WAIT) < 0) {
        ret = -1;
    } else {
        if (*len < cmd.len) {
            ret = -1;
        } else {
            memcpy(data, &cmd, cmd.len);
            *len = cmd.len;
        }
    }

    return ret;
}

static bool audio_check_params(uint32_t period, uint32_t rate)
{
    return true;
}

int audio_app_apply_config(struct audio_app_run_config *run_config, const struct play_pipeline_config **play_cfg)
{

    if (run_config->index >= max_play_configs) {
        log_err("Unsupported configuration(%u)\n", run_config->index);
        goto err;
    }

    if (run_config->mode >= AUDIO_APP_MAX_RUN_MODES) {
        log_err("Unsupported run mode(%u)\n", run_config->mode);
        goto err;
    }

    *play_cfg = play_config[run_config->index][run_config->mode];

    if (!*play_cfg) {
        log_err("Configuration(%u): Unsupported run mode(%u)\n", run_config->index, run_config->mode);
        goto err;
    }

    if (!BOARD_codec_is_rate_supported(run_config->rate)) {
        log_err("Configuration(%u): Unsupported rate(%u Hz)\n", run_config->rate);
        goto err;
    }

    if (!audio_check_params(run_config->period, run_config->rate)) {
        log_warn("Configuration(%u): Unsupported combination rate(%u Hz)/period(%u)\n",
                  run_config->index, run_config->period, run_config->rate);
    }

    return 0;

err:
    return -1;
}

int audio_app_init(void)
{
    struct rtos_apps_audio_config config = {
        .thread_count = DATA_THREADS,
        .data_stack_size = DATA_TASK_STACK_SIZE,
        .data_priority = DATA_TASK_PRIORITY,
        .ctrl_stack_size = CTRL_TASK_STACK_SIZE,
        .ctrl_priority = CTRL_TASK_PRIORITY,
        .ctrl_handle = &audio_ctx,
    };

    if (rtos_mqueue_init(&audio_ctx.queue_cmd, AUDIO_EVENT_QUEUE_LENGTH, sizeof(struct audio_app_cmd), audio_ctx.queue_buffer_cmd) < 0) {
        log_err("rtos_mqueue_init(audio_ctx) failed\n");
        goto err_cmd;
    }

    if (rtos_mqueue_init(&audio_ctx.queue_resp, AUDIO_EVENT_QUEUE_LENGTH, sizeof(struct audio_app_resp), audio_ctx.queue_buffer_resp) < 0) {
        log_err("rtos_mqueue_init(audio_ctx) failed\n");
        goto err_resp;
    }

    if (rtos_apps_audio_init(&config) < 0)
        goto err_init;

    return 0;

err_init:
    rtos_mqueue_destroy(&audio_ctx.queue_resp);

err_resp:
    rtos_mqueue_destroy(&audio_ctx.queue_cmd);

err_cmd:
    return -1;
}

static void main_task(void *data)
{
    shell_handle_t shell;

    if (storage_init() < 0) {
        log_err("storage_init() failed\n");
        goto exit;
    }

    shell = shell_init("AVB");
    if (shell == NULL) {
        log_err("shell_init() failed\n");
        goto exit;
    }

    storage_set_shell(shell);

    if (STATS_TaskInit(NULL, NULL, STATS_PERIOD_MS, NULL) < 0) {
        log_err("STATS_TaskInit() failed\n");
        goto exit;
    }

    if (gavb_stack_init()) {
        log_err("gavb_stack_init() failed\n");
        goto exit;
    }

    if (audio_app_init() < 0) {
        log_err("audio_app_init() failed\n");
        goto exit;
    }

    if (shell_start(&audio_init_shell) < 0) {
        log_err("shell_start() failed\n");
        goto exit;
    }

    /* nothing else to do, exit */

exit:
    vTaskDelete(NULL);
}

void audio_app_main(void)
{
    (void)xTaskCreate(&main_task, "main task", MAIN_TASK_STACK_SIZE, NULL, MAIN_TASK_PRIORITY, NULL);

    vTaskStartScheduler();
}

struct genavb_handle *audio_app_avb_init(void)
{
    return get_genavb_handle();
}

void audio_app_avb_exit(void)
{
    return;
}
