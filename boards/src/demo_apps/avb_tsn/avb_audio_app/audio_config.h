/*
 * Copyright 2018-2019 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _AUDIO_CONFIG_H_
#define _AUDIO_CONFIG_H_

#include "media_stack/config.h"

struct audio_param {
    unsigned int avb_callback_period_ns;
};

extern const struct audio_param audio_playback_params[AUDIO_DEVICE_MAX_PLAYBACK_INSTANCES];
extern const struct audio_param audio_capture_params[AUDIO_DEVICE_MAX_CAPTURE_INSTANCES];

#endif /* _AUDIO_CONFIG_H_ */
