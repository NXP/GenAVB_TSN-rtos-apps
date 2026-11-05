/*
 * Copyright 2018-2019 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _AUDIO_STREAM_H_
#define _AUDIO_STREAM_H_

#include "avb_stream_config.h"
#include "task_config.h"
#include "audio.h"

#define STATS_MAX_COUNTS 32

struct audio_stream_stats {
    unsigned long long handler_count;
    unsigned int underflow_handler_count;
};

struct audio_stream {
    unsigned int created;
    unsigned int index;
    bool is_listener;
    struct task_slot *avb_task_slot;
    struct task_slot *audio_task_slot;
    struct audio_device_handle audio_dev_handle;
    struct avb_stream *avb_stream;

    struct audio_stream_stats stats;
};

void audio_stats_dump(struct audio_stream *stream);
int talker_audio_connect(struct audio_stream *talker, struct genavb_stream_params *params);
int talker_audio_disconnect(struct audio_stream *talker);
int listener_audio_connect(struct audio_stream *listener, struct genavb_stream_params *params);
int listener_audio_disconnect(struct audio_stream *listener);

#endif /* _AUDIO_STREAM_H_ */
