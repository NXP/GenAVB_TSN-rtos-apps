/*
 * Copyright 2018-2019, 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <string.h>

#include "rtos_apps/types.h"

#include "avb_stream.h"

#include "avb_tsn/common/genavb.h"
#include "avb_tsn/common/log.h"

#include "genavb/genavb.h"

static struct avb_stream *avbstream_get_listener_stream(unsigned int unique_id)
{
    if (unique_id >= MAX_AVB_LISTENER_STREAMS)
        return NULL;

    return &avb_listener_streams[unique_id];
}

static struct avb_stream *avbstream_get_talker_stream(unsigned int unique_id)
{
    if (unique_id >= MAX_AVB_TALKER_STREAMS)
        return NULL;

    return &avb_talker_streams[unique_id];
}

unsigned int genavbstream_batch_size(unsigned int batch_size_ns, struct avb_stream_params *params)
{
    return (((uint64_t)batch_size_ns * avdecc_fmt_sample_rate(&params->format) * avdecc_fmt_sample_size(&params->format) + NSECS_PER_SEC - 1) / NSECS_PER_SEC);
}

int genavbstream_listener_add(unsigned int unique_id, struct genavb_stream_params *params, struct avb_stream **stream)
{
    int avb_result;
    struct avb_stream *avbstream;
    struct genavb_stream_params *stream_params;
    struct genavb_handle *handle;

    if ((handle = get_genavb_handle()) == NULL) {
        log_err("get_genavb_handle() failed: null genavb_handle\n");
        goto err;
    }

    if ((avbstream = avbstream_get_listener_stream(unique_id)) == NULL) {
        log_err("avbstream_get_listener_stream() failed: unique_id = %d\n", unique_id);
        goto err;
    }

    stream_params = &avbstream->stream_params;

    if (params)
        memcpy(stream_params, params, sizeof(*params));

    avbstream->batch_size_ns = rtos_apps_max(avbstream->batch_size_ns, sr_class_interval_p(avbstream->stream_params.stream_class) / sr_class_interval_q(avbstream->stream_params.stream_class));

    avbstream->is_first_wakeup = 1;
    //
    avbstream->cur_batch_size = genavbstream_batch_size(avbstream->batch_size_ns, stream_params);

    // Create new AVTP stream, update stream_handle
    if ((avb_result = genavb_stream_create(handle, &avbstream->stream_handle, stream_params, &avbstream->cur_batch_size, 0)) != GENAVB_SUCCESS) {
        log_err("genavb_stream_create() failed: %s\n", genavb_strerror(avb_result));
        goto err_stream_create;
    }

    if (stream)
        *stream = avbstream;

    return 0;

err_stream_create:
    avbstream->cur_batch_size = 0;
err:
    return -1;
}

int genavbstream_talker_add(unsigned int unique_id, struct genavb_stream_params *params, struct avb_stream **stream)
{
    int avb_result;
    struct avb_stream *avbstream;
    struct genavb_stream_params *stream_params;
    struct genavb_handle *handle;

    if ((handle = get_genavb_handle()) == NULL) {
        log_err("get_genavb_handle() failed: null genavb_handle\n");
        goto err;
    }

    if ((avbstream = avbstream_get_talker_stream(unique_id)) == NULL) {
        log_err("avbstream_get_talker_stream() failed: unique_id = %d\n", unique_id);
        goto err;
    }

    stream_params = &avbstream->stream_params;

    if (params)
        memcpy(stream_params, params, sizeof(*params));

    avbstream->batch_size_ns = rtos_apps_max(avbstream->batch_size_ns, sr_class_interval_p(avbstream->stream_params.stream_class) / sr_class_interval_q(avbstream->stream_params.stream_class));

    avbstream->is_first_wakeup = 1;
    //
    avbstream->cur_batch_size = genavbstream_batch_size(avbstream->batch_size_ns, stream_params);

    // Create new AVTP stream, update stream_handle
    if ((avb_result = genavb_stream_create(handle, &avbstream->stream_handle, stream_params, &avbstream->cur_batch_size, 0)) != GENAVB_SUCCESS) {
        log_err("genavb_stream_create() failed: %s\n", genavb_strerror(avb_result));
        goto err_stream_create;
    }

    if (stream)
        *stream = avbstream;

    return 0;

err_stream_create:
    avbstream->cur_batch_size = 0;
err:
    return -1;
}

static int __avbstream_remove(struct avb_stream *avbstream)
{
    int avb_result;
    if ((avb_result = genavb_stream_destroy(avbstream->stream_handle)) != GENAVB_SUCCESS) {
        log_err("genavb_stream_destroy() failed: %s\n", genavb_strerror(avb_result));
        return -1;
    }

    return 0;
}

int genavbstream_listener_remove(unsigned int unique_id)
{
    struct avb_stream *avbstream;

    if ((avbstream = avbstream_get_listener_stream(unique_id)) == NULL)
        return -1;

    return __avbstream_remove(avbstream);
}

int genavbstream_talker_remove(unsigned int unique_id)
{
    struct avb_stream *avbstream;

    if ((avbstream = avbstream_get_talker_stream(unique_id)) == NULL)
        return -1;

    return __avbstream_remove(avbstream);
}
