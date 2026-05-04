/*
 * Copyright 2018-2020, 2023, 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "avb_tsn/common/system_config.h"
#include "audio_device.h"
#include "board.h"

extern struct sai_instance sai_instances[SAI_MAX_INSTANCES];

static struct sai_instance *__sai_get_instance(int instance)
{
    if (instance >= SAI_MAX_INSTANCES)
        return NULL;

    return &sai_instances[instance];
}

static void sai_init_clock(struct sai_instance *sai)
{
    struct sai_clock_config *config = &sai->clock_config;

    config->masterClockHz = BOARD_SAI_MCLK_HZ;
}

int32_t sai_init(unsigned int instance, enum audio_device_direction dir)
{
    struct sai_instance *sai;

    if ((sai = __sai_get_instance(instance)) == NULL)
        goto err;

    if (!(sai->flags & SAI_FLAGS_INITIALIZED)) {
        sai_init_clock(sai);
        SAI_Init(sai->base);
        sai->flags |= SAI_FLAGS_INITIALIZED;
    }

    switch (sai->mode) {
    case SAI_TXRX_INDEPENDENT:
        if (dir == TX) {
            if (sai->flags & SAI_TX_FLAGS_INITIALIZED)
                goto out;

            SAI_TransferTxCreateHandle(sai->base, &sai->transfer_config[TX].handle, NULL, NULL);

            sai->flags |= SAI_TX_FLAGS_INITIALIZED;
        } else if (dir == RX) {
            if (sai->flags & SAI_RX_FLAGS_INITIALIZED)
                goto out;

            SAI_TransferRxCreateHandle(sai->base, &sai->transfer_config[RX].handle, NULL, NULL);

            sai->flags |= SAI_RX_FLAGS_INITIALIZED;
        } else {
            goto err;
        }

        break;
    case SAI_TXRX_DEPENDENT:
        /**
         * Dependent mode is used for SAI's which share BCLK and SYNC sygnals
         * between TX and RX lines
         */
        if (sai->flags & (SAI_TX_FLAGS_INITIALIZED | SAI_RX_FLAGS_INITIALIZED))
            goto out;

        SAI_TransferTxCreateHandle(sai->base, &sai->transfer_config[TX].handle, NULL, NULL);
        SAI_TransferRxCreateHandle(sai->base, &sai->transfer_config[RX].handle, NULL, NULL);

        sai->flags |= SAI_TX_FLAGS_INITIALIZED | SAI_RX_FLAGS_INITIALIZED;

        break;
    default:
        goto err;
    }

out:
    return SAI_SUCCESS;

err:
    return ERR_SAI_INIT;
}

int32_t sai_exit(unsigned int instance, enum audio_device_direction dir)
{
    struct sai_instance *sai;

    if ((sai = __sai_get_instance(instance)) == NULL)
        goto err;

    sai_stop(instance, dir);

    switch (sai->mode) {
    case SAI_TXRX_INDEPENDENT:
        if (dir == TX) {
            SAI_TxReset(sai->base);
            sai->flags &= ~(SAI_TX_FLAGS_INITIALIZED | SAI_TX_FLAGS_CONFIGURED);
        } else if (dir == RX) {
            SAI_RxReset(sai->base);
            sai->flags &= ~(SAI_RX_FLAGS_INITIALIZED | SAI_RX_FLAGS_CONFIGURED);
        } else {
            goto err;
        }

        break;
    case SAI_TXRX_DEPENDENT:
        if (dir == TX)
            sai->flags &= ~(SAI_TX_FLAGS_INITIALIZED | SAI_TX_FLAGS_CONFIGURED);
        else if (dir == RX)
            sai->flags &= ~(SAI_RX_FLAGS_INITIALIZED | SAI_RX_FLAGS_CONFIGURED);
        else
            goto err;

        if (!(sai->flags & (SAI_RX_FLAGS_INITIALIZED | SAI_RX_FLAGS_CONFIGURED | SAI_TX_FLAGS_INITIALIZED | SAI_TX_FLAGS_CONFIGURED))) {
            SAI_TxReset(sai->base);
            SAI_RxReset(sai->base);
        }
        break;
    default:
        goto err;
    }

    if (sai->flags == SAI_FLAGS_INITIALIZED) {
        SAI_Deinit(sai->base);
        sai->flags = 0;
    }

    return SAI_SUCCESS;

err:
    return ERR_SAI_EXIT;
}

