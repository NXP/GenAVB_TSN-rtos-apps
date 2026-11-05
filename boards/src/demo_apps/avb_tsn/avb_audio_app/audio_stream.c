/*
 * Copyright 2018-2020, 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FreeRTOS.h"
#include "queue.h"

#include "genavb/genavb.h"

#include "audio_stream.h"

#include "task_management.h"

#include "audio.h"
#include "avb_stream.h"
#include "avb_stream_config.h"

#include "avb_tsn/common/log.h"

#define talker_stats_dump(stream)                                                                           \
do {                                                                                                        \
    log_info("gen_avb_tx_bytes:              %llu\n", stream->audio_dev_handle.stats.avb_stack_treated_bytes);   \
    log_info("media_stack_rx_bytes:          %llu\n", stream->audio_dev_handle.stats.media_stack_treated_bytes); \
    log_info("audio_dev rx_err:              %u\n", stream->audio_dev_handle.stats.rx_err);                      \
    log_info("stream tx_err:                 %u\n", stream->avb_stream->stats.tx_err);                           \
    log_info("stream batch_tx:               %u\n", stream->avb_stream->stats.batch_tx);                         \
} while (0)

#define listener_stats_dump(stream)                                                                         \
do {                                                                                                        \
    log_info("gen_avb_rx_bytes:              %llu\n", stream->audio_dev_handle.stats.avb_stack_treated_bytes);   \
    log_info("media_stack_tx_bytes:          %llu\n", stream->audio_dev_handle.stats.media_stack_treated_bytes); \
    log_info("underflow_handler count:       %u\n", stream->stats.underflow_handler_count);                      \
    log_info("audio_dev tx_err:              %u\n", stream->audio_dev_handle.stats.tx_err);                      \
    log_info("audio_dev tx_avail_err:        %u\n", stream->audio_dev_handle.stats.tx_avail_err);                \
    log_info("audio_dev tx_start:            %u\n", stream->audio_dev_handle.stats.tx_start);                    \
    log_info("audio_dev tx_start_error:      %u\n", stream->audio_dev_handle.stats.tx_start_err);                \
    log_info("audio_dev tx_start_no_data:    %u\n", stream->audio_dev_handle.stats.tx_start_no_data);            \
    log_info("audio_dev tx_start_drop:       %u\n", stream->audio_dev_handle.stats.tx_start_drop);               \
    log_info("stream invalid_ts:             %u\n", stream->avb_stream->stats.invalid_timestamp);                \
    log_info("stream rx_err:                 %u\n", stream->avb_stream->stats.rx_err);                           \
    log_info("stream batch_rx:               %u\n", stream->avb_stream->stats.batch_rx);                         \
} while (0)

void audio_stats_dump(struct audio_stream *stream)
{
    log_info("stream ID:                     " STREAM_STR_FMT "\n", STREAM_STR(stream->avb_stream->stream_params.stream_id));
    log_info("audio_handler count:           %llu\n", stream->stats.handler_count);

    log_info("current_latency_bytes:         %lld\n", stream->audio_dev_handle.stats.current_latency_bytes);
    log_info("media_stack_cb_count:          %llu\n", stream->audio_dev_handle.stats.media_stack_cb_count);

    if (stream->is_listener)
        listener_stats_dump(stream);
    else
        talker_stats_dump(stream);

    log_info("stream gptp_2cont_wakeup:      %u\n", stream->avb_stream->stats.gptp_2cont_wakeup);

    if (stream->is_listener) {
        log_info("stream event_2cont_wakeup:     %u\n", stream->avb_stream->stats.event_2cont_wakeup);
        log_info("stream event_gptp:             %u\n", stream->avb_stream->stats.event_gptp);
    }
}

static int audio_rx_callback(void *data)
{
    struct task_slot *slot = (struct task_slot *)data;

    return task_slot_notify(slot);
}

static int talker_audio_handler(void *data)
{
    struct audio_stream *talker = (struct audio_stream *)data;

    talker->stats.handler_count++;

    audio_rx(&talker->audio_dev_handle, talker->avb_stream);

    return 0;
}

int talker_audio_connect(struct audio_stream *talker, struct genavb_stream_params *params)
{
    if (talker->created)
        goto err;

    talker->audio_dev_handle.instance = talker->index;
    talker->audio_dev_handle.dir = AUDIO_DEV_DIR_CAPTURE;

    talker->stats.handler_count = 0;

    if (task_slot_add(TASK_CAP_STREAM_TALKER, (void *)talker, &talker_audio_handler, &talker->audio_task_slot))
        goto err;

    if (audio_init(&talker->audio_dev_handle, params, &audio_rx_callback, talker->audio_task_slot))
        goto err_audio_rx_init;

    params->clock_domain = GENAVB_CLOCK_DOMAIN_0;

    if (genavbstream_talker_add(talker->index, params, &talker->avb_stream))
        goto err_talker_add;

#if 0
    if (task_slot_join(get_task(talker->audio_task_slot), (void *)talker, talker_audio_handler, &talker->avb_task_slot))
        goto err_slot_join;

    if (genavb_stream_set_callback(talker->avb_stream->stream_handle, &talker_stream_avb_stack_callback, talker->avb_task_slot) != GENAVB_SUCCESS)
        goto err_cb;
#endif

    audio_rx_start(&talker->audio_dev_handle, talker->avb_stream);

    talker->created = 1;

    return 0;

#if 0
err_cb:
    task_slot_free(talker->avb_task_slot);
err_slot_join:
    genavbstream_talker_remove(talker->index);
#endif
err_talker_add:
    audio_rx_exit(&talker->audio_dev_handle);
err_audio_rx_init:
    task_slot_free(talker->audio_task_slot);
err:
    return -1;
}

int talker_audio_disconnect(struct audio_stream *talker)
{
    if (!talker->created)
        goto err;

    if (audio_rx_exit(&talker->audio_dev_handle))
        goto err;

    if (genavbstream_talker_remove(talker->index))
        goto err;

#if 0
    if (task_slot_free(talker->avb_task_slot))
        goto err;
#endif

    if (task_slot_free(talker->audio_task_slot))
        goto err;

    talker->created = 0;

    return 0;

err:
    return -1;
}

/**
 * Called when lack of new data to output by audio device is detected
 */
