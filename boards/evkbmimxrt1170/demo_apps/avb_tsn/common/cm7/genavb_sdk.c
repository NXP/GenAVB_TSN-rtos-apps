/*
 * Copyright 2023-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_anatop_ai.h"
#include "fsl_common.h"
#include "board.h"
#include "genavb_sdk.h"
#include "avb_tsn/common/system_config.h"

/*
 * 24MHz XTAL oscillator
 * Primary clock source for all PLLs
 */
uint32_t dev_get_pll_ref_freq(void)
{
	return CLOCK_GetFreq(kCLOCK_Osc24M);
}

/*
 * Audio Pll tuning
 */
void dev_write_audio_pll_num(uint32_t num)
{
    ANATOP_AI_Write(kAI_Itf_Audio, 0x20, num);
}

uint32_t dev_read_audio_pll_num(void)
{
    return ANATOP_AI_Read(kAI_Itf_Audio, 0x20);
}

uint32_t dev_read_audio_pll_denom(void)
{
    return ANATOP_AI_Read(kAI_Itf_Audio, 0x30);
}

uint32_t dev_read_audio_pll_post_div(void)
{
    return ANATOP_AI_Read(kAI_Itf_Audio, 0x00) & 0x7f;
}

/*
 * Enet module frequency (ipg_clk)
 */
uint32_t dev_get_enet_core_freq(void *base)
{
    return CLOCK_GetRootClockFreq(kCLOCK_Root_Bus);
}

/*
 * Enet 1588 timer frequency (ipg_clk_time)
 */
uint32_t dev_get_enet_1588_freq(void *base)
{
    if (base == ENET)
        return CLOCK_GetRootClockFreq(kCLOCK_Root_Enet_Timer1);
    else if (base == ENET_1G)
        return CLOCK_GetRootClockFreq(kCLOCK_Root_Enet_Timer2);
    else if (base == ENET_QOS)
        return CLOCK_GetRootClockFreq(kCLOCK_Root_Enet_Timer3);
    else
        return 0;
}

/*
 * GPT input frequency (ipg_clk)
 */
uint32_t dev_get_gpt_ipg_freq(void *base)
{
    if (base == GPT1)
        return CLOCK_GetRootClockFreq(kCLOCK_Root_Gpt1);
    else if (base == GPT2)
        return CLOCK_GetRootClockFreq(kCLOCK_Root_Gpt2);
    else if (base == GPT3)
        return CLOCK_GetRootClockFreq(kCLOCK_Root_Gpt3);
    else if (base == GPT4)
        return CLOCK_GetRootClockFreq(kCLOCK_Root_Gpt4);
    else if (base == GPT5)
        return CLOCK_GetRootClockFreq(kCLOCK_Root_Gpt5);
    else if (base == GPT6)
        return CLOCK_GetRootClockFreq(kCLOCK_Root_Gpt6);
    else
        return 0;
}

unsigned int BOARD_GPT_clk_src(void *base)
{
    return kGPT_ClockSource_Periph;
}

/* Needed for Media clock recovery. */
unsigned int BOARD_GPT_clk_src_div(void *base)
{
    if (base == GPT1)
        return CLOCK_GetRootClockDiv(kCLOCK_Root_Gpt1);
    else if (base == GPT2)
        return CLOCK_GetRootClockDiv(kCLOCK_Root_Gpt2);
    else if (base == GPT3)
        return CLOCK_GetRootClockDiv(kCLOCK_Root_Gpt3);
    else if (base == GPT4)
        return CLOCK_GetRootClockDiv(kCLOCK_Root_Gpt4);
    else if (base == GPT5)
        return CLOCK_GetRootClockDiv(kCLOCK_Root_Gpt5);
    else if (base == GPT6)
        return CLOCK_GetRootClockDiv(kCLOCK_Root_Gpt6);
    else
        return 1;
}

unsigned int BOARD_GPT_clk_freq(void *base)
{
    return dev_get_gpt_ipg_freq(base);
}

int BOARD_NetPort_Get_MAC(unsigned int port, uint8_t *mac)
{
    const struct net_config *net_cfg = system_config_get_net(port);

    if (!net_cfg)
        return -1;

    memcpy(mac, net_cfg->hw_addr, 6);

    return 0;
}
