/*
 * Copyright 2018-2019, 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _AUDIO_DEVICE_H_
#define _AUDIO_DEVICE_H_

#include "fsl_common.h"
#include "media_stack/config.h"
#include "fsl_common.h"
#include "sai.h"
#include "i2c.h"
#include "dma.h"
#include "codec.h"

#define ERR_CODE(module, code)     (-(((module)*100) + (code)))
#define SAI_ERR_GROUP_NUMBER       1
#define I2C_ERR_GROUP_NUMBER       2
#define DMA_ERR_GROUP_NUMBER       3
#define CODEC_ERR_GROUP_NUMBER     4
#define AUDIO_DEV_ERR_GROUP_NUMBER 5

#define AUDIO_DEV_ERR(code) ERR_CODE(AUDIO_DEV_ERR_GROUP_NUMBER, code)

#define AUDIO_DEV_SUCCESS                       (0)
#define AUDIO_DEV_FAIL                          AUDIO_DEV_ERR(1)
#define ERR_AUDIO_DEV_INTERFACE_NOT_SUPPORTED   AUDIO_DEV_ERR(2)
#define ERR_AUDIO_DEV_CHANNELS_NOT_SUPPORTED    AUDIO_DEV_ERR(3)
#define ERR_AUDIO_DEV_BITWIDTH_NOT_SUPPORTED    AUDIO_DEV_ERR(4)
#define ERR_AUDIO_DEV_SAMPLE_RATE_NOT_SUPPORTED AUDIO_DEV_ERR(5)
#define ERR_AUDIO_DEV_CB_PERIOD_NOT_SUPPORTED   AUDIO_DEV_ERR(6)
#define ERR_AUDIO_DEV_INVALID_PARAMS            AUDIO_DEV_ERR(7)
#define ERR_AUDIO_DEV_DIRECTION                 AUDIO_DEV_ERR(8)

#define AUDIO_INTERFACE_TYPE_SAI 1

#define AUDIO_DEVICE_FLAGS_INITIALIZED (1 << 0)
#define AUDIO_DEVICE_FLAGS_CONFIGURED  (1 << 1)
#define AUDIO_DEVICE_FLAGS_STARTED     (1 << 2)

struct cb_data {
    void (*cb)(uint32_t status, void *data);
    void *data;
};

struct audio_device_instance {
    enum audio_device_direction direction;
    unsigned int audio_interface_type;
    union {
        struct {
            unsigned int instance;
        } sai;
    } audio_interface;

    struct {
        unsigned int instance;
        struct {
            int number;
        } channels[2]; /* tx and rx */
    } dma;

    struct {
        unsigned int instance;
    } codec;

    struct cb_data cb_data;

    uint8_t flags;
};

struct transfer_config {
    struct audio_device_config *audio_device;
    union {
        struct sai_transfer_format sai;
    } audio_interface;
    struct dma_transfer_config dma[2];
    struct codec_transfer_config codec;
};

struct audio_device_config {
    struct {
        uint32_t sampleRate_Hz;
        uint8_t bitWidth;
        uint8_t channels;
    } audio_interface;

    uint32_t buffer_size;                          /* size in bytes of transfer buffers */
    void *buffer_addr;                             /* address of transmit/receive buffer */
    uint32_t callback_size;                        /* number of bytes per callback call */
    void (*callback)(uint32_t status, void *data); /* callback function called each callback_period bytes */
    void *callback_data;
};

/**
 * Initialize all audio device peripherals and clocks
 * Call this function only once, or after audio_device_exit
 * @param instance: audio device number
 */
int audio_device_init(unsigned int instance, enum audio_device_direction dir);

/**
 * Disables audio device transfer and deinitialize all audio device peripherals
 * @param instance: audio device number
 */
int audio_device_exit(unsigned int instance, enum audio_device_direction dir);

/**
 * Configures transfer format and audio data source/desination
 * Call after audio_device_init
 *
 * @param instance: audio device number
 * @param config: transfer configuration
 */
int audio_device_config(unsigned int instance, struct audio_device_config *config, enum audio_device_direction dir);

/**
 * Enables audio device transfer
 * Call only after audio_device_config
 * @param instance: audio device number
 */
int audio_device_start(unsigned int instance, enum audio_device_direction dir);

/**
 * Disables audio device transfer
 * @param instance: audio device number
 */
int audio_device_stop(unsigned int instance, enum audio_device_direction dir);

/**
 * Resets audio device to initial value keeping the configuration
 */
int audio_device_reset(unsigned int instance, enum audio_device_direction dir);

/**
 *
 */
int32_t audio_device_get_remaining_buffer_bytes(unsigned int instance, enum audio_device_direction dir);

#endif /* _AUDIO_DEVICE_H_ */
