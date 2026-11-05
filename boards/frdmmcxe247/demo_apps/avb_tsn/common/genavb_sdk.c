/*
 * Copyright 2023, 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_common.h"
#include "fsl_ftm.h"
#include "board.h"
#include "genavb_sdk.h"
#include "avb_tsn/common/system_config.h"

unsigned int BOARD_FTM_clk_freq(void *base)
{
    return CLOCK_GetFreq(kCLOCK_Lpo32kClk);
}

unsigned int BOARD_FTM_clk_src(void *base)
{
    return kFTM_FixedClock;
}

/*
 * Enet module frequency
 */
uint32_t dev_get_enet_core_freq(void *base)
{
    return CLOCK_GetCoreSysClkFreq();
}

/*
 * Enet 1588 timer frequency (ipg_clk_time)
 */
uint32_t dev_get_enet_1588_freq(void *base)
{
    return CLOCK_GetIpFreq(kCLOCK_Enet);
}

int BOARD_NetPort_Get_MAC(unsigned int port, uint8_t *mac)
{
    const struct net_config *net_cfg = system_config_get_net(port);

    if (!net_cfg)
        return -1;

    memcpy(mac, net_cfg->hw_addr, 6);

    return 0;
}
