/*
 * Copyright 2018-2019 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "dma.h"
#include "audio_device.h"

extern struct dma_instance dma_instances[DMA_MAX_INSTANCES];

static struct dma_instance *__dma_get_instance(unsigned int instance)
{
    if (instance >= DMA_MAX_INSTANCES)
        return NULL;

    return &dma_instances[instance];
}

static struct dma_channel *__dma_get_channel(struct dma_instance *dma, unsigned int channel_number)
{
    if (channel_number >= DMA_MAX_CHANNELS)
        return NULL;

    return &dma->channels[channel_number];
}

static inline void __dma_stop_channel(struct dma_channel *ch)
{
    if (ch && ch->flags & DMA_CHANNEL_FLAGS_STARTED) {
        EDMA_StopTransfer(&ch->handle);
        ch->flags &= ~DMA_CHANNEL_FLAGS_STARTED;
    }
}

int32_t dma_init(unsigned int instance)
{
    struct dma_instance *dma;

    if ((dma = __dma_get_instance(instance)) == NULL)
        goto err;

    if (dma->flags & DMA_INSTANCE_FLAGS_INITIALIZED)
        return DMA_SUCCESS;

    EDMA_GetDefaultConfig(&dma->config);

    EDMA_Init(dma->base, &dma->config);

    DMAMUX_Init(dma->dmamux_base);

    dma->flags |= DMA_INSTANCE_FLAGS_INITIALIZED;

    return DMA_SUCCESS;

err:
    return ERR_DMA_INIT;
}

static int32_t __dma_exit_channel(struct dma_instance *dma, unsigned int channel_number)
{
    struct dma_channel *dma_channel;

    dma_channel = &dma->channels[channel_number];

    DMAMUX_DisableChannel(dma->dmamux_base, channel_number);

    if (dma_channel->handle.callback) {
        if (DisableIRQ(dma_channel->interrupt) != kStatus_Success)
            goto err;
        EDMA_DisableChannelInterrupts(dma->base, channel_number, dma_channel->interrupt_mask);
    }

    __dma_stop_channel(dma_channel);

    EDMA_ResetChannel(dma->base, channel_number);

    memset(&dma_channel->handle, 0, sizeof(edma_handle_t));

    dma_channel->flags &= ~DMA_CHANNEL_FLAGS_CONFIGURED;

    return DMA_SUCCESS;

err:
    return ERR_DMA_EXIT;
}

int32_t dma_exit_channel(unsigned int instance, unsigned int channel_number)
{
    struct dma_instance *dma;

    if ((dma = __dma_get_instance(instance)) == NULL)
        goto err;

    return __dma_exit_channel(dma, channel_number);

err:
    return ERR_DMA_EXIT;
}

int32_t dma_exit(unsigned int instance)
{
    struct dma_instance *dma;
    unsigned int ch_n;

    if ((dma = __dma_get_instance(instance)) == NULL)
        goto err;

    /* reset all channels */
    for (ch_n = 0; ch_n < DMA_MAX_CHANNELS; ch_n++)
        __dma_exit_channel(dma, ch_n);

    EDMA_Deinit(dma->base);

    dma->flags &= ~DMA_INSTANCE_FLAGS_INITIALIZED;

    return DMA_SUCCESS;

err:
    return ERR_DMA_EXIT;
}

