/*
 * Copyright 2023, 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_common.h"
#include "board.h"
#include "genavb_sdk.h"
#include "avb_tsn/common/system_config.h"

/*
 * Primary clock source for all PLLs
 */
uint32_t dev_get_pll_ref_freq(void)
{
    return CLOCK_GetPllPhiClkFreq(0);
}

/*
 * Enet module frequency (ipg_clk)
 */
uint32_t dev_get_enet_core_freq(void *base)
{
    return CLOCK_GetCoreClkFreq();
}

/*
 * Enet 1588 timer frequency (ipg_clk_time)
 */
uint32_t dev_get_enet_1588_freq(void *base)
{
    if (base == ENET_QOS)
        return CLOCK_GetEmacTsClkFreq();
    else
        return 0;
}

unsigned int BOARD_STM_clk_freq(void *base)
{
    if (base == STM_0)
        return CLOCK_GetStmClkFreq(0U);
    else if (base == STM_1)
        return CLOCK_GetStmClkFreq(1U);
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
