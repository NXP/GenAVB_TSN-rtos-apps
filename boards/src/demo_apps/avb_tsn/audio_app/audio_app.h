/*
 * Copyright 2024-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _AUDIO_APP_H_
#define _AUDIO_APP_H_

#include "rtos_abstraction_layer.h"
#include "rtos_mqueue.h"
#include "rtos_apps/audio/audio_ctrl.h"

#define AUDIO_APP_MAX_RUN_MODES	2

#define AUDIO_EVENT_QUEUE_LENGTH 1

struct audio_app_cmd {
    struct audio_command cmd;
    uint32_t len;
};

struct audio_app_resp {
    struct audio_response resp;
    uint32_t len;
};

struct audio_ctx {
    rtos_mqueue_t queue_cmd;
    rtos_mqueue_t queue_resp;
    uint8_t queue_buffer_cmd[AUDIO_EVENT_QUEUE_LENGTH * sizeof(struct audio_app_cmd)];
    uint8_t queue_buffer_resp[AUDIO_EVENT_QUEUE_LENGTH * sizeof(struct audio_app_resp)];
};

extern struct audio_ctx audio_ctx;

void audio_app_main(void);
int audio_app_init(void);

#endif /* _AUDIO_APP_H_ */
