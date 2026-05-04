/*
 * Copyright 2019-2020 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _MEDIA_STACK_COMMON_H_
#define _MEDIA_STACK_COMMON_H_

#define TX 0
#define RX 1

enum audio_device_direction {
    AUDIO_DEV_DIR_PLAYBACK = TX,
    AUDIO_DEV_DIR_CAPTURE = RX
};

struct audio_interface_config {
    void *data_output_addr;
    void *data_input_addr;
    uint32_t masterClockHz;
    uint8_t protocol;
    uint32_t dma_request_source[2];
    uint32_t dma_request_size;
};

#endif /* _MEDIA_STACK_COMMON_H_ */
