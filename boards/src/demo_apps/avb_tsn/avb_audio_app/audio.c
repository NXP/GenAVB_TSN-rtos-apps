/*
 * Copyright 2018-2020, 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "rtos_apps/types.h"

#include "FreeRTOS.h"
#include "task.h"
#include "audio.h"
#include "audio_config.h"
#include "avb_tsn/common/log.h"

#include "genavb/clock.h"

static void media_stack_rx_callback(uint32_t status, void *data)
{
    struct audio_device_handle *audio_dev_h = (struct audio_device_handle *)data;
    audio_dev_h->stats.media_stack_cb_count++;

    if (audio_dev_h->cb)
        audio_dev_h->cb(audio_dev_h->cb_data);
}

static void media_stack_tx_callback(uint32_t status, void *data)
{
    struct audio_device_handle *audio_dev_h = (struct audio_device_handle *)data;
    audio_dev_h->stats.media_stack_cb_count++;

    // check if we still receive data from genavb stack
    if (audio_dev_h->stats.avb_stack_treated_bytes_stamp == audio_dev_h->stats.avb_stack_treated_bytes)
        if (audio_dev_h->cb)
            audio_dev_h->cb(audio_dev_h->cb_data);

    audio_dev_h->stats.avb_stack_treated_bytes_stamp = audio_dev_h->stats.avb_stack_treated_bytes;
}

static void init_audio_dev_stats(struct audio_device_handle *audio_dev_h)
{
    audio_dev_h->stats.media_stack_cb_count = 0;
    audio_dev_h->stats.media_stack_treated_bytes = 0;
    audio_dev_h->stats.avb_stack_treated_bytes = 0;
    audio_dev_h->stats.media_stack_last_buffer_position = 0;
    audio_dev_h->stats.current_latency_bytes = 0;
    audio_dev_h->stats.avb_stack_treated_bytes_stamp = 0;
}

/**
 * This function do the endianness conversion swap (network order BE -> LE) then adjust padding for AAF 24/32 bits format for output direction (stream listener)
 */
static void audio_swap_data_32_adjust_padding_s24_le_output(struct audio_device_handle *audio_dev_h, void *src_frame, unsigned int samples_to_commit)
{
    uint32_t *src_sample;
    int i;
    unsigned int bytes_per_sample = audio_dev_h->frame_size / audio_dev_h->config.audio_interface.channels;

    if (bytes_per_sample != 4)
        return;

    src_sample = (uint32_t *)src_frame;

    /* Do endianess conversion */
    /* Adjust padding: move unused bits from LSB (lower bits) to MSB (upper bits)*/
    for (i = 0; i < samples_to_commit; i++)
        src_sample[i] = bswap_32(src_sample[i]) >> 8;
}

static void audio_swap_data_32(struct audio_device_handle *audio_dev_h, void *src_frame, unsigned int samples_to_commit)
{
    uint32_t *src_sample;
    int i;
    unsigned int bytes_per_sample = audio_dev_h->frame_size / audio_dev_h->config.audio_interface.channels;

    if (bytes_per_sample != 4)
        return;

    src_sample = (uint32_t *)src_frame;

    /* Do endianess conversion */
    for (i = 0; i < samples_to_commit; i++)
        src_sample[i] = bswap_32(src_sample[i]);
}

static unsigned int audio_ns_to_samples(unsigned int ns, struct avb_stream_params *stream_params)
{
    return (((unsigned long long)ns * avdecc_fmt_sample_rate(&stream_params->format) + NSECS_PER_SEC - 1) / NSECS_PER_SEC);
}

static unsigned int audio_bytes_to_ns(unsigned int bytes, struct avb_stream_params *stream_params)
{
    return (((unsigned long long)bytes * NSECS_PER_SEC) / ((unsigned long long)avdecc_fmt_sample_rate(&stream_params->format) * avdecc_fmt_sample_size(&stream_params->format)));
}

