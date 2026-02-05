/*
 * Copyright 2021-2025 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_debug_console.h"
#include "board.h"
#if defined(SDK_I2C_BASED_COMPONENT_USED) && SDK_I2C_BASED_COMPONENT_USED
#include "fsl_lpi2c.h"
#endif /* SDK_I2C_BASED_COMPONENT_USED */
#include "fsl_cache.h"
#include "FreeRTOSConfig.h"
#include "fsl_ele_base_api.h"
#include "fsl_dcdc.h"
#include "avb_tsn/common/uart.h"
#include "fsl_lpuart.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

/* Get debug console frequency. */
uint32_t BOARD_DebugConsoleSrcFreq(void)
{
    return CLOCK_GetRootClockFreq(BOARD_DEBUG_UART_CLK_ROOT);
}

/* Initialize debug console. */
void BOARD_InitDebugConsole(void)
{
    uart_init();
}

void BOARD_UART_ISR(void)
{
    LPUART_TransferHandleIRQ(BOARD_DEBUG_UART_BASEADDR, s_lpuartHandle[BOARD_DEBUG_UART_INSTANCE]);
}

void BOARD_InitDebug(void)
{
    BLK_CTRL_NS_AONMIX_Type *aon = BLK_CTRL_NS_AONMIX;

    aon->IPG_DEBUG |= BLK_CTRL_NS_AONMIX_IPG_DEBUG_M33_GPT1_MASK;
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

status_t BOARD_LPI2C_SendSCCB(LPI2C_Type *base,
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

status_t BOARD_LPI2C_ReceiveSCCB(LPI2C_Type *base,
                                 uint8_t deviceAddress,
                                 uint32_t subAddress,
                                 uint8_t subAddressSize,
                                 uint8_t *rxBuff,
                                 uint8_t rxBuffSize)
{
    status_t status;
    lpi2c_master_transfer_t xfer;

    xfer.flags          = kLPI2C_TransferDefaultFlag;
    xfer.slaveAddress   = deviceAddress;
    xfer.direction      = kLPI2C_Write;
    xfer.subaddress     = subAddress;
    xfer.subaddressSize = subAddressSize;
    xfer.data           = NULL;
    xfer.dataSize       = 0;

    status = LPI2C_MasterTransferBlocking(base, &xfer);

    if (kStatus_Success == status)
    {
        xfer.subaddressSize = 0;
        xfer.direction      = kLPI2C_Read;
        xfer.data           = rxBuff;
        xfer.dataSize       = rxBuffSize;

        status = LPI2C_MasterTransferBlocking(base, &xfer);
    }

    return status;
}

void BOARD_Accel_I2C_Init(void)
{
    BOARD_LPI2C_Init(BOARD_ACCEL_I2C_BASEADDR, BOARD_ACCEL_I2C_CLOCK_FREQ);
}

status_t BOARD_Accel_I2C_Send(uint8_t deviceAddress, uint32_t subAddress, uint8_t subaddressSize, uint32_t txBuff)
{
    uint8_t data = (uint8_t)txBuff;

    return BOARD_LPI2C_Send(BOARD_ACCEL_I2C_BASEADDR, deviceAddress, subAddress, subaddressSize, &data, 1);
}

status_t BOARD_Accel_I2C_Receive(
    uint8_t deviceAddress, uint32_t subAddress, uint8_t subaddressSize, uint8_t *rxBuff, uint8_t rxBuffSize)
{
    return BOARD_LPI2C_Receive(BOARD_ACCEL_I2C_BASEADDR, deviceAddress, subAddress, subaddressSize, rxBuff, rxBuffSize);
}

void BOARD_Codec_I2C_Init(void)
{
    BOARD_LPI2C_Init(BOARD_CODEC_I2C_BASEADDR, BOARD_CODEC_I2C_CLOCK_FREQ);
}

status_t BOARD_Codec_I2C_Send(
    uint8_t deviceAddress, uint32_t subAddress, uint8_t subAddressSize, const uint8_t *txBuff, uint8_t txBuffSize)
{
    return BOARD_LPI2C_Send(BOARD_CODEC_I2C_BASEADDR, deviceAddress, subAddress, subAddressSize, (uint8_t *)txBuff,
                            txBuffSize);
}

status_t BOARD_Codec_I2C_Receive(
    uint8_t deviceAddress, uint32_t subAddress, uint8_t subAddressSize, uint8_t *rxBuff, uint8_t rxBuffSize)
{
    return BOARD_LPI2C_Receive(BOARD_CODEC_I2C_BASEADDR, deviceAddress, subAddress, subAddressSize, rxBuff, rxBuffSize);
}

void BOARD_Camera_I2C_Init(void)
{
    const clock_root_config_t lpi2cClockConfig = {
        .clockOff = false,
        .mux      = BOARD_CAMERA_I2C_CLOCK_SOURCE,
        .div      = BOARD_CAMERA_I2C_CLOCK_DIVIDER,
    };

    CLOCK_SetRootClock(BOARD_CAMERA_I2C_CLOCK_ROOT, &lpi2cClockConfig);

    BOARD_LPI2C_Init(BOARD_CAMERA_I2C_BASEADDR, CLOCK_GetRootClockFreq(BOARD_CAMERA_I2C_CLOCK_ROOT));
}

status_t BOARD_Camera_I2C_Send(
    uint8_t deviceAddress, uint32_t subAddress, uint8_t subAddressSize, const uint8_t *txBuff, uint8_t txBuffSize)
{
    return BOARD_LPI2C_Send(BOARD_CAMERA_I2C_BASEADDR, deviceAddress, subAddress, subAddressSize, (uint8_t *)txBuff,
                            txBuffSize);
}

status_t BOARD_Camera_I2C_Receive(
    uint8_t deviceAddress, uint32_t subAddress, uint8_t subAddressSize, uint8_t *rxBuff, uint8_t rxBuffSize)
{
    return BOARD_LPI2C_Receive(BOARD_CAMERA_I2C_BASEADDR, deviceAddress, subAddress, subAddressSize, rxBuff,
                               rxBuffSize);
}

status_t BOARD_Camera_I2C_SendSCCB(
    uint8_t deviceAddress, uint32_t subAddress, uint8_t subAddressSize, const uint8_t *txBuff, uint8_t txBuffSize)
{
    return BOARD_LPI2C_SendSCCB(BOARD_CAMERA_I2C_BASEADDR, deviceAddress, subAddress, subAddressSize, (uint8_t *)txBuff,
                                txBuffSize);
}

status_t BOARD_Camera_I2C_ReceiveSCCB(
    uint8_t deviceAddress, uint32_t subAddress, uint8_t subAddressSize, uint8_t *rxBuff, uint8_t rxBuffSize)
{
    return BOARD_LPI2C_ReceiveSCCB(BOARD_CAMERA_I2C_BASEADDR, deviceAddress, subAddress, subAddressSize, rxBuff,
                                   rxBuffSize);
}

void BOARD_MIPIPanelTouch_I2C_Init(void)
{
    const clock_root_config_t lpi2cClockConfig = {
        .clockOff = false,
        .mux      = BOARD_MIPI_PANEL_TOUCH_I2C_CLOCK_SOURCE,
        .div      = BOARD_MIPI_PANEL_TOUCH_I2C_CLOCK_DIVIDER,
    };

    CLOCK_SetRootClock(BOARD_MIPI_PANEL_TOUCH_I2C_CLOCK_ROOT, &lpi2cClockConfig);

    BOARD_LPI2C_Init(BOARD_MIPI_PANEL_TOUCH_I2C_BASEADDR,
                     CLOCK_GetRootClockFreq(BOARD_MIPI_PANEL_TOUCH_I2C_CLOCK_ROOT));
}

status_t BOARD_MIPIPanelTouch_I2C_Send(
    uint8_t deviceAddress, uint32_t subAddress, uint8_t subAddressSize, const uint8_t *txBuff, uint8_t txBuffSize)
{
    return BOARD_LPI2C_Send(BOARD_MIPI_PANEL_TOUCH_I2C_BASEADDR, deviceAddress, subAddress, subAddressSize,
                            (uint8_t *)txBuff, txBuffSize);
}

status_t BOARD_MIPIPanelTouch_I2C_Receive(
    uint8_t deviceAddress, uint32_t subAddress, uint8_t subAddressSize, uint8_t *rxBuff, uint8_t rxBuffSize)
{
    return BOARD_LPI2C_Receive(BOARD_MIPI_PANEL_TOUCH_I2C_BASEADDR, deviceAddress, subAddress, subAddressSize, rxBuff,
                               rxBuffSize);
}
#endif /* SDK_I2C_BASED_COMPONENT_USED */

/* MPU configuration. */
#if __CORTEX_M == 33
void BOARD_ConfigMPU(void)
{
#if defined(__ICCARM__) || defined(__GNUC__)
    extern uint32_t __NCACHE_REGION_START[];
    extern uint32_t __NCACHE_REGION_SIZE[];
    uint32_t nonCacheStart = (uint32_t)__NCACHE_REGION_START;
    uint32_t nonCacheSize  = (uint32_t)__NCACHE_REGION_SIZE;
#endif
    uint8_t attr;

    /* Disable code & system cache */
    XCACHE_DisableCache(XCACHE_PC);
    XCACHE_DisableCache(XCACHE_PS);

    /* Disable MPU */
    ARM_MPU_Disable();

    /* Attr0: device. */
    ARM_MPU_SetMemAttr(0U, ARM_MPU_ATTR(ARM_MPU_ATTR_DEVICE, ARM_MPU_ATTR_DEVICE_nGnRE));

    /* Attr1: non cacheable. */
    ARM_MPU_SetMemAttr(1U, ARM_MPU_ATTR(ARM_MPU_ATTR_NON_CACHEABLE, ARM_MPU_ATTR_NON_CACHEABLE));

    /* Attr2: non transient, write through, read allocate. */
    attr = ARM_MPU_ATTR_MEMORY_(0U, 0U, 1U, 0U);
    ARM_MPU_SetMemAttr(2U, ARM_MPU_ATTR(attr, attr));

    /* Attr3: non transient, write back, read/write allocate. */
    attr = ARM_MPU_ATTR_MEMORY_(0U, 1U, 1U, 1U);
    ARM_MPU_SetMemAttr(3U, ARM_MPU_ATTR(attr, attr));

    /* NOTE:
     *   1. When memory regions overlap, the processor generates a fault if a core access hits the overlapping regions
     */

#ifdef CONFIG_APP_HYPERRAM
    /*
       The default attribute of the background system address map for this address space:
         normal, write through, read allocate, non-shareable, read/write in privilege and non-privilege, executable
    */

    /* Region 0 (FlexSPI2, Hyperram): [0x04000000, 0x047FFFFFF, 8M] */
#if !defined(CACHE_MODE_WRITE_THROUGH)
    /* non-shareable, read/write in privilege and non-privilege, executable. Attr 3 */
    ARM_MPU_SetRegion(0U, ARM_MPU_RBAR(BOARD_HYPERRAM_START_ADDR, ARM_MPU_SH_NON, 0U, 1U, 0U), ARM_MPU_RLAR(BOARD_HYPERRAM_START_ADDR + (BOARD_HYPERRAM_SIZE - 1), 3U));
#endif
#endif

    /* Region 1 (Code TCM): [0x0FFE0000, 0x0FFFFFFF, 128K] */
    /* non-shareable, read/write in privilege and non-privilege, executable. Attr 2 */
    ARM_MPU_SetRegion(1U, ARM_MPU_RBAR(0x0FFE0000, ARM_MPU_SH_NON, 0U, 1U, 0U), ARM_MPU_RLAR(0x0FFFFFFF, 2U));

    /* Region 2 (System TCM): [0x20000000, 0x2001FFFF, 128K] */
    /* non-shareable, read/write in privilege and non-privilege, executable. Attr 3 */
    ARM_MPU_SetRegion(2U, ARM_MPU_RBAR(0x20000000, ARM_MPU_SH_NON, 0U, 1U, 0U), ARM_MPU_RLAR(0x2001FFFF, 3U));

    /* Region 3 (CM7 I/D TCM): [0x203C0000, 0x2043FFFF, 512K] */
    /* non-shareable, read/write in privilege and non-privilege, execute-never. Attr 1 (non cacheable). */
    ARM_MPU_SetRegion(3U, ARM_MPU_RBAR(0x203C0000, ARM_MPU_SH_NON, 0U, 1U, 1U), ARM_MPU_RLAR(0x2043FFFF, 1U));

    /* Region 5 (FlexSPI1, Nor Flash): [0x28000000, 0x28FFFFFF, 16M] */
    /* non-shareable, read only in privilege and non-privileged, executable. Attr 3 */
    ARM_MPU_SetRegion(5U, ARM_MPU_RBAR(BOARD_FLASH_START_ADDR, ARM_MPU_SH_NON, 1U, 1U, 0U), ARM_MPU_RLAR(BOARD_FLASH_START_ADDR + (BOARD_FLASH_SIZE - 1), 3U));

    /* Region 6 (Peripherals): [0x40000000, 0x7FFFFFFF, 1G ] */
    /* non-shareable, read/write in privilege and non-privileged, execute-never. Attr 0 (device). */
    ARM_MPU_SetRegion(6U, ARM_MPU_RBAR(0x40000000, ARM_MPU_SH_NON, 0U, 1U, 1U), ARM_MPU_RLAR(0x7FFFFFFF, 0U));

    /* Region 7 (OCRAM1): [0x20480000, 0x204FFFFF, 512K] */
    /* non-shareable, read/write in privilege and non-privilege, executable. Attr 1 */
    ARM_MPU_SetRegion(7U, ARM_MPU_RBAR(nonCacheStart, ARM_MPU_SH_NON, 0U, 1U, 0U), ARM_MPU_RLAR(nonCacheStart + nonCacheSize - 1, 1U));

    /* Region 8 (OCRAM1): [0x20480000, 0x204FFFFF, 512K] */
    /* non-shareable, read/write in privilege and non-privilege, executable. Attr 3 */
    ARM_MPU_SetRegion(8U, ARM_MPU_RBAR(nonCacheStart + nonCacheSize, ARM_MPU_SH_NON, 0U, 1U, 0U), ARM_MPU_RLAR(0x204FFFFF, 3U));

    /* Region 9 (OCRAM2): [0x20500000, 0x2053FFFF, 256K] */
    /* non-shareable, read/write in privilege and non-privilege, executable. Attr 1 */
    ARM_MPU_SetRegion(9U, ARM_MPU_RBAR(0x20500000, ARM_MPU_SH_NON, 0U, 1U, 0U), ARM_MPU_RLAR(0x2053FFFF, 1U));

    /* Enable MPU */
    ARM_MPU_Enable(MPU_CTRL_ENABLE_Msk);

    /* Enable code & system cache */
    XCACHE_EnableCache(XCACHE_PS);
    XCACHE_EnableCache(XCACHE_PC);
}
#endif

void BOARD_SD_Pin_Config(uint32_t speed, uint32_t strength)
{
}

void BOARD_MMC_Pin_Config(uint32_t speed, uint32_t strength)
{
}

void BOARD_InitNVIC(void)
{
    NVIC_SetPriority(NETC_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 2);
    NVIC_SetPriority(MSGINTR1_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 2);
    NVIC_SetPriority(MSGINTR2_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 2);
    NVIC_SetPriority(MSGINTR3_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 2);
    NVIC_SetPriority(MSGINTR4_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 2);
    NVIC_SetPriority(MSGINTR5_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 2);
    NVIC_SetPriority(MSGINTR6_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 2);
    NVIC_SetPriority(GPT1_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 3);
}

/* This function is used to set EdgeLock clock via safe method */
void EdgeLock_SetClock(uint8_t mux, uint8_t div)
{
    if ((CLOCK_GetRootClockDiv(kCLOCK_Root_Edgelock) != (uint32_t)div) ||
        (CLOCK_GetRootClockMux(kCLOCK_Root_Edgelock) != (uint32_t)mux))
    {
        status_t sts;
        uint32_t ele_clk_mhz;

        clock_root_config_t rootCfg = {
            .div      = div,
            .mux      = mux,
            .clockOff = false,
        };

        do
        {
            sts = ELE_BaseAPI_ClockChangeStart(MU_RT_S3MUA);
        } while (sts != kStatus_Success);

        CLOCK_SetRootClock(kCLOCK_Root_Edgelock, &rootCfg);

        ele_clk_mhz = CLOCK_GetRootClockFreq(kCLOCK_Root_Edgelock) / 1000000UL;
        do
        {
            sts = ELE_BaseAPI_ClockChangeFinish(MU_RT_S3MUA, ele_clk_mhz, 0);
        } while (sts != kStatus_Success);
    }
}

/* This function is used to set DCDC output voltage via safe method */
void DCDC_SetVoltage(uint8_t core, uint8_t targetVoltage)
{
    /*
     * When GDET is enabled, it is required to work with special ELE FW, which
     * support ELE API VOLTAGE_CHANGE_START and VOLTAGE_CHANGE_FINISH, and
     * DCDC voltage setting must be guarded with VOLTAGE_CHANGE_START and
     * VOLTAGE_CHANGE_FINISH.
     *
     * For those ELE FW or ELE ROM, which doesn't support ELE API VOLTAGE_CHANGE_START
     * and VOLTAGE_CHANGE_FINISH, there is no side effect to send such API command,
     * since ELE just responde with ERROR and ingore the API command.
     */
    ELE_BaseAPI_VoltageChangeStart(MU_RT_S3MUA);

    DCDC_SetVDD1P0BuckModeTargetVoltage(DCDC, (dcdc_core_slice_t)core, (dcdc_1P0_target_vol_t)targetVoltage);

    ELE_BaseAPI_VoltageChangeFinish(MU_RT_S3MUA);
}

void BOARD_RequestTRDC(bool bRequestAON, bool bRequestWakeup, bool bReqeustMega)
{
#define ELE_TRDC_AON_ID    0x74
#define ELE_TRDC_WAKEUP_ID 0x78
#define ELE_TRDC_MEGA_ID   0x82
#define ELE_CORE_CM33_ID   0x1
#define ELE_CORE_CM7_ID    0x2

/* Set ELE_STICK_FAIL to 0 when ELE status check is not required,
 * which is useful when debug reset, where the core has already get the
 * TRDC ownership at first time and ELE is not able to release TRDC
 * ownership again for the following TRDC ownership request.
 */
#define ELE_STICK_FAIL  1U

#if ELE_STICK_FAIL
#define ELE_CHECK_FAIL(sts) if( sts != kStatus_Success ) { while(1) {}}
#else
#define ELE_CHECK_FAIL(sts) (void)sts
#endif

#if (__CORTEX_M == 33)
    uint8_t ele_core_id = ELE_CORE_CM33_ID;
#elif (__CORTEX_M == 7)
    uint8_t ele_core_id = ELE_CORE_CM7_ID;
#endif

    uint32_t ele_fw_sts;
    status_t sts;

    /* Get ELE FW status */
    sts = ELE_BaseAPI_GetFwStatus(MU_RT_S3MUA, &ele_fw_sts);
    ELE_CHECK_FAIL(sts);

    if(bRequestAON)
    {
        /* Release TRDC AON to current core */
        sts = ELE_BaseAPI_ReleaseRDC(MU_RT_S3MUA, ELE_TRDC_AON_ID, ele_core_id);
        ELE_CHECK_FAIL(sts);
    }

    /*
     * TRDC MEGA request must be prior to TRDC WAKEUP, as TRDC MEGA access
     * is controlled by the TRDC WAKEUP.
     * note:
     *   If TRDC WAKEUP has been release to one core firstly, then it will fail
     *   to release TRDC MEGA to same/another core.
     */
    if(bReqeustMega)
    {
        /* Release TRDC MEGA to current core */
        sts = ELE_BaseAPI_ReleaseRDC(MU_RT_S3MUA, ELE_TRDC_MEGA_ID, ele_core_id);
        ELE_CHECK_FAIL(sts);
    }

    if(bRequestWakeup)
    {
        /* Release TRDC WAKEUP to current core */
        sts = ELE_BaseAPI_ReleaseRDC(MU_RT_S3MUA, ELE_TRDC_WAKEUP_ID, ele_core_id);
        ELE_CHECK_FAIL(sts);
    }
}