int32_t sai_configure(unsigned int instance, enum audio_device_direction dir, struct sai_transfer_format *transfer_format)
{
    struct sai_instance *sai;
    sai_transceiver_t config = {0};
    sai_mono_stereo_t stereo;
    uint32_t channel_mask;

    if ((sai = __sai_get_instance(instance)) == NULL)
        goto err;

    if (sai->flags & (SAI_TX_FLAGS_CONFIGURED | SAI_RX_FLAGS_CONFIGURED)) {
        if (memcmp(&sai->transfer_format, transfer_format, sizeof(struct sai_transfer_format)))
            goto err;
    } else {
        memcpy(&sai->transfer_format, transfer_format, sizeof(struct sai_transfer_format));
    }

    stereo = (transfer_format->channels == 2) ? kSAI_Stereo : kSAI_MonoRight;

    switch (sai->mode) {
    case SAI_TXRX_INDEPENDENT:
        if (dir == TX) {
            if (!(sai->flags & SAI_TX_FLAGS_INITIALIZED))
                goto err;

            channel_mask = (1 << sai->transfer_config[TX].channel);

            SAI_GetLeftJustifiedConfig(&config, kSAI_WordWidth32bits, stereo, channel_mask);
            config.serialData.dataFirstBitShifted = transfer_format->bitWidth;

            SAI_TransferTxSetConfig(sai->base, &sai->transfer_config[TX].handle, &config);
            SAI_TxSetBitClockRate(sai->base, sai->clock_config.masterClockHz, transfer_format->sampleRate_Hz, sai->frame_width, transfer_format->channels);

            sai->flags |= SAI_TX_FLAGS_CONFIGURED;
        } else if (dir == RX) {
            if (!(sai->flags & SAI_RX_FLAGS_INITIALIZED))
                goto err;

            channel_mask = (1 << sai->transfer_config[RX].channel);

            SAI_GetLeftJustifiedConfig(&config, kSAI_WordWidth32bits, stereo, channel_mask);
            config.serialData.dataFirstBitShifted = transfer_format->bitWidth;
            config.syncMode = kSAI_ModeAsync;

            SAI_TransferRxSetConfig(sai->base, &sai->transfer_config[RX].handle, &config);
            SAI_RxSetBitClockRate(sai->base, sai->clock_config.masterClockHz, transfer_format->sampleRate_Hz, sai->frame_width, transfer_format->channels);

            sai->flags |= SAI_RX_FLAGS_CONFIGURED;
        } else {
            goto err;
        }
        break;
    case SAI_TXRX_DEPENDENT:
        if (!(sai->flags & (SAI_TX_FLAGS_INITIALIZED | SAI_RX_FLAGS_INITIALIZED)))
            goto err;

        if (sai->flags & (SAI_TX_FLAGS_CONFIGURED | SAI_RX_FLAGS_CONFIGURED))
            goto out;

        channel_mask = (1 << sai->transfer_config[TX].channel) | (1 << sai->transfer_config[RX].channel);

        SAI_GetLeftJustifiedConfig(&config, kSAI_WordWidth32bits, stereo, channel_mask);
        config.serialData.dataFirstBitShifted = transfer_format->bitWidth;

        SAI_TransferTxSetConfig(sai->base, &sai->transfer_config[TX].handle, &config);
        SAI_TxSetBitClockRate(sai->base, sai->clock_config.masterClockHz, transfer_format->sampleRate_Hz, sai->frame_width, transfer_format->channels);

        config.syncMode = kSAI_ModeSync;
        SAI_TransferRxSetConfig(sai->base, &sai->transfer_config[RX].handle, &config);
        SAI_RxSetBitClockRate(sai->base, sai->clock_config.masterClockHz, transfer_format->sampleRate_Hz, sai->frame_width, transfer_format->channels);

        sai->flags |= SAI_TX_FLAGS_CONFIGURED | SAI_RX_FLAGS_CONFIGURED;
        break;
    default:
        goto err;
    }

out:
    return SAI_SUCCESS;

err:
    return ERR_SAI_CONFIGURE;
}