int audio_init(struct audio_device_handle *audio_dev_h, struct genavb_stream_params *stream_params, int (*cb)(void *data), void *cb_data)
{
    unsigned int audio_dev_instance = audio_dev_h->instance;
    const struct audio_param *params;
    uint32_t period_size_samples;
    uint32_t buffer_size_samples;
    int result;

    if ((result = audio_device_init(audio_dev_instance, audio_dev_h->dir)) != AUDIO_DEV_SUCCESS) {
        log_err("audio_device_init() failed: error %d\n", result);
        goto err;
    }

    if (audio_dev_h->dir == AUDIO_DEV_DIR_PLAYBACK) {
        params = &audio_playback_params[audio_dev_h->instance];
    } else {
        params = &audio_capture_params[audio_dev_h->instance];
    }

    audio_dev_h->buffer_offset = 0;
    audio_dev_h->frame_size = avdecc_fmt_sample_size(&stream_params->format);
    if (!audio_dev_h->frame_size) {
        log_err("invalid frame_size\n");
        goto err_format;
    }

    audio_dev_h->rate = avdecc_fmt_sample_rate(&stream_params->format);
    if (!audio_dev_h->rate) {
        log_err("invalid rate\n");
        goto err_format;
    }

    audio_dev_h->frame_duration = NSECS_PER_SEC / audio_dev_h->rate;

    audio_dev_h->cb = cb;
    audio_dev_h->cb_data = cb_data;

    audio_dev_h->config.audio_interface.sampleRate_Hz = audio_dev_h->rate;
    audio_dev_h->config.audio_interface.bitWidth = avdecc_fmt_bits_per_sample(&stream_params->format) - avdecc_fmt_unused_bits(&stream_params->format);
    audio_dev_h->config.audio_interface.channels = avdecc_fmt_channels_per_sample(&stream_params->format);

    /*Set the right process sample function*/
    if (avdecc_format_is_aaf_pcm(&stream_params->format)) {
        switch (avdecc_fmt_bits_per_sample(&stream_params->format)) {
        case 32:
            if (avdecc_fmt_unused_bits(&stream_params->format) == 8) {
                audio_dev_h->process_samples = &audio_swap_data_32_adjust_padding_s24_le_output;
            } else {
                log_err("Unsupported bit depth for 32bits sample\n");
                goto err_format;
            }

            break;
        default:
            log_err("Unsupported Alsa format\n");
            goto err_format;
        }
    } else if (avdecc_format_is_61883_6(&stream_params->format) && (AVDECC_FMT_61883_6_FDF_EVT(&stream_params->format) == IEC_61883_6_FDF_EVT_AM824)) {
        audio_dev_h->process_samples = &audio_swap_data_32;
    } else {
        log_err("Unsupported AVDECC format\n");
        goto err_format;
    }

    period_size_samples = audio_ns_to_samples(params->avb_callback_period_ns, stream_params);

    if (audio_dev_h->dir == AUDIO_DEV_DIR_PLAYBACK) {
        buffer_size_samples = audio_ns_to_samples(sr_class_max_transit_time(stream_params->stream_class) + CFG_AUDIO_PLAYBACK_LATENCY_NS, stream_params) + 2 * period_size_samples;
        buffer_size_samples = ((buffer_size_samples + period_size_samples - 1) / period_size_samples) * period_size_samples;

        audio_dev_h->period_size_samples = period_size_samples;
        audio_dev_h->config.buffer_size = buffer_size_samples * avdecc_fmt_sample_size(&stream_params->format);

        audio_dev_h->tx_buffer = pvPortMalloc(audio_dev_h->config.buffer_size);
        if (!audio_dev_h->tx_buffer) {
            log_err("pvPortMalloc failed\n");
            goto err_malloc;
        }
        memset(audio_dev_h->tx_buffer, 0, audio_dev_h->config.buffer_size);

        audio_dev_h->config.buffer_addr = audio_dev_h->tx_buffer;
        audio_dev_h->config.callback_size = audio_dev_h->config.buffer_size / 2;
        audio_dev_h->config.callback = &media_stack_tx_callback;
        audio_dev_h->config.callback_data = (void *)audio_dev_h;
    } else {
        period_size_samples = audio_ns_to_samples(params->avb_callback_period_ns, stream_params);
        buffer_size_samples = period_size_samples * 2;

        audio_dev_h->period_size_samples = period_size_samples;
        audio_dev_h->config.buffer_size = buffer_size_samples * avdecc_fmt_sample_size(&stream_params->format);

        audio_dev_h->rx_buffer = pvPortMalloc(audio_dev_h->config.buffer_size);
        if (!audio_dev_h->rx_buffer) {
            log_err("pvPortMalloc failed\n");
            goto err_malloc;
        }
        memset(audio_dev_h->rx_buffer, 0, audio_dev_h->config.buffer_size);

        audio_dev_h->config.buffer_addr = audio_dev_h->rx_buffer;
        audio_dev_h->config.callback_size = audio_dev_h->config.buffer_size / 2;
        audio_dev_h->config.callback = &media_stack_rx_callback;
        audio_dev_h->config.callback_data = (void *)audio_dev_h;
    }

    init_audio_dev_stats(audio_dev_h);

    if (audio_dev_h->config.buffer_size % audio_dev_h->frame_size) {
        log_err("Buffer size not aligned\n");
        goto err_buff_size;
    }

    if ((result = audio_device_config(audio_dev_instance, &audio_dev_h->config, audio_dev_h->dir)) != AUDIO_DEV_SUCCESS) {
        log_err("audio_device_config() failed: error %d\n", result);
        goto err_dev_config;
    }

    audio_dev_h->stats.media_stack_treated_bytes = 0;

    audio_dev_h->flags = 0;

    log_debug("audio_dev_instance = %d\n", audio_dev_instance);
    log_debug("frame_size = %d\n", audio_dev_h->frame_size);
    log_debug("sampleRate_Hz = %d\n", audio_dev_h->config.audio_interface.sampleRate_Hz);
    log_debug("bitWidth = %d\n", audio_dev_h->config.audio_interface.bitWidth);
    log_debug("channels = %d\n", audio_dev_h->config.audio_interface.channels);
    log_debug("buffer_size = %d\n", audio_dev_h->config.buffer_size);

    return 0;

err_dev_config:
    memset(&audio_dev_h->config, 0, sizeof(struct audio_device_config));
err_buff_size:
    if (audio_dev_h->dir == AUDIO_DEV_DIR_PLAYBACK)
        vPortFree(audio_dev_h->tx_buffer);
    else
        vPortFree(audio_dev_h->rx_buffer);
err_malloc:
err_format:
    audio_device_exit(audio_dev_instance, audio_dev_h->dir);
err:
    return -1;
}

