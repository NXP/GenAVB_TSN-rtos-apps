/*
 * Copyright 2021-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "rtos_abstraction_layer.h"

#include "fsl_codec_common.h"
#include "fsl_wm8960.h"
#include "fsl_codec_i2c.h"

#include "app_board.h"
#include "board.h"
#include "avb_tsn/common/codec_config.h"

#include "rtos_apps/audio/audio_app.h"
#include "rtos_apps/log.h"

/* Additional Control 1 */
#define VSEL                3 /* Analog Bias optimisation */
#define VSEL_BITS           (VSEL << 6)
/* ADC Data Output Select:
 * 0 left data = left ADC; right data = right ADC
 * 1: left data = left ADC; right data = left ADC
 * 2: left data = right ADC; right data = right ADC
 * 3: left data = right ADC; right data = left ADC 
 */
#define DATASEL             2
#define DATASEL_BITS        (DATASEL << 2)
#define DMONOMIX_BIT        (1 << 4) /* DAC Mono Mix */

static codec_handle_t wm8960_codec_handle = {0};

static wm8960_config_t wm8960Config = {
    .i2cConfig = {
        .codecI2CInstance = WM8960_BOARD_I2C_INSTANCE
    },
    .route = kWM8960_RoutePlaybackandRecord,
    .leftInputSource = kWM8960_InputClosed,
    .rightInputSource = kWM8960_InputDifferentialMicInput2,
    .playSource = kWM8960_PlaySourceDAC,
    .slaveAddress = WM8960_I2C_ADDR,
    .bus = kWM8960_BusI2S,
    .format = {
        .mclk_HZ = WM8960_SAI_CLK_FREQ,
        .sampleRate = WM8960_SAI_SAMPLE_RATE,
        .bitWidth = WM8960_SAI_AUDIO_BIT_WIDTH
    },
    .master_slave = false,
};

static codec_config_t wm8960_codec_config = {
    .codecDevType = kCODEC_WM8960,
    .codecDevConfig = &wm8960Config
    };

static bool is_value_in_array(uint32_t value, uint32_t array[], size_t size)
{
    for (int i = 0; i < size; i++) {
        if (array[i] == value)
            return true;
    }

    return false;
}

int32_t audio_app_codec_set_format(uint8_t codec_id, uint32_t mclk, uint32_t sample_rate, uint32_t bitwidth)
{
    int32_t err;

    if (codec_id == CODEC_ID_WM8960) {
        err = CODEC_SetFormat(&wm8960_codec_handle, mclk, sample_rate, bitwidth);
        if (err != kStatus_Success) {
            log_warn("WM8960 set format failed: sample rate %d not supported (err %d)\n", sample_rate, err);
            goto end;
        }
    } else {
        err = -1;
        rtos_assert(0, "Unexpected codec id (%d)", codec_id);
    }

end:
    return err;
}

int32_t audio_app_codec_setup(uint8_t codec_id)
{
    int32_t err = 0;

    wm8960Config.i2cConfig.codecI2CSourceClock = WM8960_BOARD_I2C_CLK_FREQ;

    if (codec_id == CODEC_ID_WM8960) {
        wm8960_handle_t *devHandle;

        /* Use default setting to init codec */
        err = CODEC_Init(&wm8960_codec_handle, &wm8960_codec_config);
        if ((err != kStatus_Success) && (err != kStatus_CODEC_NotSupport)) {
            log_err("WM8960 initialisation failed (err %d)\n", err);
            goto end;
        }

        err = CODEC_SetVolume(&wm8960_codec_handle, (kCODEC_VolumeDAC), 100);
        if (err != kStatus_Success) {
            log_err("WM8960 set DAC volume failed (err %d)\n", err);
            goto end;
        }

        /* Set volume on both channels */
        err = CODEC_SetVolume(&wm8960_codec_handle, (kCODEC_PlayChannelHeadphoneLeft | kCODEC_PlayChannelHeadphoneRight), 95);
        if (err != kStatus_Success) {
            log_err("WM8960 set Headphone volume failed (err %d)\n", err);
            goto end;
        }

        /* Setup ADC Data Output Select */
        devHandle = (wm8960_handle_t *)((uintptr_t)(((codec_handle_t *)&wm8960_codec_handle)->codecDevHandle));
        err = WM8960_ModifyReg(devHandle, WM8960_ADDCTL1, 0x0C, DATASEL_BITS);
        if (err != kStatus_Success) {
            log_err("WM8960 set ADC Data Output Select failed (err %d)\n", err);
            goto end;
        }

        err = kStatus_Success;
    } else {
        err = -1;
        rtos_assert(0, "Unexpected codec id (%d)", codec_id);
    }

end:
    return err;
}

int32_t audio_app_codec_close(uint8_t codec_id)
{
    int32_t err;

    if (codec_id == CODEC_ID_WM8960) {
        err = CODEC_Deinit(&wm8960_codec_handle);
        if (err != kStatus_Success) {
            log_err("WM8960 deinitialisation failed (err %d)\n", err);
            goto end;
        }
    } else {
        err = -1;
        rtos_assert(0, "Unexpected codec id (%d)", codec_id);
    }

end:
    return err;
}

bool BOARD_codec_is_rate_supported(uint32_t rate)
{
    uint32_t supported_rates[] = APP_SUPPORTED_RATES;

    return is_value_in_array(rate, supported_rates, ARRAY_SIZE(supported_rates));
}