int32_t dma_configure(unsigned int instance, unsigned int channel_number, struct dma_transfer_config *config)
{
    struct dma_instance *dma;
    struct dma_channel *dma_channel;

    if ((dma = __dma_get_instance(instance)) == NULL)
        goto err;

    if ((dma_channel = __dma_get_channel(dma, channel_number)) == NULL)
        goto err;

    if (!(dma->flags & DMA_INSTANCE_FLAGS_INITIALIZED))
        goto err;

    if (dma_channel->flags & DMA_CHANNEL_FLAGS_STARTED)
        goto err;

    EDMA_CreateHandle(&dma_channel->handle, dma->base, channel_number);

    EDMA_PrepareTransfer(&dma_channel->transfer_config, config->srcAddr, config->srcWidth, config->destAddr, config->destWidth, config->bytesEachRequest, config->transferBytes, config->type);

    EDMA_TcdSetTransferConfig(&dma_channel->tcd, &dma_channel->transfer_config, NULL);

    /* Configure dma transfer in circular mode */
    if (config->circular) {
        if (config->type == kEDMA_MemoryToMemory) {
            dma_channel->tcd.SLAST = -config->transferBytes;
            dma_channel->tcd.DLAST_SGA = -config->transferBytes;
        } else if (config->type == kEDMA_MemoryToPeripheral) {
            dma_channel->tcd.SLAST = -config->transferBytes;
            dma_channel->tcd.DLAST_SGA = 0;
        } else if (config->type == kEDMA_PeripheralToMemory) {
            dma_channel->tcd.SLAST = 0;
            dma_channel->tcd.DLAST_SGA = -config->transferBytes;
        } else {
            goto err;
        }
    } else {
        EDMA_TcdEnableAutoStopRequest(&dma_channel->tcd, true);
    }

    /* Set the DMAMUX channel source */
    DMAMUX_SetSource(dma->dmamux_base, channel_number, config->request_source);

    if (config->callback) {
        /* TODO: for now we support only half and complete transfer callbacks */
        if (config->callback_period == (config->transferBytes / 2))
            dma_channel->interrupt_mask = kEDMA_HalfInterruptEnable | kEDMA_MajorInterruptEnable | kEDMA_ErrorInterruptEnable;
        else if (config->callback_period == config->transferBytes)
            dma_channel->interrupt_mask = kEDMA_MajorInterruptEnable | kEDMA_ErrorInterruptEnable;
        else
            goto err_cb_period;

        NVIC_SetPriority(dma_channel->interrupt, dma->interrupt_priority);

        /* Set callback */
        EDMA_SetCallback(&dma_channel->handle, config->callback, config->callback_data);
        EDMA_TcdEnableInterrupts(&dma_channel->tcd, dma_channel->interrupt_mask);
    }

    EDMA_InstallTCD(dma->base, channel_number, &dma_channel->tcd);

    dma_channel->flags |= DMA_CHANNEL_FLAGS_CONFIGURED;

    DMAMUX_EnableChannel(dma->dmamux_base, channel_number);

    return DMA_SUCCESS;

err_cb_period:
    EDMA_ResetChannel(dma->base, channel_number);
err:
    return ERR_DMA_CONFIGURE;
}

int32_t dma_start(unsigned int instance, unsigned int channel_number)
{
    struct dma_instance *dma;
    struct dma_channel *dma_channel;

    if ((dma = __dma_get_instance(instance)) == NULL)
        goto err;

    if ((dma_channel = __dma_get_channel(dma, channel_number)) == NULL)
        goto err;

    if (!(dma_channel->flags & DMA_CHANNEL_FLAGS_CONFIGURED))
        goto err;

    if (dma_channel->flags & DMA_CHANNEL_FLAGS_STARTED)
        return DMA_SUCCESS;

    EDMA_StartTransfer(&dma_channel->handle);

    dma_channel->flags |= DMA_CHANNEL_FLAGS_STARTED;

    return DMA_SUCCESS;

err:
    return ERR_DMA_START;
}

int32_t dma_stop(unsigned int instance, unsigned int channel_number)
{
    struct dma_instance *dma;
    struct dma_channel *dma_channel;

    if ((dma = __dma_get_instance(instance)) == NULL)
        goto err;

    if ((dma_channel = __dma_get_channel(dma, channel_number)) == NULL)
        goto err;

    __dma_stop_channel(dma_channel);

    return DMA_SUCCESS;

err:
    return ERR_DMA_STOP;
}

int32_t dma_reset(unsigned int instance, unsigned int channel_number)
{
    struct dma_instance *dma;
    struct dma_channel *dma_channel;

    if ((dma = __dma_get_instance(instance)) == NULL)
        goto err;

    if ((dma_channel = __dma_get_channel(dma, channel_number)) == NULL)
        goto err;

    if (!(dma_channel->flags & DMA_CHANNEL_FLAGS_CONFIGURED))
        goto err;

    if (dma_channel->flags & DMA_CHANNEL_FLAGS_STARTED)
        goto err;

    EDMA_ResetChannel(dma->base, channel_number);

    EDMA_InstallTCD(dma->base, channel_number, &dma_channel->tcd);

    return DMA_SUCCESS;
err:
    return ERR_DMA_RESET;
}

uint32_t dma_get_remaining_bytes(unsigned int instance, unsigned int channel_number)
{
    struct dma_instance *dma;
    struct dma_channel *dma_channel;

    if ((dma = __dma_get_instance(instance)) == NULL)
        goto err;

    if ((dma_channel = __dma_get_channel(dma, channel_number)) == NULL)
        goto err;

    return EDMA_GetRemainingMajorLoopCount(dma->base, channel_number) * dma_channel->transfer_config.minorLoopBytes;

err:
    return -1;
}
