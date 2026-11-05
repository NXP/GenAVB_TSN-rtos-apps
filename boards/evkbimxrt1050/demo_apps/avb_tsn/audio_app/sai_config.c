/*
 * Copyright 2022, 2024-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_common.h"
#include "fsl_sai.h"

#include "app_board.h"

#include "rtos_apps/audio/audio_app.h"

struct sai_active_config audio_app_sai_active_list[] = {
    {
        .sai_base = APP_SAI_BASE,
        .clk_id = APP_SAI_CLKD_ID,
        .clk_freq = APP_SAI_CLK_FREQ,
        .masterSlave = APP_SAI_MASTER_SLAVE,
        .tx_sync_mode = APP_SAI_TX_SYNC_MODE,
        .rx_sync_mode = APP_SAI_RX_SYNC_MODE,
        .msel = APP_SAI_BIT_CLK_SRC,
        .codec_id = APP_SAI_CID,
        .slot_count = APP_SAI_AUDIO_DATA_CHANNEL,
        .slot_size = APP_SAI_AUDIO_BIT_WIDTH,
        .rx_mask = APP_SAI_DATA_MASK_NONE,
        .tx_mask = APP_SAI_DATA_MASK_NONE,
    },
};

uint32_t audio_app_sai_active_list_nelems = ARRAY_SIZE(audio_app_sai_active_list);