static int audio_tx_underflow_callback(void *data)
{
    struct task_slot *slot = (struct task_slot *)data;

    return task_slot_notify(slot);
}

static int audio_tx_underflow_handler(void *data)
{
    struct audio_stream *listener = (struct audio_stream *)data;

    listener->stats.underflow_handler_count++;

    audio_stop(&listener->audio_dev_handle);

    audio_reset(&listener->audio_dev_handle);

    return 0;
}

/**
 * Called by GENAVB Stack each time new stream event occur
 */
static int listener_stream_avb_stack_callback(void *data)
{
    struct task_slot *slot = (struct task_slot *)data;

    return task_slot_notify(slot);
}

static int listener_audio_handler(void *data)
{
    struct audio_stream *listener = (struct audio_stream *)data;

    listener->stats.handler_count++;

    audio_tx(&listener->audio_dev_handle, listener->avb_stream);

    genavb_stream_enable_callback(listener->avb_stream->stream_handle);

    return 0;
}

int listener_audio_connect(struct audio_stream *listener, struct genavb_stream_params *params)
{
    if (listener->created)
        goto err;

    listener->audio_dev_handle.instance = listener->index;
    listener->audio_dev_handle.dir = AUDIO_DEV_DIR_PLAYBACK;

    listener->stats.handler_count = 0;

    // Add task slot to handle audio underflow events
    if (task_slot_add(TASK_CAP_STREAM_LISTENER, (void *)listener, &audio_tx_underflow_handler, &listener->audio_task_slot))
        goto err;

    // Init audio support according to stream parameters
    if (audio_init(&listener->audio_dev_handle, params, &audio_tx_underflow_callback, listener->audio_task_slot))
        goto err_audio_tx_init;

    params->clock_domain = GENAVB_CLOCK_DOMAIN_0;
    params->flags |= GENAVB_STREAM_FLAGS_MCR; // enable media clock recovery

    // create avb_stream
    if (genavbstream_listener_add(listener->index, params, &listener->avb_stream))
        goto err_listener_add;

    // Add handler with data to slot and get slot handler
    if (task_slot_join(get_task(listener->audio_task_slot), (void *)listener, &listener_audio_handler, &listener->avb_task_slot))
        goto err_slot_join;

    // Connect callback to avb stream and pass task slot handle as data
    if (genavb_stream_set_callback(listener->avb_stream->stream_handle, &listener_stream_avb_stack_callback, listener->avb_task_slot) != GENAVB_SUCCESS)
        goto err_cb;

    listener->created = 1;

    return 0;

err_cb:
    task_slot_free(listener->avb_task_slot);
err_slot_join:
    genavbstream_listener_remove(listener->index);
err_listener_add:
    audio_tx_exit(&listener->audio_dev_handle);
err_audio_tx_init:
    task_slot_free(listener->audio_task_slot);
err:
    return -1;
}

int listener_audio_disconnect(struct audio_stream *listener)
{
    if (!listener->created)
        goto err;

    if (genavbstream_listener_remove(listener->index))
        goto err;

    if (audio_tx_exit(&listener->audio_dev_handle))
        goto err;

    if (task_slot_free(listener->avb_task_slot))
        goto err;

    if (task_slot_free(listener->audio_task_slot))
        goto err;

    listener->created = 0;

    return 0;

err:
    return -1;
}
