/*
 * Copyright 2018-2019, 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "i2c.h"
#include "board.h"
#include "audio_device.h"

extern struct i2c_instance i2c_instances[I2C_MAX_INSTANCES];

static struct i2c_instance *__i2c_get_instance(unsigned int instance)
{
    if (instance >= I2C_MAX_INSTANCES)
        return NULL;

    return &i2c_instances[instance];
}

int32_t i2c_init(unsigned int instance)
{
    struct i2c_instance *i2c;
    lpi2c_master_config_t i2cConfig = {0};

    if ((i2c = __i2c_get_instance(instance)) == NULL)
        goto err;

    if (i2c->flags & I2C_FLAGS_INITIALIZED)
        return I2C_SUCCESS;

    LPI2C_MasterGetDefaultConfig(&i2cConfig);
    LPI2C_MasterInit(i2c->base, &i2cConfig, BOARD_CODEC_I2C_CLOCK_FREQ);
    LPI2C_MasterTransferCreateHandle(i2c->base, &i2c->handle, NULL, NULL);

    i2c->flags |= I2C_FLAGS_INITIALIZED;

    return I2C_SUCCESS;

err:
    return ERR_I2C_INIT;
}

int32_t i2c_exit(unsigned int instance)
{
    struct i2c_instance *i2c;
    if ((i2c = __i2c_get_instance(instance)) == NULL)
        goto err;

    if (!(i2c->flags & I2C_FLAGS_INITIALIZED))
        return I2C_SUCCESS;

    LPI2C_MasterDeinit(i2c->base);

    i2c->flags &= ~I2C_FLAGS_INITIALIZED;

    return I2C_SUCCESS;

err:
    return ERR_I2C_EXIT;
}

int32_t i2c_send(unsigned int instance, uint8_t address, const uint8_t *data, size_t size)
{
    struct i2c_instance *i2c;
    if ((i2c = __i2c_get_instance(instance)) == NULL)
        goto err;

    if (!(i2c->flags & I2C_FLAGS_INITIALIZED))
        goto err;

    /* TODO: replace by non-blocking functions */
    if (LPI2C_MasterStart(i2c->base, address, kLPI2C_Write) != kStatus_Success)
        goto err_transfer;
    if (LPI2C_MasterSend(i2c->base, (void *)data, size) != kStatus_Success)
        goto err_transfer;
    if (LPI2C_MasterStop(i2c->base) != kStatus_Success)
        goto err_transfer;

    return I2C_SUCCESS;

err_transfer:
    /* get and clear status flags */
    i2c->status = LPI2C_MasterGetStatusFlags(i2c->base);
    LPI2C_MasterClearStatusFlags(i2c->base, i2c->status);
    /* reset all internal logic and registers except the Master Control Register */
    LPI2C_MasterReset(i2c->base);
err:
    return ERR_I2C_SEND;
}

int32_t i2c_receive(unsigned int instance, uint8_t address, const uint8_t *reg, size_t reg_size, uint8_t *data, size_t size)
{
    struct i2c_instance *i2c;
    if ((i2c = __i2c_get_instance(instance)) == NULL)
        goto err;

    if (!(i2c->flags & I2C_FLAGS_INITIALIZED))
        goto err;

    if (LPI2C_MasterStart(i2c->base, address, kLPI2C_Write) != kStatus_Success)
        goto err_transfer;
    if (LPI2C_MasterSend(i2c->base, (void *)reg, reg_size) != kStatus_Success)
        goto err_transfer;
    /* Need to send repeated start for read operation*/
    if (LPI2C_MasterStart(i2c->base, address, kLPI2C_Read) != kStatus_Success)
        goto err_transfer;
    if (LPI2C_MasterReceive(i2c->base, data, size) != kStatus_Success)
        goto err_transfer;
    if (LPI2C_MasterStop(i2c->base) != kStatus_Success)
        goto err_transfer;

    return I2C_SUCCESS;

err_transfer:
    /* get and clear status flags */
    i2c->status = LPI2C_MasterGetStatusFlags(i2c->base);
    LPI2C_MasterClearStatusFlags(i2c->base, i2c->status);
    /* reset all internal logic and registers except the Master Control Register */
    LPI2C_MasterReset(i2c->base);
err:
    return ERR_I2C_RECEIVE;
}

uint32_t i2c_get_status(unsigned int instance)
{
    struct i2c_instance *i2c;
    if ((i2c = __i2c_get_instance(instance)) == NULL)
        goto err;

    return i2c->status;

err:
    return ERR_I2C_GET_STATUS;
}