static int32_t update_audio_dev_latency(struct audio_device_handle *audio_dev_h, int is_listener)
{
    unsigned media_stack_buffer_position;

    // Media stack position in audio ring buffer
    media_stack_buffer_position = (audio_dev_h->config.buffer_size - audio_device_get_remaining_buffer_bytes(audio_dev_h->instance, audio_dev_h->dir));

    if (media_stack_buffer_position >= audio_dev_h->stats.media_stack_last_buffer_position)
        audio_dev_h->stats.media_stack_treated_bytes += media_stack_buffer_position - audio_dev_h->stats.media_stack_last_buffer_position;
    else
        audio_dev_h->stats.media_stack_treated_bytes += media_stack_buffer_position + audio_dev_h->config.buffer_size - audio_dev_h->stats.media_stack_last_buffer_position;

    audio_dev_h->stats.media_stack_last_buffer_position = media_stack_buffer_position;

    if (is_listener)
        audio_dev_h->stats.current_latency_bytes = audio_dev_h->stats.avb_stack_treated_bytes - audio_dev_h->stats.media_stack_treated_bytes;
    else
        audio_dev_h->stats.current_latency_bytes = audio_dev_h->stats.media_stack_treated_bytes - audio_dev_h->stats.avb_stack_treated_bytes;

    return audio_dev_h->stats.current_latency_bytes;
}

/**
 * try to read @size bytes from avb and fill audio ring buffer
 * if buffer offset is too big to fill @size bytes, then fill only the free tail
 */
static int read_from_avb(struct audio_device_handle *audio_dev_h, struct avb_stream *avb_stream, unsigned int size, struct genavb_event *event, unsigned int *event_len)
{
    int read_bytes;
    unsigned int data_len;
    void *addr = audio_dev_h->tx_buffer + audio_dev_h->buffer_offset;
    unsigned int bytes_per_sample = audio_dev_h->frame_size / audio_dev_h->config.audio_interface.channels;

    if (audio_dev_h->config.buffer_size - audio_dev_h->buffer_offset >= size) {
        // if there is more, or equal than size free bytes in audio ring buffer, read size bytes from avb
        data_len = size;
    } else {
        // there is less than size bytes left in audio ring buffer: fill free tail of the buffer
        data_len = audio_dev_h->config.buffer_size - audio_dev_h->buffer_offset;
    }

    if ((read_bytes = genavb_stream_receive(avb_stream->stream_handle, addr, data_len, event, event_len)) < 0) {
        return -1;
    }

    if (audio_dev_h->process_samples)
        audio_dev_h->process_samples(audio_dev_h, addr, read_bytes / bytes_per_sample);

    // Update offset
    audio_dev_h->buffer_offset += read_bytes;
    audio_dev_h->buffer_offset %= audio_dev_h->config.buffer_size;

    return read_bytes;
}

static int send_to_avb(struct audio_device_handle *audio_dev_h, struct avb_stream *avb_stream, unsigned int size, struct genavb_event *event, unsigned int event_len)
{
    unsigned int data_len;
    void *addr = audio_dev_h->rx_buffer + audio_dev_h->buffer_offset;
    unsigned int bytes_per_sample = audio_dev_h->frame_size / audio_dev_h->config.audio_interface.channels;

    if (audio_dev_h->config.buffer_size - audio_dev_h->buffer_offset >= size) {
        data_len = size;
    } else {
        data_len = audio_dev_h->config.buffer_size - audio_dev_h->buffer_offset;
    }

    if (audio_dev_h->process_samples)
        audio_dev_h->process_samples(audio_dev_h, addr, data_len / bytes_per_sample);

    return genavb_stream_send(avb_stream->stream_handle, addr, data_len, event, event_len);
}

