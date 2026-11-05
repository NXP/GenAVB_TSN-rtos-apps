/*
 * Copyright 2023-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "genavb_sdk_common.h"
#include "fsl_gpt.h"
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

int BOARD_Phy8201_fixup(phy_handle_t *handle)
{
    status_t result;
    uint16_t data;

    result = PHY_Write(handle, 0x1f, 7);
    if (result != kStatus_Success)
    {
        return result;
    }

    result = PHY_Read(handle, 0x10, &data);
    if (result != kStatus_Success)
    {
        return result;
    }

    /* CRS/DV pin is RXDV signal. */
    data |= (1U << 2);
    result = PHY_Write(handle, 0x10, data);
    if (result != kStatus_Success)
    {
        return result;
    }

    result = PHY_Write(handle, 0x1f, 4);
    if (result != kStatus_Success)
    {
        return result;
    }

    /* Disable EEE */
    result = PHY_Write(handle, 0x10, 0);
    if (result != kStatus_Success)
    {
        return result;
    }

    result = PHY_Write(handle, 0x1f, 0);

    return result;
}

uint32_t dev_get_net_core_freq(void *base)
{
    return CLOCK_GetRootClockFreq(kCLOCK_Root_Netc);
}

uint32_t dev_get_net_1588_freq(void *base)
{
    return CLOCK_GetRootClockFreq(kCLOCK_Root_Tmr_1588);
}
