/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _APP_BOARD_H_
#define _APP_BOARD_H_

#include "MIMXRT1052.h"

/* Definitions for WM8960 */
#define WM8960_SAI            (SAI1)

#define WM8960_BOARD_I2C_INSTANCE      (BOARD_CODEC_I2C_INSTANCE)
#define WM8960_BOARD_I2C_CLK_FREQ      (BOARD_CODEC_I2C_CLOCK_FREQ)

#define WM8960_SAI_CLK_FREQ            (12288000U)
#define WM8960_SAI_SAMPLE_RATE         (kWM8960_AudioSampleRate48KHz)
#define WM8960_SAI_AUDIO_BIT_WIDTH     (kWM8960_AudioBitWidth32bit)

#define WM8960_SAI_MASTER_SLAVE        (kSAI_Master)
#define WM8960_SAI_TX_SYNC_MODE        (kSAI_ModeAsync)
#define WM8960_SAI_RX_SYNC_MODE        (kSAI_ModeSync)

/* SAI definitions */
#define APP_SAI_BASE                (SAI1)
#define APP_SAI_CLKD_ID             (kCLOCK_Sai1)
#define APP_SAI_CLK_FREQ            (WM8960_SAI_CLK_FREQ)
#define APP_SAI_MASTER_SLAVE        (WM8960_SAI_MASTER_SLAVE)
#define APP_SAI_TX_SYNC_MODE        (WM8960_SAI_TX_SYNC_MODE)
#define APP_SAI_RX_SYNC_MODE        (WM8960_SAI_RX_SYNC_MODE)
#define APP_SAI_BIT_CLK_SRC         (kSAI_BclkSourceMclkOption1)
#define APP_SAI_CID                 (CODEC_ID_WM8960)

#define APP_SAI_CHANNEL             (0)

#define APP_SAI_AUDIO_DATA_CHANNEL  (2U)
#define APP_SAI_AUDIO_BIT_WIDTH      kSAI_WordWidth32bits

#define APP_SAI_DATA_MASK_NONE 0

#define APP_SUPPORTED_RATES         {48000, 96000}
#define APP_AUDIO_PLL                393216000

#define CODEC_ID_WM8960 0

#endif /* _APP_BOARD_H_ */
