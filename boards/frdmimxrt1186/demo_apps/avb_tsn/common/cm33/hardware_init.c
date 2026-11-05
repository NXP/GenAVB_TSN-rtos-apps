/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/*${header:start}*/
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_phyyt8521.h"
#include "fsl_netc_endpoint.h"
#include "flexspi_hyperram.h"
/*${header:end}*/

/*${macro:start}*/
/*!< PHY reset pins. */
#define EXAMPLE_SWT_PORT0_PHY_RESET_PIN RGPIO1, 15
#define EXAMPLE_SWT_PORT2_PHY_RESET_PIN RGPIO1, 20

#define PHY_EXT_ADDR_REG             0x1EU
#define PHY_EXT_DATA_REG             0x1FU
#define PHY_RGMII_CONFIG1_REG        0xA003U
#define PHY_RGMII_CONFIG1_TXDLY_MASK 0xFU
/*${macro:end}*/

static void BOARD_NetPhysReset(void)
{
    /* Reset all PHYs even some are not used in case unstable status has effect on other PHYs. */
    /* Reset PHY8201 for ETH4(EP), ETH0(Switch port0). Power on 150ms, reset 10ms, wait 150ms. */
    /* Reset PHY8211 for ETH1(Switch port1), ETH2(Switch port2), ETH3(Switch port3). Reset 10ms, wait 30ms. */
    RGPIO_PinWrite(EXAMPLE_SWT_PORT0_PHY_RESET_PIN, 0);
    RGPIO_PinWrite(EXAMPLE_SWT_PORT2_PHY_RESET_PIN, 0);
    SDK_DelayAtLeastUs(10000, CLOCK_GetFreq(kCLOCK_CpuClk));
    RGPIO_PinWrite(EXAMPLE_SWT_PORT0_PHY_RESET_PIN, 1);
    RGPIO_PinWrite(EXAMPLE_SWT_PORT2_PHY_RESET_PIN, 1);
    SDK_DelayAtLeastUs(150000, CLOCK_GetFreq(kCLOCK_CpuClk));

}

/*${function:start}*/
void BOARD_Init(void)
{
    BOARD_ConfigMPU();
    BOARD_InitBootPins();
    BOARD_InitFLASHPins();
    BOARD_InitBootClocks();
    BOARD_InitHYPERRAMPins();
    BOARD_InitHYPERRAM();
    BOARD_InitNETPins();
    BOARD_NetPhysReset();
    BOARD_RequestTRDC(true, true, false);
}
/*${function:end}*/
