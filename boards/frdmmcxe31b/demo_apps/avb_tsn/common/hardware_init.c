/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*${header:start}*/
#include "fsl_enet_qos.h"
#include "fsl_siul2.h"
#include "pin_mux.h"
#include "board.h"
/*${header:end}*/

/*${function:start}*/
void BOARD_InitEnetClock(void)
{
    CLOCK_SetEmacRmiiTxClkFreq(50000000U);

    CLOCK_AttachClk(kEMAC_RMII_TX_CLK_to_EMAC_TX);
    CLOCK_AttachClk(kEMAC_RMII_TX_CLK_to_EMAC_RX);
    CLOCK_AttachClk(kEMAC_RMII_TX_CLK_to_EMAC_TS);

    CLOCK_SetClkDiv(kCLOCK_DivEmacRxClk, 2U);
    CLOCK_SetClkDiv(kCLOCK_DivEmacTxClk, 2U);
    CLOCK_SetClkDiv(kCLOCK_DivEmacTsClk, 2U);

    ENET_QOS_EnableClock(true);
}

void ENET_QOS_SetSYSControl(enet_qos_mii_mode_t miiMode)
{
    if (miiMode == kENET_QOS_RmiiMode) {
        DCM_GPR->DCMRWF1 = (DCM_GPR->DCMRWF1 & DCM_GPR_DCMRWF1_RMII_MII_SEL_MASK) | DCM_GPR_DCMRWF1_RMII_MII_SEL(1U);
    } else {
        DCM_GPR->DCMRWF1 = (DCM_GPR->DCMRWF1 & DCM_GPR_DCMRWF1_RMII_MII_SEL_MASK) | DCM_GPR_DCMRWF1_RMII_MII_SEL(0U);
    }
}

void ENET_QOS_EnableClock(bool enable)
{
    if (enable) {
        CLOCK_EnableClock(kCLOCK_Emac);
    } else {
        CLOCK_DisableClock(kCLOCK_Emac);
    }
}

static void BOARD_EnetPhyReset(void)
{
    /* Reset PHY */
    SIUL2_PortPinWrite(BOARD_INITEMACPINS_ENET_PHY_RST_SIUL2_BASE, BOARD_INITEMACPINS_ENET_PHY_RST_GPIO, 3U, 0U);
    SDK_DelayAtLeastUs(25000, CLOCK_GetFreq(kCLOCK_CoreSysClk));
    SIUL2_PortPinWrite(BOARD_INITEMACPINS_ENET_PHY_RST_SIUL2_BASE, BOARD_INITEMACPINS_ENET_PHY_RST_GPIO, 3U, 1U);
}

void BOARD_InitNetInterfaces(void)
{
    BOARD_InitEnetClock();
    BOARD_EnetPhyReset();
}

static void BOARD_InitSTMClock(void)
{
    CLOCK_SetClkDiv(kCLOCK_DivStm0Clk, 1U);
    /* Use AIPS_PLAT_CLK clock for the STM0 */
    CLOCK_AttachClk(kAIPS_PLAT_CLK_to_STM0);
}

void BOARD_InitHardware(void)
{
    /* Hardware Initialization. */
    BOARD_ConfigMPU();
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitEMACPins();
    BOARD_InitLEDsPins();
    BOARD_InitNetInterfaces();
    BOARD_InitSTMClock();
}
/*${function:end}*/