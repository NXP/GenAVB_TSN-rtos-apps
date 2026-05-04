/*
 * Copyright 2021-2022, 2025 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/*${header:start}*/
#include "pin_mux.h"
#include "fsl_enet.h"
#include "fsl_gpio.h"
#include "fsl_cache.h"
#include "board.h"
/*${header:end}*/

static void BOARD_InitModuleClock(void)
{
   /* Set IEEE 1588 Ethernet clock source to SPLLDIV1_CLK */
   CLOCK_SetIpSrcDiv(kCLOCK_Enet, kCLOCK_IpSrcSysPllAsync, 1, 0);
}

static void BOARD_EnetPhyReset(void)
{
    GPIO_PinWrite(BOARD_INITENETPINS_PTC3_PIN42_GPIO, BOARD_INITENETPINS_PTC3_PIN42_PIN, 0U);
    SDK_DelayAtLeastUs(25000, CLOCK_GetFreq(kCLOCK_CoreSysClk));
    GPIO_PinWrite(BOARD_INITENETPINS_PTC3_PIN42_GPIO, BOARD_INITENETPINS_PTC3_PIN42_PIN, 1U);
}

void BOARD_InitNetInterfaces(void)
{
    SYSMPU->CESR &= ~SYSMPU_CESR_VLD(1);

    BOARD_InitModuleClock();
    CLOCK_EnableClock(kCLOCK_Enet);

    SIM->MISCTRL0 |= SIM_MISCTRL0_RMII_CLK_OBE_MASK | SIM_MISCTRL0_RMII_CLK_SEL_MASK;

    BOARD_EnetPhyReset();
}

void BOARD_InitHardware(void)
{
    BOARD_InitBootPins();
    BOARD_InitENETPins();
    BOARD_InitBootClocks();
    L1CACHE_EnableCodeCache();
    BOARD_InitDebugConsole();
    BOARD_InitNVIC();
    BOARD_InitNetInterfaces();
}