static inline unsigned int audio_compute_silence(struct audio_device_handle *audio_dev_h, unsigned int max_frames, unsigned int *desired_time, unsigned int now, unsigned int total, unsigned int pcm_start)
{
    unsigned int silence_frames;
    unsigned int max = max_frames - (total - pcm_start);

    if (avtp_after(now, *desired_time)) {
        if (total == pcm_start) { // We're late on the first iteration
            log_err("audio_tx(%p) desired time (%u) likely in the past (now = %u), resetting to %d ns in the future\n",
                audio_dev_h, *desired_time, now, CFG_AUDIO_PLAYBACK_LATENCY_NS);
            *desired_time = now + CFG_AUDIO_PLAYBACK_LATENCY_NS;
        } else { // We got late while adding silence, let's stop there
            log_err("audio_tx(%p) desired time (%u) likely in the past, resetting to now(%u)\n",
                audio_dev_h, *desired_time, now);
            *desired_time = now;
        }
    }

    silence_frames = *desired_time - now;
    silence_frames = ((uint64_t)silence_frames * audio_dev_h->config.audio_interface.sampleRate_Hz + (NSECS_PER_SEC / 2)) / NSECS_PER_SEC;

    if (silence_frames >= total)
        silence_frames -= total;
    else {
        log_err("audio_tx(%p) now(%u) desired_time(%u) Added too many frames (%u instead of %u)\n",
            audio_dev_h, now, *desired_time, total, silence_frames);
        silence_frames = 0;
    }

    if (silence_frames > max) {
        log_err("audio_tx(%p) Amount of silence to add (%u) exceeds max space available, clamping to %u\n",
            audio_dev_h, silence_frames, max);
        silence_frames = max;
    }

    return silence_frames;
}

static int listener_timestamp_accept(unsigned int ts, unsigned int now, struct avb_stream *avbstream)
{
    /* Timestamp + playback offset must be after now (otherwise packet are too late) */
    /* Timestamp must be before now + transit time + timing uncertainty (otherwise they arrived too early) */
    if (avtp_after(ts + CFG_AUDIO_PLAYBACK_LATENCY_NS, now) &&
        avtp_before(ts, now + sr_class_max_transit_time(avbstream->stream_params.stream_class) + sr_class_max_timing_uncertainty(avbstream->stream_params.stream_class)))
        return 1;

    log_debug("avb(%p) Invalid timestamp, ts - now: %d\n", avbstream->stream_handle, ts - now);

    return 0;
}

static void snd_silence(struct audio_device_handle *audio_dev_h, unsigned int silence_frames)
{
    int silence_bytes = silence_frames * audio_dev_h->frame_size;

    if (silence_bytes + audio_dev_h->buffer_offset < audio_dev_h->config.buffer_size) {
        // enough place in the buffer
        memset(audio_dev_h->tx_buffer + audio_dev_h->buffer_offset, 0, silence_bytes);
    } else {
        // not enought place in the tail, slience needs to wrap
        int tail_bytes = audio_dev_h->config.buffer_size - audio_dev_h->buffer_offset;
        // make sure we do not overflow
        silence_bytes %= audio_dev_h->config.buffer_size;
        // fill the tail of the buffer
        memset(audio_dev_h->tx_buffer + audio_dev_h->buffer_offset, 0, tail_bytes);
        // and the beginning
        memset(audio_dev_h->tx_buffer, 0, silence_bytes - tail_bytes);
    }

    audio_dev_h->buffer_offset += silence_bytes;
    audio_dev_h->buffer_offset %= audio_dev_h->config.buffer_size;
}

