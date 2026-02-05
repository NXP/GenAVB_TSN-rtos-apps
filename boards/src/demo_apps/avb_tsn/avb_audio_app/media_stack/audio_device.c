/*
 * Copyright 2018-2020, 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "audio_device.h"

extern struct audio_device_instance audio_capture_instances[AUDIO_DEVICE_MAX_CAPTURE_INSTANCES];
extern struct audio_device_instance audio_playback_instances[AUDIO_DEVICE_MAX_PLAYBACK_INSTANCES];

static void __audio_devices_edma_cb(edma_handle_t *handle, void *userData, bool done, uint32_t tcds)
{
    struct cb_data *cb_data = (struct cb_data *)userData;
    uint32_t status = EDMA_GetChannelStatusFlags(handle->base, handle->channel);

    EDMA_ClearChannelStatusFlags(handle->base, handle->channel, status);

    if (cb_data)
        cb_data->cb(status, cb_data->data);
}

static inline struct audio_device_instance *__audio_device_get_instance(unsigned int instance, enum audio_device_direction dir)
{
    switch (dir) {
    case AUDIO_DEV_DIR_CAPTURE:
        if (instance >= AUDIO_DEVICE_MAX_CAPTURE_INSTANCES)
            return NULL;

        return &audio_capture_instances[instance];

        break;
    case AUDIO_DEV_DIR_PLAYBACK:
        if (instance >= AUDIO_DEVICE_MAX_PLAYBACK_INSTANCES)
            return NULL;

        return &audio_playback_instances[instance];

        break;
    default:
        return NULL;
    }
}

int audio_device_init(unsigned int instance, enum audio_device_direction dir)
{
    struct audio_device_instance *audio_device = NULL;
    int32_t (*audio_interface_exit)(unsigned int instance, enum audio_device_direction dir) = NULL;
    int audio_interface_instance = -1;
    int32_t errno = AUDIO_DEV_FAIL;

    if ((audio_device = __audio_device_get_instance(instance, dir)) == NULL)
        goto err;

    if (audio_device->flags & AUDIO_DEVICE_FLAGS_INITIALIZED)
        goto out;

    switch (audio_device->audio_interface_type) {
    case AUDIO_INTERFACE_TYPE_SAI:
        audio_interface_exit = &sai_exit;
        audio_interface_instance = audio_device->audio_interface.sai.instance;

        if ((errno = sai_init(audio_interface_instance, dir)) < 0)
            goto err;
        break;
    default:
        errno = ERR_AUDIO_DEV_INTERFACE_NOT_SUPPORTED;
        goto err;
    }

    if ((errno = dma_init(audio_device->dma.instance)) < 0)
        goto err_dma;

    if ((errno = codec_init(audio_device->codec.instance)) < 0)
        goto err_codec;

    audio_device->flags |= AUDIO_DEVICE_FLAGS_INITIALIZED;

out:
    return AUDIO_DEV_SUCCESS;

err_codec:
    dma_exit(audio_device->dma.instance);
err_dma:
    audio_interface_exit(audio_interface_instance, dir);
err:
    return errno;
}

int audio_device_exit(unsigned int instance, enum audio_device_direction dir)
{
    struct audio_device_instance *audio_device;
    int32_t errno = AUDIO_DEV_FAIL;

    if ((audio_device = __audio_device_get_instance(instance, dir)) == NULL)
        goto err;

    if (!(audio_device->flags & AUDIO_DEVICE_FLAGS_INITIALIZED))
        return AUDIO_DEV_SUCCESS;

    switch (audio_device->audio_interface_type) {
    case AUDIO_INTERFACE_TYPE_SAI:
        if ((errno = sai_exit(audio_device->audio_interface.sai.instance, dir)) < 0)
            goto err;
        break;
    default:
        errno = ERR_AUDIO_DEV_INTERFACE_NOT_SUPPORTED;
        goto err;
    }

    if (audio_device->direction == AUDIO_DEV_DIR_PLAYBACK) {
        if ((errno = dma_exit_channel(audio_device->dma.instance, audio_device->dma.channels[TX].number)) < 0)
            goto err;
    } else if (audio_device->direction == AUDIO_DEV_DIR_CAPTURE) {
        if ((errno = dma_exit_channel(audio_device->dma.instance, audio_device->dma.channels[RX].number)) < 0)
            goto err;
    } else {
        errno = ERR_AUDIO_DEV_DIRECTION;
        goto err;
    }

    if ((errno = codec_exit(audio_device->codec.instance)) < 0)
        goto err;

    audio_device->flags = 0;

    return AUDIO_DEV_SUCCESS;

err:
    return errno;
}

static int32_t __prepare_transfer_config(struct audio_device_instance *audio_device, struct transfer_config *config)
{
    int32_t errno = AUDIO_DEV_FAIL;
    struct audio_interface_config audio_interface_config = {0};
    uint8_t bitWidth = 0, sample_size_bytes = 0;
    uint32_t sampleRate_Hz = 0;
    uint8_t channels = 0;

    /* check all parameters */
    /* bit width */
    switch (config->audio_device->audio_interface.bitWidth) {
    case 16:
        bitWidth = 16;
        sample_size_bytes = 2;
        break;
    case 24:
        bitWidth = 24;
        sample_size_bytes = 4;
        break;
    case 32:
        bitWidth = 32;
        sample_size_bytes = 4;
        break;
    default:
        errno = ERR_AUDIO_DEV_BITWIDTH_NOT_SUPPORTED;
        goto err;
    }

    /* sample rate */
    switch (config->audio_device->audio_interface.sampleRate_Hz) {
    case 8000:
    case 24000:
    case 32000:
    case 48000:
    case 96000:
        sampleRate_Hz = config->audio_device->audio_interface.sampleRate_Hz;
        break;
    default:
        errno = ERR_AUDIO_DEV_SAMPLE_RATE_NOT_SUPPORTED;
        goto err;
    }

    /* channels */
    switch (config->audio_device->audio_interface.channels) {
    case 1:
    case 2:
        channels = config->audio_device->audio_interface.channels;
        break;
    default:
        errno = ERR_AUDIO_DEV_CHANNELS_NOT_SUPPORTED;
        goto err;
    }

    if (!config->audio_device->buffer_addr) {
        errno = ERR_AUDIO_DEV_INVALID_PARAMS;
        goto err;
    }

    /* audio interface */
    switch (audio_device->audio_interface_type) {
    case AUDIO_INTERFACE_TYPE_SAI:
        config->audio_interface.sai.bitWidth = bitWidth;
        config->audio_interface.sai.sampleRate_Hz = sampleRate_Hz;
        config->audio_interface.sai.channels = channels;

        if ((errno = sai_get_config(audio_device->audio_interface.sai.instance, &audio_interface_config)) < 0)
            goto err;

        break;
    default:
        errno = ERR_AUDIO_DEV_INTERFACE_NOT_SUPPORTED;
        goto err;
    }

    /* dma tx and rx channels */
    for (int tt = TX; tt <= RX; tt++) {
        uint32_t size = config->audio_device->buffer_size;
        uint32_t callback_period = config->audio_device->callback_size;

        /* if user gives his callback we pass it as data to edma callback and we call it from there */
        if (config->audio_device->callback) {
            if ((callback_period != (size / 2)) && (callback_period != size)) {
                errno = ERR_AUDIO_DEV_CB_PERIOD_NOT_SUPPORTED;
                goto err;
            }
            audio_device->cb_data.cb = config->audio_device->callback;
            audio_device->cb_data.data = config->audio_device->callback_data;

            config->dma[tt].callback = &__audio_devices_edma_cb;
            config->dma[tt].callback_data = (void *)&audio_device->cb_data;
        }

        config->dma[tt].transferBytes = size;
        config->dma[tt].circular = true;
        config->dma[tt].destWidth = sample_size_bytes;
        config->dma[tt].srcWidth = sample_size_bytes;
        config->dma[tt].callback_period = callback_period;
        config->dma[tt].bytesEachRequest = audio_interface_config.dma_request_size;

        if (tt == TX) {
            config->dma[tt].srcAddr = config->audio_device->buffer_addr;
            config->dma[tt].destAddr = audio_interface_config.data_output_addr;
            config->dma[tt].request_source = audio_interface_config.dma_request_source[TX];
            config->dma[tt].type = kEDMA_MemoryToPeripheral;
        } else /* RX */ {
            config->dma[tt].srcAddr = audio_interface_config.data_input_addr;
            config->dma[tt].destAddr = config->audio_device->buffer_addr;
            config->dma[tt].request_source = audio_interface_config.dma_request_source[RX];
            config->dma[tt].type = kEDMA_PeripheralToMemory;
        }
    }

    /* codec */
    config->codec.masterClockHz = audio_interface_config.masterClockHz;
    config->codec.protocol = audio_interface_config.protocol;
    config->codec.bitWidth = bitWidth;
    config->codec.sampleRate_Hz = sampleRate_Hz;

    return AUDIO_DEV_SUCCESS;
