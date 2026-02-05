/*
 * Copyright 2018-2020 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _AVB_STREAM_CONFIG_H_
#define _AVB_STREAM_CONFIG_H_

#include "genavb/genavb.h"

#define MAX_AVB_LISTENER_STREAMS 4
#define MAX_AVB_TALKER_STREAMS   4

#define DEFAULT_BATCH_SIZE_NS 1000000 /* 1ms */

struct avb_stream_stats {
    unsigned int rx_err;             /**< AVB receive error counter */
    unsigned int tx_err;             /**< AVB send error counter */
    unsigned int batch_rx;           /**< AVB batches received counter */
    unsigned int batch_tx;           /**< AVB batches send counter */
    unsigned int invalid_timestamp;  /**< AVB invalid timestamp on received stream */
    unsigned int gptp_2cont_wakeup;  /**< gPTP time delta since last wakeup */
    unsigned int event_2cont_wakeup; /**< AVB event time delta since last wakeup (1st event) */
    unsigned int event_gptp;         /**< time delta between event time vs current gPTP time */
};

struct avb_stream {
    struct genavb_stream_params stream_params;
    struct genavb_stream_handle *stream_handle;
    unsigned int batch_size_ns;
    unsigned int cur_batch_size;

    unsigned int last_event_ts; /**< Event timestamp of last wakeup */
    unsigned int last_event_frame_offset;
    unsigned int last_exchanged_frames; /**< Number of frames exchanged in the last batch */
    unsigned int last_gptp_time;        /**< gPTP time of last wakeup */
    char is_first_wakeup;

    struct avb_stream_stats stats;
};

extern struct avb_stream avb_listener_streams[MAX_AVB_LISTENER_STREAMS];
extern struct avb_stream avb_talker_streams[MAX_AVB_TALKER_STREAMS];

#endif /* _AVB_STREAM_H_ */
