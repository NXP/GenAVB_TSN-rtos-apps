/*
 * Copyright 2018-2019, 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _I2C_H_
#define _I2C_H_

#include "fsl_lpi2c.h"

#define I2C_ERR(code) ERR_CODE(I2C_ERR_GROUP_NUMBER, code)

#define I2C_SUCCESS        (0)
#define ERR_I2C_INIT       I2C_ERR(1)
#define ERR_I2C_EXIT       I2C_ERR(2)
#define ERR_I2C_SEND       I2C_ERR(3)
#define ERR_I2C_RECEIVE    I2C_ERR(4)
#define ERR_I2C_GET_STATUS I2C_ERR(5)

#define I2C_FLAGS_INITIALIZED (1 << 0)

struct i2c_instance {
    LPI2C_Type *base;
    lpi2c_master_handle_t handle;
    uint8_t flags;
    uint32_t status;
};

/**
 * Initlialize i2c instance
 * @param instance: i2c instance
 */
int32_t i2c_init(unsigned int instance);

/**
 * Deinitialize i2c instance
 * @param instance: i2c instance
 */
int32_t i2c_exit(unsigned int instance);

/**
 * Send data over i2c
 * @param instance: i2c instance
 * @param address: i2c device address on the bus
 * @param data: data to send
 * @param size: amount of data
 */
int32_t i2c_send(unsigned int instance, uint8_t address, const uint8_t *data, size_t size);

/**
 * Receive data over i2c
 * @param instance: i2c instance
 * @param address: i2c device address on the bus
 * @param reg: register address to read data
 * @param reg_size: size of register address
 * @param data: where to store data
 * @param size: amount of data
 */
int32_t i2c_receive(unsigned int instance, uint8_t address, const uint8_t *reg, size_t reg_size, uint8_t *data, size_t size);

/**
 * Get status of i2c instance, call to investigate error
 * @param instance: i2c instance
 * @see _lpi2c_master_flags
 */
uint32_t i2c_get_status(unsigned int instance);

#endif
