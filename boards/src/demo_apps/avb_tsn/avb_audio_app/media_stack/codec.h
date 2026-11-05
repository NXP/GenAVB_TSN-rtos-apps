/*
 * Copyright 2018-2019, 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _CODEC_H_
#define _CODEC_H_

#include "fsl_common.h"
#include "fsl_wm8960.h"
#include "fsl_wm8962.h"

#define CODEC_ERR(code) ERR_CODE(CODEC_ERR_GROUP_NUMBER, code)

#define CODEC_SUCCESS               (0)
#define ERR_CODEC_INIT              CODEC_ERR(1)
#define ERR_CODEC_EXIT              CODEC_ERR(2)
#define ERR_CODEC_START             CODEC_ERR(3)
#define ERR_CODEC_STOP              CODEC_ERR(4)
#define ERR_CODEC_CONFIG            CODEC_ERR(5)
#define ERR_CODEC_SEND_RECEIVE_DATA CODEC_ERR(6)

// No device codec, used to simulate audio interface
#define CODEC_TYPE_NO_DEV 0
#define CODEC_TYPE_WM8960 1
#define CODEC_TYPE_WM8962 2

#define CODEC_NO_CONTROL_INTERFACE  0
#define CODEC_CONTROL_INTERFACE_I2C 1

/* codec flags */
#define CODEC_FLAGS_INITIALIZED (1 << 0)
#define CODEC_FLAGS_CONFIGURED  (1 << 1)
#define CODEC_FLAGS_STARTED     (1 << 2)

struct codec_transfer_config {
    uint32_t masterClockHz;
    uint32_t sampleRate_Hz;
    uint8_t bitWidth;
    uint8_t protocol;
};

struct codec_instance {
    uint8_t type;
    uint8_t control_interface_type;
    unsigned int control_interface_instance;
    struct codec_transfer_config transfer_config;
    uint8_t flags;
    uint8_t users;
    void *handle;
    void *config;
};

/**
 * Initialize code
 * @param instance: codec number
 */
int32_t codec_init(unsigned int instance);

/**
 * Deinitialize code
 * @param instance: codec number
 */
int32_t codec_exit(unsigned int instance);

/**
 * Configure data format and protocol
 * @param instance: codec number
 * @param config: transfer format
 */
int32_t codec_config(unsigned int instance, struct codec_transfer_config *config);

/**
 * Start transfer
 * @param instance: codec number
 */
int32_t codec_start(unsigned int instance);

/**
 * Stop transfer
 * @param instance: codec number
 */
int32_t codec_stop(unsigned int instance);

/**
 * Send control data using 2-wire I2C protocol
 * @param instance: codec number
 * @param data: data to send
 * @param size: amount of data
 */
int32_t codec_send_control_data(unsigned int instance, const void *data, size_t size);

/**
 * Receive control data using 2-wire I2C protocol
 * @param instance: codec number
 * @param reg: register address to read data
 * @param reg_size: size of register address
 * @param data: data to receive
 * @param size: amount of data
 */
int32_t codec_receive_control_data(unsigned int instance, const void *reg, size_t reg_size, void *data, size_t size);

#endif /* _CODEC_H_ */
