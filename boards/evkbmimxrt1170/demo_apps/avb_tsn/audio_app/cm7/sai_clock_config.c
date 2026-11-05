/*
 * Copyright 2021-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_common.h"
#include "fsl_clock.h"
#include "app_board.h"

#include "rtos_abstraction_layer.h"

#include "rtos_apps/audio/audio_app.h"

void audio_app_sai_clock_setup(void)
{
    int i;

    if (audio_app_sai_active_list_nelems == 0)
        rtos_assert(false, "No SAI enabled!");

    /* Enable SAI clocks */
    for (i = 0; i < audio_app_sai_active_list_nelems; i++) {
        CLOCK_EnableClock(audio_app_sai_active_list[i].clk_id);
    }
}

static uint32_t __get_pll_from_srate(uint32_t srate)
{
    return APP_AUDIO_PLL;
}

uint32_t audio_app_sai_select_audio_pll_mux(unsigned int index, uint32_t srate)
{
    return __get_pll_from_srate(srate);
}

uint32_t audio_app_sai_get_clock_freq(unsigned int index)
{
    return audio_app_sai_active_list[index].clk_freq;
}