err:
    return errno;
}

int audio_device_config(unsigned int instance, struct audio_device_config *audio_device_config, enum audio_device_direction dir)
{
    struct audio_device_instance *audio_device;
    struct transfer_config config = {0};
    int32_t errno = AUDIO_DEV_FAIL;

    if ((audio_device = __audio_device_get_instance(instance, dir)) == NULL)
        goto err;

    if (!(audio_device->flags & AUDIO_DEVICE_FLAGS_INITIALIZED))
        goto err;

    if (audio_device->flags & AUDIO_DEVICE_FLAGS_STARTED)
        goto err;

    config.audio_device = audio_device_config;

    if ((errno = __prepare_transfer_config(audio_device, &config)) < 0)
        goto err;

    /* audio interface */
    switch (audio_device->audio_interface_type) {
    case AUDIO_INTERFACE_TYPE_SAI:
        if ((errno = sai_configure(audio_device->audio_interface.sai.instance, dir, &config.audio_interface.sai)) < 0)
            goto err;
        break;
    default:
        errno = ERR_AUDIO_DEV_INTERFACE_NOT_SUPPORTED;
        goto err;
    }

    /* dma */
    if (audio_device->direction == AUDIO_DEV_DIR_PLAYBACK) {
        uint32_t channel_number = audio_device->dma.channels[TX].number;
        if ((errno = dma_configure(audio_device->dma.instance, channel_number, &config.dma[TX])) < 0)
            goto err;
    } else if (audio_device->direction == AUDIO_DEV_DIR_CAPTURE) {
        uint32_t channel_number = audio_device->dma.channels[RX].number;
        if ((errno = dma_configure(audio_device->dma.instance, channel_number, &config.dma[RX])) < 0)
            goto err;
    } else {
        errno = ERR_AUDIO_DEV_DIRECTION;
        goto err;
    }

    /* codec */
    if ((errno = codec_config(audio_device->codec.instance, &config.codec)) < 0)
        goto err;

    audio_device->flags |= AUDIO_DEVICE_FLAGS_CONFIGURED;

    return AUDIO_DEV_SUCCESS;

    // TODO: reset functions to undo in case of error
err:
    return errno;
}

