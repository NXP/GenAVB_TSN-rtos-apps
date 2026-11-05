/*
 * Copyright 2018-2020, 2024-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "avb_tsn/common/system_config.h"
#include "codec.h"
#include "i2c.h"
#include "audio_device.h"

extern struct codec_instance codec_instances[CODEC_MAX_INSTANCES];

static struct codec_instance *__codec_get_instance(unsigned int instance)
{
    if (instance >= CODEC_MAX_INSTANCES)
        return NULL;

    return &codec_instances[instance];
}

int32_t codec_init(unsigned int instance)
{
    struct codec_instance *codec;
    int32_t (*control_interface_exit)(unsigned int instance) = NULL;
    int control_interface_instance = -1;

    if ((codec = __codec_get_instance(instance)) == NULL)
        goto err;

    if (codec->flags & CODEC_FLAGS_INITIALIZED)
        goto out_success;

    switch (codec->control_interface_type) {
    case CODEC_NO_CONTROL_INTERFACE:
        break;
    case CODEC_CONTROL_INTERFACE_I2C:
        if (i2c_init(codec->control_interface_instance))
            goto err;

        control_interface_instance = codec->control_interface_instance;
        control_interface_exit = &i2c_exit;
        break;
    default:
        goto err;
    }

    switch (codec->type) {
    case CODEC_TYPE_NO_DEV:
        break;
    case CODEC_TYPE_WM8960:
        WM8960_Init((wm8960_handle_t *)codec->handle, NULL);
        break;
    case CODEC_TYPE_WM8962:
        WM8962_Init((wm8962_handle_t *)codec->handle, (wm8962_config_t *)codec->config);
        break;
    default:
        goto err;
    }

    codec->flags |= CODEC_FLAGS_INITIALIZED;

out_success:
    codec->users++;

    return CODEC_SUCCESS;

err:
    if (control_interface_exit)
        control_interface_exit(control_interface_instance);

    return ERR_CODEC_INIT;
}

int32_t codec_exit(unsigned int instance)
{
    struct codec_instance *codec;

    if ((codec = __codec_get_instance(instance)) == NULL)
        goto err;

    if (codec->users > 1)
        goto out_success;

    /* Last user, really exit */

    if (codec->users < 1)
        goto err;

    if (codec_stop(instance))
        goto err;

    switch (codec->type) {
    case CODEC_TYPE_NO_DEV:
        break;
    case CODEC_TYPE_WM8960:
        WM8960_Deinit((wm8960_handle_t *)codec->handle);
        break;
    case CODEC_TYPE_WM8962:
        WM8962_Deinit((wm8962_handle_t *)codec->handle);
        break;
    default:
        goto err;
    }

    switch (codec->control_interface_type) {
    case CODEC_NO_CONTROL_INTERFACE:
        break;
    case CODEC_CONTROL_INTERFACE_I2C:
        if (i2c_exit(codec->control_interface_instance))
            goto err;
        break;
    default:
        goto err;
    }

    codec->flags &= ~(CODEC_FLAGS_INITIALIZED | CODEC_FLAGS_CONFIGURED);

out_success:
    codec->users--;

    return CODEC_SUCCESS;

err:
    return ERR_CODEC_EXIT;
}

int32_t codec_config(unsigned int instance, struct codec_transfer_config *config)
{
    struct codec_instance *codec;

    if ((codec = __codec_get_instance(instance)) == NULL)
        goto err;

    if (!(codec->flags & CODEC_FLAGS_INITIALIZED))
        goto err;

    if (codec->flags & CODEC_FLAGS_CONFIGURED) {
        if (memcmp(&codec->transfer_config, config, sizeof(struct codec_transfer_config)))
            goto err;
    } else {
        memcpy(&codec->transfer_config, config, sizeof(struct codec_transfer_config));
    }

    switch (codec->type) {
    case CODEC_TYPE_NO_DEV:
        break;
    case CODEC_TYPE_WM8960:
        if (WM8960_ConfigDataFormat((wm8960_handle_t *)codec->handle, config->masterClockHz, config->sampleRate_Hz, config->bitWidth) != kStatus_Success)
            goto err;
        break;
    case CODEC_TYPE_WM8962:
        if (WM8962_ConfigDataFormat((wm8962_handle_t *)codec->handle, config->masterClockHz, config->sampleRate_Hz, config->bitWidth) != kStatus_Success)
            goto err;
        break;
    default:
        goto err;
    }

    codec->flags |= CODEC_FLAGS_CONFIGURED;

    return CODEC_SUCCESS;

err:
    return ERR_CODEC_CONFIG;
}

int32_t codec_start(unsigned int instance)
{
    struct codec_instance *codec;

    if ((codec = __codec_get_instance(instance)) == NULL)
        goto err;

    if (!(codec->flags & CODEC_FLAGS_CONFIGURED))
        goto err;

    if (codec->flags & CODEC_FLAGS_STARTED)
        return CODEC_SUCCESS;

    codec->flags |= CODEC_FLAGS_STARTED;

    return CODEC_SUCCESS;

err:
    return ERR_CODEC_START;
}

int32_t codec_stop(unsigned int instance)
{
    struct codec_instance *codec;

    if ((codec = __codec_get_instance(instance)) == NULL)
        goto err;

    if (!(codec->flags & CODEC_FLAGS_STARTED))
        return CODEC_SUCCESS;

    // TODO disable internal clock?
    codec->flags &= ~CODEC_FLAGS_STARTED;

    return CODEC_SUCCESS;

err:
    return ERR_CODEC_STOP;
}

int32_t codec_send_control_data(unsigned int instance, const void *data, size_t size)
{
    struct codec_instance *codec;

    if ((codec = __codec_get_instance(instance)) == NULL)
        goto err;

    switch (codec->type) {
    case CODEC_TYPE_NO_DEV:
        break;
    case CODEC_TYPE_WM8960:
        if (codec->control_interface_type == CODEC_CONTROL_INTERFACE_I2C)
            return i2c_send(codec->control_interface_instance, WM8960_I2C_ADDR, data, size);
        break;
    case CODEC_TYPE_WM8962:
        if (codec->control_interface_type == CODEC_CONTROL_INTERFACE_I2C)
            return i2c_send(codec->control_interface_instance, WM8962_I2C_ADDR, data, size);
        break;
    default:
        goto err;
    }

    return CODEC_SUCCESS;

err:
    return ERR_CODEC_SEND_RECEIVE_DATA;
}

int32_t codec_receive_control_data(unsigned int instance, const void *reg, size_t reg_size, void *data, size_t size)
{
    struct codec_instance *codec;

    if ((codec = __codec_get_instance(instance)) == NULL)
        goto err;

    switch (codec->type) {
    case CODEC_TYPE_NO_DEV:
        break;
    case CODEC_TYPE_WM8960:
        break;
    case CODEC_TYPE_WM8962:
        if (codec->control_interface_type == CODEC_CONTROL_INTERFACE_I2C)
            return i2c_receive(codec->control_interface_instance, WM8962_I2C_ADDR, reg, reg_size, data, size);
        break;
    default:
        goto err;
    }

    return CODEC_SUCCESS;

err:
    return ERR_CODEC_SEND_RECEIVE_DATA;
}
