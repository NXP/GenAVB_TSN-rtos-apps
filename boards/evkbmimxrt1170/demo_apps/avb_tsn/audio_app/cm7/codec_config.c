/*
 * Copyright 2021-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "rtos_abstraction_layer.h"

#include "fsl_codec_common.h"
#include "fsl_wm8962.h"
#include "fsl_codec_i2c.h"

#include "app_board.h"
#include "board.h"

#include "avb_tsn/common/codec_config.h"

#include "rtos_apps/audio/audio_app.h"
#include "rtos_apps/log.h"

#define WM8962_THREED1                  0x10CU
#define WM8962_THREED1_ADCMONOMIX_MASK  0x40U
#define WM8962_THREED1_ADCMONOMIX_SHIFT 0x06U

static codec_handle_t wm8962_codec_handle = {0};

static wm8962_config_t wm8962Config = {
    .i2cConfig = {.codecI2CInstance = WM8962_BOARD_I2C_INSTANCE},
    .route =
        {
            .enableLoopBack            = false,
            .leftInputPGASource        = kWM8962_InputPGASourceInput1,
            .leftInputMixerSource      = kWM8962_InputMixerSourceInputPGA,
            .rightInputPGASource       = kWM8962_InputPGASourceInput3,
            .rightInputMixerSource     = kWM8962_InputMixerSourceInputPGA,
            .leftHeadphoneMixerSource  = kWM8962_OutputMixerDisabled,
            .leftHeadphonePGASource    = kWM8962_OutputPGASourceDAC,
            .rightHeadphoneMixerSource = kWM8962_OutputMixerDisabled,
            .rightHeadphonePGASource   = kWM8962_OutputPGASourceDAC,
        },
    .slaveAddress = WM8962_I2C_ADDR,
    .bus          = kWM8962_BusI2S,
    .format       = {.sampleRate = kWM8962_AudioSampleRate48KHz, .bitWidth = kWM8962_AudioBitWidth16bit},
    .fllClock =
        {
            .fllClockSource        = kWM8962_FLLClkSourceMCLK,
            .fllReferenceClockFreq = WM8962_SAI_CLK_FREQ,
            .fllOutputFreq         = WM8962_SAI_CLK_FREQ,
        },
    .sysclkSource = kWM8962_SysClkSourceMclk,
    .masterSlave  = false, /* sai use as master mode, so codec as slave mode */
};

static codec_config_t wm8962_codec_config = {
    .codecDevType = kCODEC_WM8962,
    .codecDevConfig = &wm8962Config
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

    if (codec_id == CODEC_ID_WM8962) {
        err = CODEC_SetFormat(&wm8962_codec_handle, mclk, sample_rate, bitwidth);
        if (err != kStatus_Success) {
            log_warn("WM8962 set format failed: sample rate %d not supported (err %d)\n",sample_rate, err);
            goto end;
        }
    }
    else {
        err = -1;
        rtos_assert(0, "Unexpected codec id (%d)", codec_id);
    }

end:
    return err;
}

int32_t audio_app_codec_setup(uint8_t codec_id)
{
    int32_t err = 0;

    wm8962Config.i2cConfig.codecI2CSourceClock = WM8962_BOARD_I2C_CLK_FREQ;
    wm8962Config.format.mclk_HZ                = WM8962_SAI_CLK_FREQ;

    if (codec_id == CODEC_ID_WM8962) {
        wm8962_handle_t *devHandle;

        /* Use default setting to init codec */
        err = CODEC_Init(&wm8962_codec_handle, &wm8962_codec_config);
        if ((err != kStatus_Success) && (err != kStatus_CODEC_NotSupport)) {
            log_err("WM8962 initialisation failed (err %d)\n", err);
            goto end;
        }
        err = kStatus_Success;

        /* To have a stereo signal from a mono channel microphone on the EVK:
        * Enable ADC Monomix to mix both Right and Left ADC signals
        */
        devHandle = (wm8962_handle_t *)((uintptr_t)(((codec_handle_t *)&wm8962_codec_handle)->codecDevHandle));

        err = WM8962_ModifyReg(devHandle, WM8962_THREED1, WM8962_THREED1_ADCMONOMIX_MASK, 1 << WM8962_THREED1_ADCMONOMIX_SHIFT);
        if (err != kStatus_Success) {
            log_err("WM8962 enable ADC Monomix failed (err %d)\n", err);
            goto end;
        }

        /* Set DAC module volume to the maximum */
        err = CODEC_SetVolume(&wm8962_codec_handle, (kCODEC_VolumeDAC), 100);
        if (err != kStatus_Success) {
            log_err("WM8962 set DAC volume failed (err %d)\n", err);
            goto end;
        }

        /* Set volume on both channels */
        err = CODEC_SetVolume(&wm8962_codec_handle, (kCODEC_VolumeHeadphoneLeft | kCODEC_VolumeHeadphoneRight), 85);
        if (err != kStatus_Success) {
            log_err("WM8962 set Headphone volume failed (err %d)\n", err);
            goto end;
        }
    }
    else {
        err = -1;
        rtos_assert(0, "Unexpected codec id (%d)", codec_id);
    }

end:
    return err;
}

int32_t audio_app_codec_close(uint8_t codec_id)
{
    int32_t err;

    if (codec_id == CODEC_ID_WM8962) {
        err = CODEC_Deinit(&wm8962_codec_handle);
        if (err != kStatus_Success) {
            log_err("WM8962 deinitialisation failed (err %d)\n", err);
            goto end;
        }
    }
    else {
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