int audio_device_start(unsigned int instance, enum audio_device_direction dir)
{
    struct audio_device_instance *audio_device;
    int audio_interface_instance = -1;
    int32_t (*audio_interface_stop)(unsigned int instance, enum audio_device_direction dir) = NULL;
    int32_t errno = AUDIO_DEV_FAIL;

    if ((audio_device = __audio_device_get_instance(instance, dir)) == NULL)
        goto err;

    if (!(audio_device->flags & AUDIO_DEVICE_FLAGS_CONFIGURED))
        goto err;

    if (audio_device->flags & AUDIO_DEVICE_FLAGS_STARTED)
        return AUDIO_DEV_SUCCESS;

    switch (audio_device->audio_interface_type) {
    case AUDIO_INTERFACE_TYPE_SAI:
        audio_interface_instance = audio_device->audio_interface.sai.instance;
        audio_interface_stop = &sai_stop;
        if ((errno = sai_start(audio_interface_instance, dir)) < 0)
            goto err;
        break;
    default:
        errno = ERR_AUDIO_DEV_INTERFACE_NOT_SUPPORTED;
        goto err;
    }

    if (audio_device->direction == AUDIO_DEV_DIR_PLAYBACK) {
        if (dma_start(audio_device->dma.instance, audio_device->dma.channels[TX].number))
            goto err_dma;
    } else if (audio_device->direction == AUDIO_DEV_DIR_CAPTURE) {
        if (dma_start(audio_device->dma.instance, audio_device->dma.channels[RX].number))
            goto err_dma;
    } else {
        errno = ERR_AUDIO_DEV_DIRECTION;
        goto err;
    }

    if ((errno = codec_start(audio_device->codec.instance)) < 0)
        goto err_codec;

    audio_device->flags |= AUDIO_DEVICE_FLAGS_STARTED;

    return AUDIO_DEV_SUCCESS;

err_codec:
    if (audio_device->direction == AUDIO_DEV_DIR_PLAYBACK)
        dma_stop(audio_device->dma.instance, audio_device->dma.channels[TX].number);
    if (audio_device->direction == AUDIO_DEV_DIR_CAPTURE)
        dma_stop(audio_device->dma.instance, audio_device->dma.channels[RX].number);
err_dma:
    audio_interface_stop(audio_interface_instance, dir);
err:
    return errno;
}

