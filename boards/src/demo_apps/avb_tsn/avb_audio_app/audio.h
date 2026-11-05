/*
 * Copyright 2018-2020, 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _AUDIO_H_
#define _AUDIO_H_

#include "media_stack/audio_device.h"
#include "avb_stream.h"
#include "genavb/genavb.h"

#define AUDIO_DEVICE_FLAGS_RUNNING (1 << 0)
#define AUDIO_DEVICE_FIRST_READ    (1 << 1)

#define bswap_32(x)                                       \
    ((((x)&0xff000000) >> 24) | (((x)&0x00ff0000) >> 8) | \
     (((x)&0x0000ff00) << 8) | (((x)&0x000000ff) << 24))

#define bswap_16(x) \
    ((((x)&0xff00) >> 8) | (((x)&0x00ff) << 8))

/**
 * Desired latency for audio. Based on this value, the starting sequence calculates
 * how many silence frames to put before received audio signal.
 */
#define CFG_AUDIO_PLAYBACK_LATENCY_NS 2000000
/**
 * Minimum number of silence frames to add in a single go when starting a stream
 */
#define CFG_AUDIO_MIN_SILENCE_FRAMES 8

/**
 * PCM_START_DELAY_SAMPLES:
 * As the audio path can differ for different endpoints, the time between putting
 * samples into the audio ring buffer, and output them as a analog audio signal also
 * can differ. Starting sequence can artificially modify this time, by adjusting
 * audio playback latency.
 * This mechanism can be used to align/synchronise audio between different listener
 * endpoints.
 *
 * In this case the value of 7 samples was choosen experimentally to align with
 * MCIMX6 endpoint
 */
#define PCM_START_DELAY_SAMPLES 7

#define START_COPY_SIZE    1024
#define MAX_EVENTS         12
#define MAX_TOTALS         12
#define MAX_DROPPED_PERIOD 4

struct audio_device_stats {
    // number of media stack cb calls
    unsigned long long media_stack_cb_count;
    // total number of consumed/produced bytes by media stack
    unsigned long long media_stack_treated_bytes;
    // total number of sent/received bytes to/from gen_avb stack
    unsigned long long avb_stack_treated_bytes;
    // difference between received bytes and media stack consumed bytes
    long long int current_latency_bytes;

    // number of bytes readed by media stack between two last avb stack calls
    unsigned int media_stack_last_buffer_position;
    // total number of treated bytes at last media stack cb call
    unsigned long long avb_stack_treated_bytes_stamp;
    //

    unsigned int tx_err; // write error counter
    unsigned int rx_err; // read error counter
    unsigned int tx_avail_err;
    unsigned int tx_invalid_timestamp;
    // tx start sequence
    unsigned int tx_start;
    unsigned int tx_start_err;
    unsigned int tx_start_drop;
    unsigned int tx_start_no_data;
};

struct audio_device_handle {
    unsigned int instance;
    enum audio_device_direction dir;
    uint8_t flags;
    struct audio_device_config config;
    uint8_t *tx_buffer;
    uint8_t *rx_buffer;
    unsigned int period_size_samples;
    unsigned int batch_size_bytes;
    unsigned int buffer_offset;
    unsigned int frame_size;
    unsigned int rate;
    unsigned int frame_duration;
    uint32_t start_time;
    void (*process_samples)(struct audio_device_handle *audio_dev_h, void *src_frame, unsigned int samples_to_commit);

    int (*cb)(void *data);
    void *cb_data;

    struct audio_device_stats stats;
};

int audio_init(struct audio_device_handle *handle, struct genavb_stream_params *stream_params, int (*cb)(void *data), void *cb_data);

int audio_tx(struct audio_device_handle *audio_dev_h, struct avb_stream *avb_stream);
int audio_rx(struct audio_device_handle *audio_dev_h, struct avb_stream *avb_stream);

int audio_tx_start(struct audio_device_handle *audio_dev_h, struct avb_stream *avb_stream);
int audio_rx_start(struct audio_device_handle *audio_dev_h, struct avb_stream *avb_stream);

int audio_stop(struct audio_device_handle *audio_dev_h);
int audio_reset(struct audio_device_handle *audio_dev_h);

int audio_tx_exit(struct audio_device_handle *audio_dev_h);
int audio_rx_exit(struct audio_device_handle *audio_dev_h);

#endif /* _AUDIO_H_ */
