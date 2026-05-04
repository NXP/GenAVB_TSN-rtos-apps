/*
 * Copyright 2023, 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "board.h"
#include "genavb_sdk.h"
#include "avb_tsn/common/system_config.h"

unsigned int BOARD_GPT_clk_src(void *base)
{
    if (base == BOARD_GPT_1_BASE)
#ifdef CONFIG_APP_MCR
        return kGPT_ClockSource_Ext;
#else
        return kGPT_ClockSource_Periph;
#endif
    else
        return kGPT_ClockSource_Periph;
}

/* Needed for Media Clock Recovery */
unsigned int BOARD_GPT_clk_src_div(void *base)
{
    /* Based on BOARD_InitMediaClock() configuration and external pin connection */
    if (base == BOARD_GPT_1_BASE)
#ifdef CONFIG_APP_MCR
        return (CLOCK_GetDiv(kCLOCK_Sai1PreDiv) + 1) * (CLOCK_GetDiv(kCLOCK_Sai1Div) + 1);
#else
        return 1;
#endif
    else
        return 1;
}

unsigned int BOARD_GPT_clk_freq(void *base)
{
    if (base == BOARD_GPT_1_BASE)
        return BOARD_SAI_MCLK_HZ;
    else
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

/*
 * Primary clock source for all PLLs
 */
uint32_t dev_get_pll_ref_freq(void)
{
    return CLOCK_GetOscFreq();
}

/*
 * Audio Pll tuning
 */
void dev_write_audio_pll_num(uint32_t num)
{
    CCM_ANALOG->PLL_AUDIO_NUM = CCM_ANALOG_PLL_AUDIO_NUM_A(num);
}

uint32_t dev_read_audio_pll_num(void)
{
    return CCM_ANALOG->PLL_AUDIO_NUM;
}

uint32_t dev_read_audio_pll_denom(void)
{
    return CCM_ANALOG->PLL_AUDIO_DENOM;
}

uint32_t dev_read_audio_pll_post_div(void)
{
    return CCM_ANALOG->PLL_AUDIO & CCM_ANALOG_PLL_AUDIO_DIV_SELECT_MASK;
}

/*
 * Enet module frequency (ipg_clk)
 */
uint32_t dev_get_enet_core_freq(void *base)
{
    return CLOCK_GetFreq(kCLOCK_IpgClk);
}

/*
 * Enet 1588 timer frequency (ipg_clk_time)
 */
uint32_t dev_get_enet_1588_freq(void *base)
{
    return CLOCK_GetFreq(kCLOCK_EnetPll1Clk);
}

/*
 * GPT input frequency (ipg_clk)
 */
uint32_t dev_get_gpt_ipg_freq(void *base)
{
    return CLOCK_GetFreq(kCLOCK_PerClk);
}
