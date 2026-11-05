/*
 * Copyright 2023, 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "genavb_sdk.h"
#include "avb_tsn/common/system_config.h"

unsigned int BOARD_GPT_clk_src(void *base)
{
    return kGPT_ClockSource_Periph;
}

unsigned int BOARD_GPT_clk_freq(void *base)
{
    if (base == GPT1)
        return CLOCK_GetRootClockFreq(kCLOCK_Root_Gpt1);
    else if (base == GPT2)
        return CLOCK_GetRootClockFreq(kCLOCK_Root_Gpt2);
    else
        return 0;
}

int BOARD_NetPort_Get_MAC(unsigned int port, uint8_t *mac)
{
    const struct net_config *net_cfg = system_config_get_net(port);

    if (!net_cfg)
        return -1;

    memcpy(mac, net_cfg->hw_addr, 6);

    return 0;
}

uint32_t dev_get_net_core_freq(void *base)
{
    return CLOCK_GetRootClockFreq(kCLOCK_Root_Netc);
}

uint32_t dev_get_net_1588_freq(void *base)
{
    return CLOCK_GetRootClockFreq(kCLOCK_Root_Tmr_1588);
}