#define EVENT_LEN 16
int audio_tx(struct audio_device_handle *audio_dev_h, struct avb_stream *avb_stream)
{
    int ret = 0;
    uint64_t gptp_time;
    unsigned int frames_remaining, start_frames, frames_to_commit, frames_committed;
    int avail_bytes, exchanged_bytes = 0;
    bool is_first_event = true;
    uint64_t ts_offset;

    if (!genavb_clock_gettime64(GENAVB_CLOCK_GPTP_0_0, &gptp_time)) {
        if (avb_stream->is_first_wakeup) {
            // First wakeup, just store the time
            avb_stream->last_gptp_time = gptp_time;
        } else {
            avb_stream->stats.gptp_2cont_wakeup = gptp_time - avb_stream->last_gptp_time;
            avb_stream->last_gptp_time = gptp_time;
        }
    }

    if (!(audio_dev_h->flags & AUDIO_DEVICE_FLAGS_RUNNING)) {
        ret = audio_tx_start(audio_dev_h, avb_stream);
        if (ret < 0) {
            audio_dev_h->stats.tx_start_err++;
            return 0;
        }
    }

    start_frames = ret;
    avail_bytes = audio_dev_h->stats.current_latency_bytes;
    if (ret < 0 || avail_bytes < avb_stream->cur_batch_size || avail_bytes >= audio_dev_h->config.buffer_size) {
        audio_dev_h->stats.tx_avail_err++;

        audio_dev_h->flags &= ~AUDIO_DEVICE_FLAGS_RUNNING;

        ret = audio_tx_start(audio_dev_h, avb_stream);
        if (ret < 0)
            audio_dev_h->stats.tx_start_err++;

        return ret;
    }

    frames_remaining = rtos_apps_min(avail_bytes / audio_dev_h->frame_size, (avb_stream->cur_batch_size / audio_dev_h->frame_size) - start_frames);
    while (frames_remaining > 0) {
        int bytes_to_read, read_bytes;
        struct genavb_event event[EVENT_LEN];
        unsigned int event_len;

        frames_to_commit = frames_remaining;
        bytes_to_read = frames_to_commit * audio_dev_h->frame_size;
        event_len = EVENT_LEN;

        read_bytes = read_from_avb(audio_dev_h, avb_stream, bytes_to_read, event, &event_len);
        if (read_bytes < audio_dev_h->frame_size) {
            avb_stream->stats.rx_err++;
            return 0;
        }

        if (event_len) {
            int idx, i;

            // Check if AVTP packet lost occurs on starting of batch
            for (i = 0; i < event_len; i++) {
                if (event[i].event_mask & AVTP_PACKET_LOST) {
                    avb_stream->stats.rx_err++;

                    // Restart audio tx
                    audio_dev_h->flags &= ~AUDIO_DEVICE_FLAGS_RUNNING;
                    ret = audio_tx_start(audio_dev_h, avb_stream);
                    if (ret < 0)
                        audio_dev_h->stats.tx_start_err++;

                    return ret;
                }
            }

            i = 0;
            while ((i < event_len) && (event[i].event_mask & (AVTP_TIMESTAMP_INVALID | AVTP_TIMESTAMP_UNCERTAIN)))
                i++;

            for (idx = i; idx < event_len; idx++) {

                if (event[idx].event_mask & (AVTP_TIMESTAMP_INVALID | AVTP_TIMESTAMP_UNCERTAIN))
                    continue;

                ts_offset = audio_bytes_to_ns(event[idx].index, &avb_stream->stream_params);

                if (!listener_timestamp_accept(event[idx].ts - ts_offset, gptp_time, avb_stream)) {
                    avb_stream->stats.invalid_timestamp++;

                    audio_dev_h->flags &= ~AUDIO_DEVICE_FLAGS_RUNNING;
                    ret = audio_tx_start(audio_dev_h, avb_stream);
                    if (ret < 0)
                        audio_dev_h->stats.tx_start_err++;

                    return ret;
                }
            }

            if (i < event_len) {
                if (avb_stream->is_first_wakeup) {
                    avb_stream->last_event_ts = event[i].ts;
                    avb_stream->last_event_frame_offset = avb_stream->last_exchanged_frames + event[i].index / audio_dev_h->frame_size;
                } else if (is_first_event) {
                    unsigned int dt_elapsed = event[i].ts - avb_stream->last_event_ts;
                    unsigned int event_frame_offset = avb_stream->last_exchanged_frames + event[i].index / audio_dev_h->frame_size;
                    unsigned int frames_elapsed = event_frame_offset - avb_stream->last_event_frame_offset;

                    avb_stream->last_event_ts = event[i].ts;
                    avb_stream->last_event_frame_offset = event_frame_offset;

                    if (avb_stream->last_exchanged_frames && (abs(dt_elapsed - frames_elapsed * audio_dev_h->frame_duration) > audio_dev_h->frame_duration)) {

                        avb_stream->stats.rx_err++;

                        audio_dev_h->flags &= ~AUDIO_DEVICE_FLAGS_RUNNING;
                        ret = audio_tx_start(audio_dev_h, avb_stream);
                        if (ret < 0)
                            audio_dev_h->stats.tx_start_err++;

                        return ret;
                    }

                    is_first_event = false;
                    avb_stream->stats.event_2cont_wakeup = dt_elapsed;
                    avb_stream->stats.event_gptp = event[i].ts - gptp_time;
                }
            } else { // Invalid time stamp
                audio_dev_h->stats.tx_err++;
            }
        }
        avb_stream->stats.batch_rx++;
        frames_committed = read_bytes / audio_dev_h->frame_size;

        exchanged_bytes += read_bytes;

        if ((read_bytes < bytes_to_read) && (event_len < EVENT_LEN))
            break;

        frames_remaining -= frames_committed;
    }
    audio_dev_h->stats.avb_stack_treated_bytes += exchanged_bytes;
    update_audio_dev_latency(audio_dev_h, true);

    avb_stream->last_exchanged_frames += exchanged_bytes / audio_dev_h->frame_size;

    if (avb_stream->is_first_wakeup) {
        // First wakeup, just store the time
        avb_stream->is_first_wakeup = 0;
    }

    return exchanged_bytes;
}

