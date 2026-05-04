/*
 * Copyright 2019-2020, 2023-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "media_stack/config.h"
#include "FreeRTOS.h"
#include "board.h"
#include "media_stack/audio_device.h"
#include "media_stack/codec.h"
#include "media_stack/common.h"
#include "media_stack/dma.h"
#include "media_stack/i2c.h"
#include "media_stack/sai.h"

struct sai_instance sai_instances[SAI_MAX_INSTANCES] = {
    [0] =
        {
            .base = SAI1,
            .config = {0},
            .watermark = FSL_FEATURE_SAI_FIFO_COUNTn(SAI1) / 2U,
            .frame_width = 32,
            .transfer_config =
                {
                    [TX] =
                        {
                            .handle = {0},
                            .channel = 0,
                            .dma_request_source = kDmaRequestMuxSai1Tx,
                        },
                    [RX] =
                        {
                            .handle = {0},
                            .channel = 0,
                            .dma_request_source = kDmaRequestMuxSai1Rx,
                        },
                },
            .flags = 0,
            .mode = SAI_TXRX_DEPENDENT,
        },
    [1] =
        {
            .base = SAI2,
            .config = {0},
            .watermark = FSL_FEATURE_SAI_FIFO_COUNTn(SAI2) / 2U,
            .frame_width = 32,
            .transfer_config =
                {
                    [TX] =
                        {
                            .handle = {0},
                            .channel = 0,
                            .dma_request_source = kDmaRequestMuxSai2Tx,
                        },
                    [RX] =
                        {
                            .handle = {0},
                            .channel = 0,
                            .dma_request_source = kDmaRequestMuxSai2Rx,
                        },
                },
            .flags = 0,
            .mode = SAI_TXRX_INDEPENDENT,
        },
    [2] =
        {
            .base = SAI3,
            .config = {0},
            .watermark = FSL_FEATURE_SAI_FIFO_COUNTn(SAI3) / 2U,
            .frame_width = 32,
            .transfer_config =
                {
                    [TX] =
                        {
                            .handle = {0},
                            .channel = 0,
                            .dma_request_source = kDmaRequestMuxSai3Tx,
                        },
                    [RX] =
                        {
                            .handle = {0},
                            .channel = 0,
                            .dma_request_source = kDmaRequestMuxSai3Rx,
                        },
                },
            .flags = 0,
            .mode = SAI_TXRX_INDEPENDENT,
        },
};

struct i2c_instance i2c_instances[I2C_MAX_INSTANCES] = {
    {
        .base = LPI2C1,
        .handle = {0},
        .flags = 0,
        .status = 0,
    },
    {
        .base = LPI2C2,
        .handle = {0},
        .flags = 0,
        .status = 0,
    },
    {
        .base = LPI2C3,
        .handle = {0},
        .flags = 0,
        .status = 0,
    },
    {
        .base = LPI2C4,
        .handle = {0},
        .flags = 0,
        .status = 0,
    },
};

struct dma_instance dma_instances[DMA_MAX_INSTANCES] = {
    {
        .base = DMA0,
        .dmamux_base = DMAMUX,
        .config = {0},
        .interrupt_priority = configMAX_SYSCALL_INTERRUPT_PRIORITY + 2,
        .flags = 0,
        .channels =
            {
                {
                    .handle = {0},
                    .transfer_config = {0},
                    .interrupt = DMA0_DMA16_IRQn,
                    .tcd = {0},
                    .interrupt_mask = 0,
                    .flags = 0,
                },
                {
                    .handle = {0},
                    .transfer_config = {0},
                    .interrupt = DMA1_DMA17_IRQn,
                    .tcd = {0},
                    .interrupt_mask = 0,
                    .flags = 0,
                },
                {
                    .handle = {0},
                    .transfer_config = {0},
                    .interrupt = DMA2_DMA18_IRQn,
                    .tcd = {0},
                    .interrupt_mask = 0,
                    .flags = 0,
                },
                {
                    .handle = {0},
                    .transfer_config = {0},
                    .interrupt = DMA3_DMA19_IRQn,
                    .tcd = {0},
                    .interrupt_mask = 0,
                    .flags = 0,
                },
                {
                    .handle = {0},
                    .transfer_config = {0},
                    .interrupt = DMA4_DMA20_IRQn,
                    .tcd = {0},
                    .interrupt_mask = 0,
                    .flags = 0,
                },
                {
                    .handle = {0},
                    .transfer_config = {0},
                    .interrupt = DMA5_DMA21_IRQn,
                    .tcd = {0},
                    .interrupt_mask = 0,
                    .flags = 0,
                },
                {
                    .handle = {0},
                    .transfer_config = {0},
                    .interrupt = DMA6_DMA22_IRQn,
                    .tcd = {0},
                    .interrupt_mask = 0,
                    .flags = 0,
                },
                {
                    .handle = {0},
                    .transfer_config = {0},
                    .interrupt = DMA7_DMA23_IRQn,
                    .tcd = {0},
                    .interrupt_mask = 0,
                    .flags = 0,
                },
                {
                    .handle = {0},
                    .transfer_config = {0},
                    .interrupt = DMA8_DMA24_IRQn,
                    .tcd = {0},
                    .interrupt_mask = 0,
                    .flags = 0,
                },
                {
                    .handle = {0},
                    .transfer_config = {0},
                    .interrupt = DMA9_DMA25_IRQn,
                    .tcd = {0},
                    .interrupt_mask = 0,
                    .flags = 0,
                },
                {
                    .handle = {0},
                    .transfer_config = {0},
                    .interrupt = DMA10_DMA26_IRQn,
                    .tcd = {0},
                    .interrupt_mask = 0,
                    .flags = 0,
                },
                {
                    .handle = {0},
                    .transfer_config = {0},
                    .interrupt = DMA11_DMA27_IRQn,
                    .tcd = {0},
                    .interrupt_mask = 0,
                    .flags = 0,
                },
                {
                    .handle = {0},
                    .transfer_config = {0},
                    .interrupt = DMA12_DMA28_IRQn,
                    .tcd = {0},
                    .interrupt_mask = 0,
                    .flags = 0,
                },
                {
                    .handle = {0},
                    .transfer_config = {0},
                    .interrupt = DMA13_DMA29_IRQn,
                    .tcd = {0},
                    .interrupt_mask = 0,
                    .flags = 0,
                },
                {
                    .handle = {0},
                    .transfer_config = {0},
                    .interrupt = DMA14_DMA30_IRQn,
                    .tcd = {0},
                    .interrupt_mask = 0,
                    .flags = 0,
                },
                {
                    .handle = {0},
                    .transfer_config = {0},
                    .interrupt = DMA15_DMA31_IRQn,
                    .tcd = {0},
                    .interrupt_mask = 0,
                    .flags = 0,
                },
                {
                    .handle = {0},
                    .transfer_config = {0},
                    .interrupt = DMA0_DMA16_IRQn,
                    .tcd = {0},
                    .interrupt_mask = 0,
                    .flags = 0,
                },
                {
                    .handle = {0},
                    .transfer_config = {0},
                    .interrupt = DMA1_DMA17_IRQn,
                    .tcd = {0},
                    .interrupt_mask = 0,
                    .flags = 0,
                },
                {
                    .handle = {0},
                    .transfer_config = {0},
                    .interrupt = DMA2_DMA18_IRQn,
                    .tcd = {0},
                    .interrupt_mask = 0,
                    .flags = 0,
                },
                {
                    .handle = {0},
                    .transfer_config = {0},
                    .interrupt = DMA3_DMA19_IRQn,
                    .tcd = {0},
                    .interrupt_mask = 0,
                    .flags = 0,
                },
                {
                    .handle = {0},
                    .transfer_config = {0},
                    .interrupt = DMA4_DMA20_IRQn,
                    .tcd = {0},
                    .interrupt_mask = 0,
                    .flags = 0,
                },
                {
                    .handle = {0},
                    .transfer_config = {0},
                    .interrupt = DMA5_DMA21_IRQn,
                    .tcd = {0},
                    .interrupt_mask = 0,
                    .flags = 0,
                },
                {
                    .handle = {0},
                    .transfer_config = {0},
                    .interrupt = DMA6_DMA22_IRQn,
                    .tcd = {0},
                    .interrupt_mask = 0,
                    .flags = 0,
                },
                {
                    .handle = {0},
                    .transfer_config = {0},
                    .interrupt = DMA7_DMA23_IRQn,
                    .tcd = {0},
                    .interrupt_mask = 0,
                    .flags = 0,
                },
                {
                    .handle = {0},
                    .transfer_config = {0},
                    .interrupt = DMA8_DMA24_IRQn,
                    .tcd = {0},
                    .interrupt_mask = 0,
                    .flags = 0,
                },
                {
                    .handle = {0},
                    .transfer_config = {0},
                    .interrupt = DMA9_DMA25_IRQn,
                    .tcd = {0},
                    .interrupt_mask = 0,
                    .flags = 0,
                },
                {
                    .handle = {0},
                    .transfer_config = {0},
                    .interrupt = DMA10_DMA26_IRQn,
                    .tcd = {0},
                    .interrupt_mask = 0,
                    .flags = 0,
                },
                {
                    .handle = {0},
                    .transfer_config = {0},
                    .interrupt = DMA11_DMA27_IRQn,
                    .tcd = {0},
                    .interrupt_mask = 0,
                    .flags = 0,
                },
                {
                    .handle = {0},
                    .transfer_config = {0},
                    .interrupt = DMA12_DMA28_IRQn,
                    .tcd = {0},
                    .interrupt_mask = 0,
                    .flags = 0,
                },
                {
                    .handle = {0},
                    .transfer_config = {0},
                    .interrupt = DMA13_DMA29_IRQn,
                    .tcd = {0},
                    .interrupt_mask = 0,
                    .flags = 0,
                },
                {
                    .handle = {0},
                    .transfer_config = {0},
                    .interrupt = DMA14_DMA30_IRQn,
                    .tcd = {0},
                    .interrupt_mask = 0,
                    .flags = 0,
                },
                {
                    .handle = {0},
                    .transfer_config = {0},
                    .interrupt = DMA15_DMA31_IRQn,
                    .tcd = {0},
                    .interrupt_mask = 0,
                    .flags = 0,
                },
            },
    },
};

wm8960_handle_t wm8960_handle = {
    .instance = 0,
    .send_data = &codec_send_control_data,
};

struct codec_instance codec_instances[CODEC_MAX_INSTANCES] = {
    [0] =
        {
            .type = CODEC_TYPE_WM8960,
            .control_interface_type = CODEC_CONTROL_INTERFACE_I2C,
            .control_interface_instance = BOARD_CODEC_I2C_INSTANCE - 1,
            .transfer_config = {0},
            .flags = 0,
            .users = 0,
            .handle = &wm8960_handle,
        },
    [1] =
        {
            .type = CODEC_TYPE_NO_DEV,
            .control_interface_type = CODEC_NO_CONTROL_INTERFACE,
        },
};

struct audio_device_instance audio_playback_instances[AUDIO_DEVICE_MAX_PLAYBACK_INSTANCES] = {
        [0] =
            {
                .direction = AUDIO_DEV_DIR_PLAYBACK,
                .audio_interface_type = AUDIO_INTERFACE_TYPE_SAI,
                .audio_interface =
                    {
                        .sai =
                            {
                                .instance = 0,
                            },
                    },
                .dma =
                    {
                        .instance = 0,
                        .channels =
                            {
                                [TX] =
                                    {
                                        .number = 0,
                                    },
                            },
                    },
                .codec =
                    {
                        .instance = 0,
                    },
                .flags = 0,
                .cb_data = {0},
            },
        [1] =
            {
                .direction = AUDIO_DEV_DIR_PLAYBACK,
                .audio_interface_type = AUDIO_INTERFACE_TYPE_SAI,
                .audio_interface =
                    {
                        .sai =
                            {
                                .instance = 1,
                            },
                    },
                .dma =
                    {
                        .instance = 0,
                        .channels =
                            {
                                [TX] =
                                    {
                                        .number = 2,
                                    },
                            },
                    },
                .codec =
                    {
                        .instance = CODEC_NO_DEV_INSTANCE,
                    },
                .flags = 0,
                .cb_data = {0},
            },
        [2] =
            {
                .direction = AUDIO_DEV_DIR_PLAYBACK,
                .audio_interface_type = AUDIO_INTERFACE_TYPE_SAI,
                .audio_interface =
                    {
                        .sai =
                            {
                                .instance = 2,
                            },
                    },
                .dma =
                    {
                        .instance = 0,
                        .channels =
                            {
                                [TX] =
                                    {
                                        .number = 4,
                                    },
                            },
                    },
                .codec =
                    {
                        .instance = CODEC_NO_DEV_INSTANCE,
                    },
                .flags = 0,
                .cb_data = {0},
            },
};

struct audio_device_instance audio_capture_instances[AUDIO_DEVICE_MAX_CAPTURE_INSTANCES] = {
        [0] =
            {
                .direction = AUDIO_DEV_DIR_CAPTURE,
                .audio_interface_type = AUDIO_INTERFACE_TYPE_SAI,
                .audio_interface =
                    {
                        .sai =
                            {
                                .instance = 0,
                            },
                    },
                .dma =
                    {
                        .instance = 0,
                        .channels =
                            {
                                [RX] =
                                    {
                                        .number = 1,
                                    },
                            },
                    },
                .codec =
                    {
                        .instance = 0,
                    },
                .flags = 0,
                .cb_data = {0},
            },
        [1] =
            {
                .direction = AUDIO_DEV_DIR_CAPTURE,
                .audio_interface_type = AUDIO_INTERFACE_TYPE_SAI,
                .audio_interface =
                    {
                        .sai =
                            {
                                .instance = 1,
                            },
                    },
                .dma =
                    {
                        .instance = 0,
                        .channels =
                            {
                                [RX] =
                                    {
                                        .number = 3,
                                    },
                            },
                    },
                .codec =
                    {
                        .instance = CODEC_NO_DEV_INSTANCE,
                    },
                .flags = 0,
                .cb_data = {0},
            },
        [2] =
            {
                .direction = AUDIO_DEV_DIR_CAPTURE,
                .audio_interface_type = AUDIO_INTERFACE_TYPE_SAI,
                .audio_interface =
                    {
                        .sai =
                            {
                                .instance = 2,
                            },
                    },
                .dma =
                    {
                        .instance = 0,
                        .channels =
                            {
                                [RX] =
                                    {
                                        .number = 5,
                                    },
                            },
                    },
                .codec =
                    {
                        .instance = CODEC_NO_DEV_INSTANCE,
                    },
                .flags = 0,
                .cb_data = {0},
            },
};
