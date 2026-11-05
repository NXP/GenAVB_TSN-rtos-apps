/*
 * Copyright 2024-2025 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>
#include "fsl_common.h"
#include "fsl_debug_console.h"
#include "board.h"
#include "genavb_sdk.h"
#if defined(SDK_I2C_BASED_COMPONENT_USED) && SDK_I2C_BASED_COMPONENT_USED
#include "fsl_lpi2c.h"
#endif /* SDK_I2C_BASED_COMPONENT_USED */
#include "fsl_lpuart.h"
#include "FreeRTOSConfig.h"
#include "avb_tsn/common/uart.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

/* Get debug console frequency. */
uint32_t BOARD_DebugConsoleSrcFreq(void)
{
    return BOARD_DEBUG_UART_CLK_FREQ;
}

/* Initialize debug console. */
void BOARD_InitDebugConsole(void)
{
    CLOCK_SetIpSrc(kCLOCK_Lpuart2, BOARD_DEBUG_UART_CLKSRC);

    uart_init();
}

void BOARD_UART_ISR(void)
{
    LPUART_TransferHandleIRQ(BOARD_DEBUG_UART_BASEADDR, s_lpuartHandle[BOARD_DEBUG_UART_INSTANCE]);
}


#if defined(SDK_I2C_BASED_COMPONENT_USED) && SDK_I2C_BASED_COMPONENT_USED
void BOARD_LPI2C_Init(LPI2C_Type *base, uint32_t clkSrc_Hz)
{
    lpi2c_master_config_t lpi2cConfig = {0};

    /*
     * lpi2cConfig.debugEnable = false;
     * lpi2cConfig.ignoreAck = false;
     * lpi2cConfig.pinConfig = kLPI2C_2PinOpenDrain;
     * lpi2cConfig.baudRate_Hz = 100000U;
     * lpi2cConfig.busIdleTimeout_ns = 0;
     * lpi2cConfig.pinLowTimeout_ns = 0;
     * lpi2cConfig.sdaGlitchFilterWidth_ns = 0;
     * lpi2cConfig.sclGlitchFilterWidth_ns = 0;
     */
    LPI2C_MasterGetDefaultConfig(&lpi2cConfig);
    LPI2C_MasterInit(base, &lpi2cConfig, clkSrc_Hz);
}

status_t BOARD_LPI2C_Send(LPI2C_Type *base,
                          uint8_t deviceAddress,
                          uint32_t subAddress,
                          uint8_t subAddressSize,
                          uint8_t *txBuff,
                          uint8_t txBuffSize)
{
    lpi2c_master_transfer_t xfer;

    xfer.flags          = kLPI2C_TransferDefaultFlag;
    xfer.slaveAddress   = deviceAddress;
    xfer.direction      = kLPI2C_Write;
    xfer.subaddress     = subAddress;
    xfer.subaddressSize = subAddressSize;
    xfer.data           = txBuff;
    xfer.dataSize       = txBuffSize;

    return LPI2C_MasterTransferBlocking(base, &xfer);
}

status_t BOARD_LPI2C_Receive(LPI2C_Type *base,
                             uint8_t deviceAddress,
                             uint32_t subAddress,
                             uint8_t subAddressSize,
                             uint8_t *rxBuff,
                             uint8_t rxBuffSize)
{
    lpi2c_master_transfer_t xfer;

    xfer.flags          = kLPI2C_TransferDefaultFlag;
    xfer.slaveAddress   = deviceAddress;
    xfer.direction      = kLPI2C_Read;
    xfer.subaddress     = subAddress;
    xfer.subaddressSize = subAddressSize;
    xfer.data           = rxBuff;
    xfer.dataSize       = rxBuffSize;

    return LPI2C_MasterTransferBlocking(base, &xfer);
}

void BOARD_Accel_I2C_Init(void)
{
    BOARD_LPI2C_Init(BOARD_ACCEL_I2C_BASEADDR, BOARD_ACCEL_I2C_CLOCK_FREQ);
}