int audio_rx(struct audio_device_handle *audio_dev_h, struct avb_stream *avb_stream)
{
    uint64_t gptp_time;
    int avail_bytes, avail_frames;
    int ret;
    unsigned int frames_remaining, frames_to_commit;
    unsigned int bytes_to_read, exchanged_bytes = 0;
    int sent_bytes;
    struct genavb_event event;

    if (!genavb_clock_gettime64(GENAVB_CLOCK_GPTP_0_0, &gptp_time)) {
        if (avb_stream->is_first_wakeup) {
            // First wakeup, just store the time
            avb_stream->last_gptp_time = gptp_time;
            avb_stream->is_first_wakeup = 0;
        } else {
            avb_stream->stats.gptp_2cont_wakeup = gptp_time - avb_stream->last_gptp_time;
            avb_stream->last_gptp_time = gptp_time;
        }
    }

    avail_bytes = update_audio_dev_latency(audio_dev_h, false);
    if ((avail_bytes < avb_stream->cur_batch_size) || (avail_bytes >= audio_dev_h->config.buffer_size)) {

        audio_dev_h->stats.rx_err++;

        audio_dev_h->flags &= ~AUDIO_DEVICE_FLAGS_RUNNING;
        audio_reset(audio_dev_h);
        audio_rx_start(audio_dev_h, avb_stream);

        return 0;
    }

    avail_frames = avail_bytes / audio_dev_h->frame_size;
    if (avail_frames > audio_dev_h->period_size_samples)
        frames_remaining = audio_dev_h->period_size_samples;
    else
        frames_remaining = avail_frames;

    do {
        frames_to_commit = frames_remaining;
        bytes_to_read = frames_to_commit * audio_dev_h->frame_size;

        ret = bytes_to_read;
        do {
            if (audio_dev_h->flags & AUDIO_DEVICE_FIRST_READ) {
                event.index = 0;
                event.event_mask = AVTP_SYNC;
                event.ts = audio_dev_h->start_time + genavb_stream_presentation_offset(avb_stream->stream_handle);

                sent_bytes = send_to_avb(audio_dev_h, avb_stream, ret, &event, 1);

                audio_dev_h->flags &= ~AUDIO_DEVICE_FIRST_READ;
            } else {
                sent_bytes = send_to_avb(audio_dev_h, avb_stream, ret, NULL, 0);
            }

            if (sent_bytes <= 0) {
                avb_stream->stats.tx_err++;
                break;
            }
            avb_stream->stats.batch_tx++;

            ret -= sent_bytes;
        } while (ret > 0);
        exchanged_bytes += bytes_to_read;

        audio_dev_h->buffer_offset += bytes_to_read;
        audio_dev_h->buffer_offset %= audio_dev_h->config.buffer_size;

        audio_dev_h->stats.avb_stack_treated_bytes += bytes_to_read;

        frames_remaining -= frames_to_commit;
    } while (frames_remaining > 0);

    return exchanged_bytes;
}

