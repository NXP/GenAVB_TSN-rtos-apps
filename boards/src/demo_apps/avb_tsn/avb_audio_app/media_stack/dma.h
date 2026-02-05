/*
 * Copyright 2018-2019 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _DMA_H_
#define _DMA_H_

#include "fsl_edma.h"
#include "fsl_dmamux.h"
#include "media_stack/config.h"

#define DMA_ERR(code) ERR_CODE(DMA_ERR_GROUP_NUMBER, code)

#define DMA_SUCCESS       (0)
#define ERR_DMA_INIT      DMA_ERR(1)
#define ERR_DMA_EXIT      DMA_ERR(2)
#define ERR_DMA_CONFIGURE DMA_ERR(3)
#define ERR_DMA_START     DMA_ERR(4)
#define ERR_DMA_STOP      DMA_ERR(5)
#define ERR_DMA_RESET     DMA_ERR(6)

#define DMA_INSTANCE_FLAGS_INITIALIZED (1 << 0)

#define DMA_CHANNEL_FLAGS_CONFIGURED (1 << 0)
#define DMA_CHANNEL_FLAGS_STARTED    (1 << 1)

struct dma_channel {
    edma_handle_t handle;
    edma_transfer_config_t transfer_config;
    IRQn_Type interrupt;
    edma_tcd_t tcd __aligned(32);
    uint32_t interrupt_mask;
    uint8_t flags;
};

struct dma_instance {
    DMA_Type *base;
    DMAMUX_Type *dmamux_base;
    edma_config_t config;
    uint32_t interrupt_priority;
    uint8_t flags;
    struct dma_channel channels[DMA_MAX_CHANNELS];
};

struct dma_transfer_config {
    void *srcAddr;
    uint32_t srcWidth;
    void *destAddr;
    uint32_t destWidth;
    uint32_t bytesEachRequest;
    uint32_t transferBytes;
    bool circular;
    edma_transfer_type_t type;
    dma_request_source_t request_source;
    edma_callback callback;
    void *callback_data;
    uint32_t callback_period;
};

/**
 * Initializes the DMA instance
 * @param instance: DMA instance number
 */
int32_t dma_init(unsigned int instance);

/**
 * Deinitializes the DMA instance
 * @param instance: DMA instance number
 */
int32_t dma_exit(unsigned int instance);

/**
 * Deinitializes the DMA channel
 * @param instance: DMA instance number
 * @param instance: DMA channel number
 */
int32_t dma_exit_channel(unsigned int instance, unsigned int channel_number);

/**
 * Configures DMA transfer and enables DMA channel
 * @param instance: DMA instance number
 * @param channel: channel number
 * @param config: transfer configuration for channel
 */
int32_t dma_configure(unsigned int instance, unsigned int channel, struct dma_transfer_config *config);

/**
 * Starts DMA transfer
 * @param instance: DMA instance number
 * @param channel: channel number
 */
int32_t dma_start(unsigned int instance, unsigned int channel);

/**
 * Stops DMA transfer
 * @param instance: DMA instance number
 * @param channel: channel number
 */
int32_t dma_stop(unsigned int instance, unsigned int channel);

/**
 * Resets dma channel TCD to it's initial config value
 */
int32_t dma_reset(unsigned int instance, unsigned int channel);

/**
 *
 */
uint32_t dma_get_remaining_bytes(unsigned int instance, unsigned int channel);

#endif /* _DMA_H_ */