status_t BOARD_Accel_I2C_Send(uint8_t deviceAddress, uint32_t subAddress, uint8_t subaddressSize, uint32_t txBuff)
{
    uint8_t data = (uint8_t)txBuff;

    return BOARD_LPI2C_Send(BOARD_ACCEL_I2C_BASEADDR, deviceAddress, subAddress, subaddressSize, &data, 1U);
}

status_t BOARD_Accel_I2C_Receive(
    uint8_t deviceAddress, uint32_t subAddress, uint8_t subaddressSize, uint8_t *rxBuff, uint8_t rxBuffSize)
{
    return BOARD_LPI2C_Receive(BOARD_ACCEL_I2C_BASEADDR, deviceAddress, subAddress, subaddressSize, rxBuff, rxBuffSize);
}

void BOARD_MagSwitch_I2C_Init(void)
{
    BOARD_LPI2C_Init(BOARD_MAGSWITCH_I2C_BASEADDR, BOARD_MAGSWITCH_I2C_CLOCK_FREQ);
}

status_t BOARD_MagSwitch_I2C_Send(uint8_t deviceAddress, uint32_t subAddress, uint8_t subaddressSize, uint8_t data)
{
    return BOARD_LPI2C_Send(BOARD_MAGSWITCH_I2C_BASEADDR, deviceAddress, subAddress, subaddressSize, &data, 1U);
}

status_t BOARD_MagSwitch_I2C_Receive(
    uint8_t deviceAddress, uint32_t subAddress, uint8_t subaddressSize, uint8_t *rxBuff, uint8_t rxBuffSize)
{
    return BOARD_LPI2C_Receive(BOARD_MAGSWITCH_I2C_BASEADDR, deviceAddress, subAddress, subaddressSize, rxBuff, rxBuffSize);
}

#endif /* SDK_I2C_BASED_COMPONENT_USED */

static volatile uint32_t ulElapsedTime;
static volatile uint16_t usLastCounterValue;

void BOARD_ConfigureForRunTimeStats(void)
{
    FTM_Type *base = BOARD_FTM_STATS;
    ftm_config_t ftmConfig;

    FTM_GetDefaultConfig(&ftmConfig);

    FTM_Init(base, &ftmConfig);

    base->MODE &= ~FTM_MODE_FTMEN_MASK;

    FTM_SetTimerPeriod(base, 0xFFFF);

    FTM_StartTimer(base, kFTM_FixedClock);

    ulElapsedTime = 0;
    usLastCounterValue = 0;
}

uint32_t BOARD_GetRunTimeCounterValue()
{
    uint16_t usCurrentValue;
    uint16_t usDelta;

    usCurrentValue = (uint16_t)FTM_GetCurrentTimerCount(BOARD_FTM_STATS);

    usDelta = usCurrentValue - usLastCounterValue;

    ulElapsedTime += usDelta;
    usLastCounterValue = usCurrentValue;

    return ulElapsedTime;
}

void BOARD_InitNVIC(void)
{
    NVIC_SetPriority(ENET_Transmit_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 2);
    NVIC_SetPriority(ENET_1588_Timer_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1);
    NVIC_SetPriority(LPUART2_IRQn, configLIBRARY_LOWEST_INTERRUPT_PRIORITY);
#ifdef BOARD_FTM_0_CH01_IRQ
    NVIC_SetPriority(BOARD_FTM_0_CH01_IRQ, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 3);
#endif
#ifdef BOARD_FTM_0_CH23_IRQ
    NVIC_SetPriority(BOARD_FTM_0_CH23_IRQ, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 3);
#endif
#ifdef BOARD_FTM_0_CH45_IRQ
    NVIC_SetPriority(BOARD_FTM_0_CH45_IRQ, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 3);
#endif
#ifdef BOARD_FTM_0_CH67_IRQ
    NVIC_SetPriority(BOARD_FTM_0_CH67_IRQ, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 3);
#endif
}
