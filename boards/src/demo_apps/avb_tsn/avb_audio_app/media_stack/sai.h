/*
 * Copyright 2018-2019 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _SAI_H_
#define _SAI_H_

#include "fsl_sai.h"
#include "common.h"

#define SAI_ERR(code) ERR_CODE(SAI_ERR_GROUP_NUMBER, code)

#define SAI_SUCCESS        (0)
#define ERR_SAI_INIT       SAI_ERR(1)
#define ERR_SAI_EXIT       SAI_ERR(2)
#define ERR_SAI_CONFIGURE  SAI_ERR(3)
#define ERR_SAI_START      SAI_ERR(4)
#define ERR_SAI_STOP       SAI_ERR(5)
#define ERR_SAI_GET_CONFIG SAI_ERR(6)
#define ERR_SAI_RESET      SAI_ERR(7)

#define SAI_FLAGS_INITIALIZED    (1 << 0)
#define SAI_TX_FLAGS_INITIALIZED (1 << 1)
#define SAI_RX_FLAGS_INITIALIZED (1 << 2)
#define SAI_TX_FLAGS_CONFIGURED  (1 << 3)
#define SAI_RX_FLAGS_CONFIGURED  (1 << 4)
#define SAI_TX_FLAGS_STARTED     (1 << 5)
#define SAI_RX_FLAGS_STARTED     (1 << 6)

#define SAI_TXRX_DEPENDENT   (1 << 0)
#define SAI_TXRX_INDEPENDENT (1 << 1)

struct sai_transfer_format {
    uint32_t sampleRate_Hz; /*!< Sample rate of audio data */
    uint32_t bitWidth;      /*!< Data length of audio data, usually 8/16/24/32 bits */
    uint32_t channels;      /*!< Mono or stereo */
};

struct sai_clock_config {
    uint32_t masterClockHz;
};

struct sai_config {
    sai_handle_t handle;
    uint32_t channel;
    dma_request_source_t dma_request_source;
};

struct sai_instance {
    I2S_Type *base;
    sai_config_t config;
    uint8_t watermark;
    uint32_t frame_width;
    struct sai_config transfer_config[2];
    struct sai_transfer_format transfer_format;
    struct sai_clock_config clock_config;
    uint8_t flags;
    uint8_t mode;
};

/**
 * Initialize SAI interface
 * @param instance: SAI peripheral instance
 * @param transfer_type: transfer direction
 */
int32_t sai_init(unsigned int instance, enum audio_device_direction dir);

/**
 * Deinitialize SAI interface
 * @param instance: SAI peripheral instance
 * @param transfer_type: transfer direction
 */
int32_t sai_exit(unsigned int instance, enum audio_device_direction dir);

/**
 * Configure SAI transfer: sample rate, bit width etc.
 * @param instance: SAI peripheral instance
 * @param transfer_format: configuration structure containing all parameters
 * @param transfer_type: transfer direction
 */
int32_t sai_configure(unsigned int instance, enum audio_device_direction dir, struct sai_transfer_format *transfer_format);

/**
 * Enable SAI transfer
 * @param instance: SAI peripheral instance
 * @param transfer_type: transfer direction
 */
int32_t sai_start(unsigned int instance, enum audio_device_direction dir);

/**
 * Disable SAI transfer
 * @param instance: SAI peripheral instance
 * @param transfer_type: transfer direction
 */
int32_t sai_stop(unsigned int instance, enum audio_device_direction dir);

/**
 * Resets internal logic of sai and flush FIFO keeping the configuration
 */
int32_t sai_reset(unsigned int instance, enum audio_device_direction dir);

/**
 * Get internal configuration parameters
 * this function fill config struct
 * @param instance: SAI peripheral instance
 * @param config: pointer to config struct
 */
int32_t sai_get_config(unsigned int instance, struct audio_interface_config *config);

#endif /* _SAI_H_ */
