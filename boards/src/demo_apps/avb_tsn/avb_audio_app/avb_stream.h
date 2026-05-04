/*
 * Copyright 2018-2019 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _AVB_STREAM_H_
#define _AVB_STREAM_H_

#include "avb_stream_config.h"

int genavbstream_listener_add(unsigned int unique_id, struct genavb_stream_params *params, struct avb_stream **stream);
int genavbstream_talker_add(unsigned int unique_id, struct genavb_stream_params *params, struct avb_stream **stream);

int genavbstream_listener_remove(unsigned int unique_id);
int genavbstream_talker_remove(unsigned int unique_id);

unsigned int genavbstream_batch_size(unsigned int batch_size_ns, struct avb_stream_params *params);

#endif /* _AVB_STREAM_H_ */