int32_t sai_start(unsigned int instance, enum audio_device_direction dir)
{
    struct sai_instance *sai;

    if ((sai = __sai_get_instance(instance)) == NULL)
        goto err;

    switch (sai->mode) {
    case SAI_TXRX_INDEPENDENT:
        if (dir == TX) {
            if (!(sai->flags & SAI_TX_FLAGS_CONFIGURED))
                goto err;

            if (sai->flags & SAI_TX_FLAGS_STARTED)
                goto out;

            SAI_TxEnableDMA(sai->base, kSAI_FIFORequestDMAEnable, true);
            SAI_TxEnable(sai->base, true);

            sai->flags |= SAI_TX_FLAGS_STARTED;
        } else if (dir == RX) {
            if (!(sai->flags & SAI_RX_FLAGS_CONFIGURED))
                goto err;

            if (sai->flags & SAI_RX_FLAGS_STARTED)
                goto out;

            SAI_RxEnableDMA(sai->base, kSAI_FIFORequestDMAEnable, true);
            SAI_RxEnable(sai->base, true);

            sai->flags |= SAI_RX_FLAGS_STARTED;
        } else {
            goto err;
        }

        break;
    case SAI_TXRX_DEPENDENT:
        if (!(sai->flags & (SAI_TX_FLAGS_CONFIGURED | SAI_RX_FLAGS_CONFIGURED)))
            goto err;

        /* Enable TX first, no matter which direction */
        if (!(sai->flags & (SAI_TX_FLAGS_STARTED | SAI_RX_FLAGS_STARTED))) {
            SAI_TxEnableDMA(sai->base, kSAI_FIFORequestDMAEnable, true);
            SAI_TxEnable(sai->base, true);
            sai->flags |= SAI_TX_FLAGS_STARTED;
        }

        if ((dir == RX) && !(sai->flags & SAI_RX_FLAGS_STARTED)) {
            SAI_RxEnableDMA(sai->base, kSAI_FIFORequestDMAEnable, true);
            SAI_RxEnable(sai->base, true);
            sai->flags |= SAI_RX_FLAGS_STARTED;
        }

        break;
    default:
        goto err;
    }

out:
    return SAI_SUCCESS;
err:
    return ERR_SAI_START;
}

int32_t sai_stop(unsigned int instance, enum audio_device_direction dir)
{
    struct sai_instance *sai;

    if ((sai = __sai_get_instance(instance)) == NULL)
        goto err;

    switch (sai->mode) {
    case SAI_TXRX_INDEPENDENT:
        if (dir == TX) {
            SAI_TxEnable(sai->base, false);
            SAI_TxEnableDMA(sai->base, kSAI_FIFORequestDMAEnable, false);

            sai->flags &= ~SAI_TX_FLAGS_STARTED;
        } else if (dir == RX) {
            SAI_RxEnable(sai->base, false);
            SAI_RxEnableDMA(sai->base, kSAI_FIFORequestDMAEnable, false);

            sai->flags &= ~SAI_RX_FLAGS_STARTED;
        } else {
            goto err;
        }

        break;
    case SAI_TXRX_DEPENDENT:
        if ((dir == RX) && (sai->flags & SAI_RX_FLAGS_STARTED)) {
            SAI_RxEnable(sai->base, false);
            SAI_RxEnableDMA(sai->base, kSAI_FIFORequestDMAEnable, false);
            sai->flags &= ~SAI_RX_FLAGS_STARTED;
        }

        if (dir == TX) {
            sai->flags &= ~SAI_TX_FLAGS_STARTED;
        }

        /* Disable TX last */
        if (!(sai->flags & (SAI_TX_FLAGS_STARTED | SAI_RX_FLAGS_STARTED))) {
            SAI_TxEnable(sai->base, false);
            SAI_TxEnableDMA(sai->base, kSAI_FIFORequestDMAEnable, false);
        }

        break;
    default:
        goto err;
    }

    return SAI_SUCCESS;
err:
    return ERR_SAI_STOP;
}

int32_t sai_reset(unsigned int instance, enum audio_device_direction dir)
{
    struct sai_instance *sai;

    if ((sai = __sai_get_instance(instance)) == NULL)
        goto err;

    if (sai_stop(instance, dir))
        goto err;

    switch (sai->mode) {
    case SAI_TXRX_INDEPENDENT:
    case SAI_TXRX_DEPENDENT:
        if (dir == TX)
            SAI_TxSoftwareReset(sai->base, kSAI_ResetAll);
        else if (dir == RX)
            SAI_RxSoftwareReset(sai->base, kSAI_ResetAll);
        else
            goto err;
        break;
    default:
        goto err;
    }

    return SAI_SUCCESS;

err:
    return ERR_SAI_RESET;
}

int32_t sai_get_config(unsigned int instance, struct audio_interface_config *config)
{
    struct sai_instance *sai;
    if ((sai = __sai_get_instance(instance)) == NULL)
        goto err;

    if (!(sai->flags & SAI_FLAGS_INITIALIZED))
        goto err;

    config->masterClockHz = sai->clock_config.masterClockHz;
    config->protocol = sai->config.protocol;

    config->data_input_addr = (void *)SAI_RxGetDataRegisterAddress(sai->base, sai->transfer_config[RX].channel);
    config->data_output_addr = (void *)SAI_TxGetDataRegisterAddress(sai->base, sai->transfer_config[TX].channel);

    config->dma_request_source[TX] = sai->transfer_config[TX].dma_request_source;
    config->dma_request_source[RX] = sai->transfer_config[RX].dma_request_source;
    config->dma_request_size = FSL_FEATURE_SAI_FIFO_COUNTn(sai->base);

    return SAI_SUCCESS;
err:
    return ERR_SAI_GET_CONFIG;
}