int audio_device_stop(unsigned int instance, enum audio_device_direction dir)
{
    struct audio_device_instance *audio_device;
    int32_t errno = AUDIO_DEV_FAIL;

    if ((audio_device = __audio_device_get_instance(instance, dir)) == NULL)
        goto err;

    if (!(audio_device->flags & AUDIO_DEVICE_FLAGS_STARTED))
        return AUDIO_DEV_SUCCESS;

    switch (audio_device->audio_interface_type) {
    case AUDIO_INTERFACE_TYPE_SAI:
        if ((errno = sai_stop(audio_device->audio_interface.sai.instance, dir)) < 0)
            goto err;
        break;
    default:
        errno = ERR_AUDIO_DEV_INTERFACE_NOT_SUPPORTED;
        goto err;
    }

    if (audio_device->direction == AUDIO_DEV_DIR_PLAYBACK) {
        if ((errno = dma_stop(audio_device->dma.instance, audio_device->dma.channels[TX].number)) < 0)
            goto err;
    } else if (audio_device->direction == AUDIO_DEV_DIR_CAPTURE) {
        if ((errno = dma_stop(audio_device->dma.instance, audio_device->dma.channels[RX].number)) < 0)
            goto err;
    } else {
        errno = ERR_AUDIO_DEV_DIRECTION;
        goto err;
    }

    if ((errno = codec_stop(audio_device->codec.instance)) < 0)
        goto err;

    audio_device->flags &= ~AUDIO_DEVICE_FLAGS_STARTED;

    return AUDIO_DEV_SUCCESS;

err:
    return errno;
}

int audio_device_reset(unsigned int instance, enum audio_device_direction dir)
{
    struct audio_device_instance *audio_device;
    int32_t errno = AUDIO_DEV_FAIL;

    if ((audio_device = __audio_device_get_instance(instance, dir)) == NULL)
        goto err;

    if (audio_device->flags & AUDIO_DEVICE_FLAGS_STARTED)
        audio_device_stop(instance, dir);

    switch (audio_device->audio_interface_type) {
    case AUDIO_INTERFACE_TYPE_SAI:
        if ((errno = sai_reset(audio_device->audio_interface.sai.instance, dir)) < 0)
            goto err;
        break;
    default:
        errno = ERR_AUDIO_DEV_INTERFACE_NOT_SUPPORTED;
        goto err;
    }

    if (audio_device->direction == AUDIO_DEV_DIR_PLAYBACK) {
        if ((errno = dma_reset(audio_device->dma.instance, audio_device->dma.channels[TX].number)) < 0)
            goto err;
    } else if (audio_device->direction == AUDIO_DEV_DIR_CAPTURE) {
        if ((errno = dma_reset(audio_device->dma.instance, audio_device->dma.channels[RX].number)) < 0)
            goto err;
    } else {
        errno = ERR_AUDIO_DEV_DIRECTION;
        goto err;
    }

    return AUDIO_DEV_SUCCESS;

err:
    return errno;
}

int32_t audio_device_get_remaining_buffer_bytes(unsigned int instance, enum audio_device_direction dir)
{
    struct audio_device_instance *audio_device;
    int32_t errno = AUDIO_DEV_FAIL;

    if ((audio_device = __audio_device_get_instance(instance, dir)) == NULL)
        goto err;

    if (audio_device->direction == AUDIO_DEV_DIR_PLAYBACK) {
        return dma_get_remaining_bytes(audio_device->dma.instance, audio_device->dma.channels[TX].number);
    } else if (audio_device->direction == AUDIO_DEV_DIR_CAPTURE) {
        return dma_get_remaining_bytes(audio_device->dma.instance, audio_device->dma.channels[RX].number);
    } else {
        errno = ERR_AUDIO_DEV_DIRECTION;
        goto err;
    }

err:
    return errno;
}