int audio_tx_start(struct audio_device_handle *audio_dev_h, struct avb_stream *avb_stream)
{
    int err;
    struct genavb_event event[MAX_EVENTS];
    unsigned int event_len;
    int idx;
    uint64_t tstamp, now, t_start, t_end;
    unsigned int desired_time, pcm_start_overhead;
    unsigned int silence_frames, total_silence_frames, frames_written, frames_dropped, frames_read;
    unsigned int bytes_to_read;
    unsigned int nsecs_per_frame = NSECS_PER_SEC / audio_dev_h->config.audio_interface.sampleRate_Hz;
    char *buf;
    bool end;
    uint64_t ts_offset;

    log_debug("audio_tx_start audio_dev_h(%p)\n", audio_dev_h);

    if (audio_dev_h->flags & AUDIO_DEVICE_FLAGS_RUNNING)
        return 0;

    audio_reset(audio_dev_h);

    audio_dev_h->stats.tx_start++;
    avb_stream->last_exchanged_frames = 0;
    avb_stream->last_event_frame_offset = 0;

    frames_dropped = 0;
    frames_read = 0;
    frames_written = 0;

    buf = pvPortMalloc(START_COPY_SIZE);
    if (!buf) {
        log_err("pvPortMalloc failed\n");
        err = -1;
        goto exit;
    }

    do {
        event_len = MAX_EVENTS;

        err = genavb_stream_receive(avb_stream->stream_handle, buf, rtos_apps_min(START_COPY_SIZE, avb_stream->cur_batch_size), event, &event_len);
        if (err < 0) {
            log_err("audio tx start failed on genavb_stream_receive err(%d) event_len: %d\n", err, event_len);
            err = -1;
            goto exit;
        } else if (!err) {
            audio_dev_h->stats.tx_start_no_data++;
            err = -1;
            goto exit;
        }

        frames_dropped += frames_read;
        frames_read = err / audio_dev_h->frame_size;

        audio_dev_h->stats.tx_start_drop += frames_dropped;
        if (frames_dropped >= (MAX_DROPPED_PERIOD * audio_dev_h->period_size_samples)) {
            err = -1;
            goto exit;
        }

        err = genavb_clock_gettime64(GENAVB_CLOCK_GPTP_0_0, &now);
        if (err < 0) {
            log_err("genavb_clock_gettime64 failed err:%d\n", err);
            goto exit;
        }

        if (event_len == 0) {
            log_err("audio tx start failed, no events received\n");
            err = -1;
            goto exit;
        }

        for (idx = 0; idx < event_len; idx++) {
            if (event[idx].event_mask & (AVTP_TIMESTAMP_INVALID | AVTP_TIMESTAMP_UNCERTAIN))
                continue;

            ts_offset = audio_bytes_to_ns(event[idx].index, &avb_stream->stream_params);
            event[idx].ts -= ts_offset;

            tstamp = event[idx].ts;
            if (listener_timestamp_accept(tstamp, now, avb_stream))
                goto start;
            else
                avb_stream->stats.invalid_timestamp++;
        }
    } while (1);

start:

    err = genavb_clock_gettime64(GENAVB_CLOCK_GPTP_0_0, &t_start);
    if (err < 0) {
        log_err("genavb_clock_gettime64 failed err:%d\n", err);
        goto exit;
    }

    pcm_start_overhead = PCM_START_DELAY_SAMPLES;
    desired_time = tstamp + CFG_AUDIO_PLAYBACK_LATENCY_NS;

    total_silence_frames = pcm_start_overhead;

    /**
     * Since the process of adding silence frames takes time by itself, we account for that delay by
     * adding silence in several steps with a progressively lower number of frames at each step, and
     * stop once the number of frames to be added is low enough.
     */
    end = false;
    while (!end) {

        taskENTER_CRITICAL();

        err = genavb_clock_gettime64(GENAVB_CLOCK_GPTP_0_0, &now);
        if (err < 0) {
            log_err("genavb_clock_gettime64 failed err:%d\n", err);
            taskEXIT_CRITICAL();
            goto exit;
        }

        silence_frames = audio_compute_silence(audio_dev_h, audio_dev_h->config.buffer_size / audio_dev_h->frame_size, &desired_time, now, total_silence_frames, pcm_start_overhead);

        if (silence_frames > CFG_AUDIO_MIN_SILENCE_FRAMES) {
            silence_frames -= silence_frames >> 2;
        } else {
            end = true;
            if ((err = audio_device_start(audio_dev_h->instance, audio_dev_h->dir)) != AUDIO_DEV_SUCCESS) {
                taskEXIT_CRITICAL();
                goto exit;
            }

            err = genavb_clock_gettime64(GENAVB_CLOCK_GPTP_0_0, &t_end);
            if (err < 0) {
                log_err("genavb_clock_gettime64 failed err:%d\n", err);
                taskEXIT_CRITICAL();
                goto exit;
            }
        }

        taskEXIT_CRITICAL();

        snd_silence(audio_dev_h, silence_frames);

        total_silence_frames += silence_frames;
        frames_written += silence_frames;
    }

    total_silence_frames -= pcm_start_overhead;

    /* Start processing the first sample with a valid timestamp to (shifted because of the index offset) improve accuracy.  */
    bytes_to_read = frames_read * audio_dev_h->frame_size;
    memcpy(audio_dev_h->tx_buffer + audio_dev_h->buffer_offset, buf, bytes_to_read);
    audio_dev_h->buffer_offset += bytes_to_read;
    audio_dev_h->buffer_offset %= audio_dev_h->config.buffer_size;

    frames_written += frames_read;

    audio_dev_h->stats.avb_stack_treated_bytes += frames_written * audio_dev_h->frame_size;
    update_audio_dev_latency(audio_dev_h, true);

    audio_dev_h->flags |= AUDIO_DEVICE_FLAGS_RUNNING;

    log_debug("avtp_ts = %u   desired = %u   now = %llu     t_start = %u\n", tstamp, desired_time, now, t_start);
    log_debug("avtp_ts - now = %u us\n", (uint32_t)(tstamp - now) / 1000);
    log_debug("total_silence_frames = %u     computed silence = %u us     nsecs_per_frame = %d\n", total_silence_frames, (total_silence_frames * nsecs_per_frame) / 1000, nsecs_per_frame);
    log_debug("computation time %u ns     error = %d us     \n", (uint32_t)(t_end - t_start), (total_silence_frames * nsecs_per_frame) / 1000 - (desired_time - (uint32_t)t_end) / 1000);
    /* just to avoid unused variable warning */
    (void)nsecs_per_frame;

exit:
    vPortFree(buf);

    return frames_read;
}

