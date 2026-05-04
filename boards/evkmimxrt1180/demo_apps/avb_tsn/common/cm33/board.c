/*
 * Copyright 2021-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_common.h"
#include "board.h"
#include "genavb_sdk.h"
#include "pin_mux.h"
#include "flexspi_hyperram.h"
#if defined(SDK_I2C_BASED_COMPONENT_USED) && SDK_I2C_BASED_COMPONENT_USED
#include "fsl_lpi2c.h"
#endif /* SDK_I2C_BASED_COMPONENT_USED */
#include "fsl_cache.h"
#include "fsl_netc_ierb.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define BOARD_FLEXSPI_DLL_LOCK_RETRY (10)

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
 
static void BOARD_NetPhysReset(void)
{
    rgpio_pin_config_t pinConfig = {.pinDirection = kRGPIO_DigitalOutput, .outputLogic = 0};

    /* Reset PHY8211 for ETH1/ETH2/ETH3 (Switch port1/2/3) */

    /* REALTEK_RTL8211FD DataSheet v1.4
    - Power-on: v3.3 setup should not exceed 100ms
    - Reset: For a complete PHY reset, this pin must be asserted low for at least 10ms for the internal regulator.
             Wait for at least 50ms (for internal circuits settling time) before accessing the PHY register.
    - Toggling Time: Period between consecutive ON/OFF toggling action must be longer than 100ms.
    */

    /* Reset PHY8201 for ETH4(EP), ETH0(Switch port0). Power on 150ms, reset 10ms, wait 150ms. */

    RGPIO_PinInit(RGPIO4, 13, &pinConfig); /* Eth4 (EP), GPIO_AD_13 */
    RGPIO_PinInit(RGPIO4, 25, &pinConfig); /* Eth0 (Switch port0), GPIO_AD_25 */
    RGPIO_PinInit(RGPIO6, 13, &pinConfig); /* Eth1(Switch port1), GPIO_B1_13 */
    RGPIO_PinInit(RGPIO4, 28, &pinConfig); /* Eth2(Switch port2), GPIO_AD_28 */
    RGPIO_PinInit(RGPIO6, 15, &pinConfig); /* Eth3(Switch port3), GPIO_B2_01 */

    SDK_DelayAtLeastUs(10000, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);

    RGPIO_PinWrite(RGPIO4, 13, 1); /* Eth4 (EP), GPIO_AD_13 */
    RGPIO_PinWrite(RGPIO4, 25, 1); /* Eth0 (Switch port0), GPIO_AD_25 */
    RGPIO_PinWrite(RGPIO6, 13, 1); /* Eth1(Switch port1), GPIO_B1_13 */
    RGPIO_PinWrite(RGPIO4, 28, 1); /* Eth2(Switch port2), GPIO_AD_28 */
    RGPIO_PinWrite(RGPIO6, 15, 1); /* Eth3(Switch port3), GPIO_B2_01 */

    SDK_DelayAtLeastUs(150000, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
}

void BOARD_NETC_Init(void)
{
    /* EP and Switch port 0 use RMII interface. */
    NETC_SocSetMiiMode(kNETC_SocLinkEp0, kNETC_RmiiMode);
    NETC_SocSetMiiMode(kNETC_SocLinkSwitchPort0, kNETC_RmiiMode);

    /* Switch port 1~3 use RGMII interface. */
    NETC_SocSetMiiMode(kNETC_SocLinkSwitchPort1, kNETC_RgmiiMode);
    NETC_SocSetMiiMode(kNETC_SocLinkSwitchPort2, kNETC_RgmiiMode);
    NETC_SocSetMiiMode(kNETC_SocLinkSwitchPort3, kNETC_RgmiiMode);

    /* Output reference clock for RMII interface. */
    NETC_SocSetRmiiRefClk(kNETC_SocLinkEp0, true);
    NETC_SocSetRmiiRefClk(kNETC_SocLinkSwitchPort0, true);

    /* Unlock the IERB. It will warm reset whole NETC. */
    if (NETC_IERBUnlock() == kStatus_Success)
    {
        while (!NETC_IERBIsUnlockOver())
        {
        }
    }

    /* Set PHY address in IERB to use MAC port MDIO, otherwise the access will be blocked. */
#if (BOARD_NUM_LINK > 0)
    NETC_SocSetLinkAddr(BOARD_LINK0_PORT, BOARD_LINK0_ADDRESS);
#endif /* (BOARD_NUM_LINK > 0) */
#if (BOARD_NUM_LINK > 1)
    NETC_SocSetLinkAddr(BOARD_LINK1_PORT, BOARD_LINK1_ADDRESS);
#endif /* (BOARD_NUM_LINK > 1) */
#if (BOARD_NUM_LINK > 2)
    NETC_SocSetLinkAddr(BOARD_LINK2_PORT, BOARD_LINK2_ADDRESS);
#endif /* (BOARD_NUM_LINK > 2) */
#if (BOARD_NUM_LINK > 3)
    NETC_SocSetLinkAddr(BOARD_LINK3_PORT, BOARD_LINK3_ADDRESS);
#endif /* (BOARD_NUM_LINK > 3) */
#if (BOARD_NUM_LINK > 4)
    NETC_SocSetLinkAddr(BOARD_LINK4_PORT, BOARD_LINK4_ADDRESS);
#endif /* (BOARD_NUM_LINK > 4) */

    /* Set the access attribute, otherwise MSIX access will be blocked. */
    NETC_IERB->ARRAY_NUM_RC[0].RCMSIAMQR &= ~(7U << 27);
    NETC_IERB->ARRAY_NUM_RC[0].RCMSIAMQR |= (1U << 27);

    /* Lock the IERB. */
    assert(NETC_IERBLock() == kStatus_Success);
    while (!NETC_IERBIsLockOver())
    {
    }

    BOARD_NetPhysReset();
}

void BOARD_DeinitFlash(FLEXSPI_Type *base)
{
#if (__CORTEX_M == 33)
    XCACHE_DisableCache(XCACHE_PC);
#endif

    /* Enable FLEXSPI module */
    base->MCR0 &= ~FLEXSPI_MCR0_MDIS_MASK;

    /* Wait until FLEXSPI is not busy */
    while (!((base->STS0 & FLEXSPI_STS0_ARBIDLE_MASK) && (base->STS0 & FLEXSPI_STS0_SEQIDLE_MASK)))
    {
    }
    /* Disable module during the reset procedure */
    base->MCR0 |= FLEXSPI_MCR0_MDIS_MASK;
}

void BOARD_InitFlash(FLEXSPI_Type *base)
{
    uint32_t status;
    uint32_t lastStatus;
    uint32_t retry;

    /* If serial root clock is >= 100 MHz, DLLEN set to 1, OVRDEN set to 0, then SLVDLYTARGET setting of 0x0 is
     * recommended. */
    base->DLLCR[0] = 0x1U;

    /* Enable FLEXSPI module */
    base->MCR0 &= ~FLEXSPI_MCR0_MDIS_MASK;

    base->MCR0 |= FLEXSPI_MCR0_SWRESET_MASK;
    while (base->MCR0 & FLEXSPI_MCR0_SWRESET_MASK)
    {
    }

    /* Need to wait DLL locked if DLL enabled */
    if (0U != (base->DLLCR[0] & FLEXSPI_DLLCR_DLLEN_MASK))
    {
        lastStatus = base->STS2;
        retry      = BOARD_FLEXSPI_DLL_LOCK_RETRY;
        /* Wait slave delay line locked and slave reference delay line locked. */
        do
        {
            status = base->STS2;
            if ((status & (FLEXSPI_STS2_AREFLOCK_MASK | FLEXSPI_STS2_ASLVLOCK_MASK)) ==
                (FLEXSPI_STS2_AREFLOCK_MASK | FLEXSPI_STS2_ASLVLOCK_MASK))
            {
                /* Locked */
                retry = 100;
                break;
            }
            else if (status == lastStatus)
            {
                /* Same delay cell number in calibration */
                retry--;
            }
            else
            {
                retry      = BOARD_FLEXSPI_DLL_LOCK_RETRY;
                lastStatus = status;
            }
        } while (retry > 0);
        /* According to ERR011377, need to delay at least 100 NOPs to ensure the DLL is locked. */
        for (; retry > 0U; retry--)
        {
            __NOP();
        }
    }

#if (__CORTEX_M == 33)
    XCACHE_EnableCache(XCACHE_PC);
#endif
}

/* BOARD_SetFlexspiClock run in RAM used to configure FlexSPI clock source and divider when XIP. */
void BOARD_SetFlexspiClock(FLEXSPI_Type *base, uint8_t src, uint32_t divider)
{
    clock_root_t root;
    clock_lpcg_t lpcg;

    if (base == FLEXSPI1)
    {
        root = kCLOCK_Root_Flexspi1;
        lpcg = kCLOCK_Flexspi1;
    }
    else if (base == FLEXSPI2)
    {
        root = kCLOCK_Root_Flexspi2;
        lpcg = kCLOCK_Flexspi2;
    }
    else
    {
        return;
    }

    if (((CCM->CLOCK_ROOT[root].CONTROL & CCM_CLOCK_ROOT_CONTROL_MUX_MASK) != CCM_CLOCK_ROOT_CONTROL_MUX(src)) ||
        ((CCM->CLOCK_ROOT[root].CONTROL & CCM_CLOCK_ROOT_CONTROL_DIV_MASK) != CCM_CLOCK_ROOT_CONTROL_DIV(divider - 1)))
    {
        /* Always deinit FLEXSPI and init FLEXSPI for the flash to make sure the flash works correctly after the
         FLEXSPI root clock changed as the default FLEXSPI configuration may does not work for the new root clock
         frequency. */
        BOARD_DeinitFlash(base);

        /* Disable clock before changing clock source */
        CCM->LPCG[lpcg].DIRECT &= ~CCM_LPCG_DIRECT_ON_MASK;
        __DSB();
        __ISB();
        while (CCM->LPCG[lpcg].STATUS0 & CCM_LPCG_STATUS0_ON_MASK)
        {
        }

        /* Update flexspi clock. */
        CCM->CLOCK_ROOT[root].CONTROL = CCM_CLOCK_ROOT_CONTROL_MUX(src) | CCM_CLOCK_ROOT_CONTROL_DIV(divider - 1);
        __DSB();
        __ISB();
        (void)CCM->CLOCK_ROOT[root].CONTROL;

        /* Enable FLEXSPI clock again */
        CCM->LPCG[lpcg].DIRECT |= CCM_LPCG_DIRECT_ON_MASK;
        __DSB();
        __ISB();
        while (!(CCM->LPCG[lpcg].STATUS0 & CCM_LPCG_STATUS0_ON_MASK))
        {
        }

        BOARD_InitFlash(base);
    }
}

/* This function is used to change FlexSPI clock to a stable source before clock sources(Such as PLL and Main clock)
 * updating in case XIP(execute code on FLEXSPI memory.) */
void BOARD_FlexspiClockSafeConfig(void)
{
    /* Move FLEXSPI clock source to OSC_RC_24M to avoid instruction/data fetch issue in XIP when updating PLL. */
    BOARD_SetFlexspiClock(FLEXSPI1, 0U, 1U);
}

/*!
 * @brief   void BOARD_Init(void)
 *           - Initialization of the board low level hardware
 *             Called in startup assembly code.
 * @param   void
 *
 * @return  none
 */
/*
 */
void BOARD_Init(void)
{
    BOARD_ConfigMPU();
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitHYPERRAM();
    BOARD_RequestTRDC(true, true, false);
}