/*
 * Copyright 2018-2020 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "audio_config.h"
#include "avb_stream_config.h"

#define DEFAULT_CB_PERIOD DEFAULT_BATCH_SIZE_NS /* 1ms */

// listener
const struct audio_param audio_playback_params[AUDIO_DEVICE_MAX_PLAYBACK_INSTANCES] = {
    [0] = {
        .avb_callback_period_ns = DEFAULT_CB_PERIOD,
        // TODO audio buffer size could be function of stream class
    },
    [1] = {
        .avb_callback_period_ns = DEFAULT_CB_PERIOD,
    },
    [2] = {
        .avb_callback_period_ns = DEFAULT_CB_PERIOD,
    },
#if (AUDIO_DEVICE_MAX_PLAYBACK_INSTANCES > 3)
    [3] = {
        .avb_callback_period_ns = DEFAULT_CB_PERIOD,
    },
#endif
};

// talker
const struct audio_param audio_capture_params[AUDIO_DEVICE_MAX_CAPTURE_INSTANCES] = {
    [0] = {
        .avb_callback_period_ns = DEFAULT_CB_PERIOD,
    },
    [1] = {
        .avb_callback_period_ns = DEFAULT_CB_PERIOD,
    },
    [2] = {
        .avb_callback_period_ns = DEFAULT_CB_PERIOD,
    },
#if (AUDIO_DEVICE_MAX_CAPTURE_INSTANCES > 3)
    [3] = {
        .avb_callback_period_ns = DEFAULT_CB_PERIOD,
    },
#endif
};