#define AUDIO_RX_LATENCY 0
int audio_rx_start(struct audio_device_handle *audio_dev_h, struct avb_stream *avb_stream)
{
    int result;
    int err = 0;
    unsigned int nsecs_per_frame = NSECS_PER_SEC / audio_dev_h->config.audio_interface.sampleRate_Hz;
    const struct audio_param *params = &audio_capture_params[audio_dev_h->instance];
    uint64_t gptp_time;

    log_debug("audio_dev_h(%p)\n", audio_dev_h);

    if (audio_dev_h->flags & AUDIO_DEVICE_FLAGS_RUNNING)
        return 0;

    err = genavb_clock_gettime64(GENAVB_CLOCK_GPTP_0_0, &gptp_time);
    if (err < 0) {
        log_err("genavb_clock_gettime64 failed err:%d\n", err);
        goto exit;
    }
    audio_dev_h->start_time = gptp_time & 0xffffffff;

    if ((result = audio_device_start(audio_dev_h->instance, audio_dev_h->dir)) != AUDIO_DEV_SUCCESS) {
        log_err("audio_device_start() failed: error %d\n", result);
        return -1;
    }

    audio_dev_h->start_time += PCM_START_DELAY_SAMPLES * nsecs_per_frame + params->avb_callback_period_ns + AUDIO_RX_LATENCY;
    audio_dev_h->flags |= (AUDIO_DEVICE_FLAGS_RUNNING | AUDIO_DEVICE_FIRST_READ);

exit:
    return err;
}

int audio_stop(struct audio_device_handle *audio_dev_h)
{
    int result;

    log_debug("audio_dev_h(%p)\n", audio_dev_h);

    if (!(audio_dev_h->flags & AUDIO_DEVICE_FLAGS_RUNNING))
        return 0;

    if ((result = audio_device_stop(audio_dev_h->instance, audio_dev_h->dir)) != AUDIO_DEV_SUCCESS) {
        log_err("audio_device_stop() failed: error %d\n", result);
        return -1;
    }

    audio_dev_h->flags &= ~AUDIO_DEVICE_FLAGS_RUNNING;

    return 0;
}

int audio_reset(struct audio_device_handle *audio_dev_h)
{
    int result;

    log_debug("audio_dev_h(%p)\n", audio_dev_h);

    if (audio_dev_h->flags & AUDIO_DEVICE_FLAGS_RUNNING)
        return -1;

    if ((result = audio_device_reset(audio_dev_h->instance, audio_dev_h->dir)) != AUDIO_DEV_SUCCESS) {
        log_err("audio_device_reset() failed: error %d\n", result);
        return -1;
    }

    audio_dev_h->buffer_offset = 0;
    init_audio_dev_stats(audio_dev_h);

    return 0;
}

int audio_tx_exit(struct audio_device_handle *audio_dev_h)
{
    int result;

    log_debug("audio_dev_h(%p)\n", audio_dev_h);

    if ((result = audio_device_exit(audio_dev_h->instance, audio_dev_h->dir)) != AUDIO_DEV_SUCCESS) {
        log_err("audio_device_exit() failed: error %d\n", result);
        return -1;
    }

    if (audio_dev_h->tx_buffer) {
        vPortFree(audio_dev_h->tx_buffer);
        audio_dev_h->tx_buffer = NULL;
    }

    memset(&audio_dev_h->config, 0, sizeof(struct audio_device_config));
    memset(audio_dev_h, 0, sizeof(struct audio_device_handle));

    return 0;
}

int audio_rx_exit(struct audio_device_handle *audio_dev_h)
{
    int result;

    log_debug("audio_dev_h(%p)\n", audio_dev_h);

    if ((result = audio_device_exit(audio_dev_h->instance, audio_dev_h->dir)) != AUDIO_DEV_SUCCESS) {
        log_err("audio_device_exit() failed: error %d\n", result);
        return -1;
    }

    if (audio_dev_h->rx_buffer) {
        vPortFree(audio_dev_h->rx_buffer);
        audio_dev_h->rx_buffer = NULL;
    }

    memset(&audio_dev_h->config, 0, sizeof(struct audio_device_config));
    memset(audio_dev_h, 0, sizeof(struct audio_device_handle));

    return 0;
}
