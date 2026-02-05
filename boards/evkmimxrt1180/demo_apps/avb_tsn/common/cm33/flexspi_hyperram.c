/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017, 2024-2025 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_common.h"
#include "flexspi_hyperram.h"

#ifdef CONFIG_APP_HYPERRAM

#include "fsl_flexspi.h"

/*******************************************************************************
 * Variables
 *****************************************************************************/
#define EXAMPLE_FLEXSPI_HYPERRAM                 FLEXSPI2
#define EXAMPLE_FLEXSPI_HYPERRAM_PORT            kFLEXSPI_PortA1
#define EXAMPLE_FLEXSPI_HYPERRAM_RX_SAMPLE_CLOCK kFLEXSPI_ReadSampleClkExternalInputFromDqsPad

#define HYPERRAM_CMD_LUT_SEQ_IDX_READDATA  0
#define HYPERRAM_CMD_LUT_SEQ_IDX_WRITEDATA 1
#define HYPERRAM_CMD_LUT_SEQ_IDX_READREG   2
#define HYPERRAM_CMD_LUT_SEQ_IDX_WRITEREG  3

#define CUSTOM_LUT_LENGTH 20
#define HYPERRAM_SIZE        0x10000 /* 64Mb/KByte. */

static const uint32_t customLUT[CUSTOM_LUT_LENGTH];

static flexspi_device_config_t deviceconfig = {
    .flexspiRootClk       = 196000000, /* 196MHZ SPI serial clock */
    .isSck2Enabled        = false,
    .flashSize            = HYPERRAM_SIZE,
    .CSIntervalUnit       = kFLEXSPI_CsIntervalUnit1SckCycle,
    .CSInterval           = 2,
    .CSHoldTime           = 0,
    .CSSetupTime          = 1,
    .dataValidTime        = 1,
    .columnspace          = 3,
    .enableWordAddress    = true,
    .AWRSeqIndex          = HYPERRAM_CMD_LUT_SEQ_IDX_WRITEDATA,
    .AWRSeqNumber         = 1,
    .ARDSeqIndex          = HYPERRAM_CMD_LUT_SEQ_IDX_READDATA,
    .ARDSeqNumber         = 1,
    .AHBWriteWaitUnit     = kFLEXSPI_AhbWriteWaitUnit2AhbCycle,
    .AHBWriteWaitInterval = 0,
    .enableWriteMask      = true,
};

/*******************************************************************************
 * Functions
 *****************************************************************************/
static const uint32_t customLUT[CUSTOM_LUT_LENGTH] = {
    /* Read Data */
    [4 * HYPERRAM_CMD_LUT_SEQ_IDX_READDATA] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0xA0, kFLEXSPI_Command_RADDR_DDR, kFLEXSPI_8PAD, 0x18),
    [4 * HYPERRAM_CMD_LUT_SEQ_IDX_READDATA + 1] = FLEXSPI_LUT_SEQ(
        kFLEXSPI_Command_CADDR_DDR, kFLEXSPI_8PAD, 0x10, kFLEXSPI_Command_DUMMY_RWDS_DDR, kFLEXSPI_8PAD, 0x07),
    [4 * HYPERRAM_CMD_LUT_SEQ_IDX_READDATA + 2] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_READ_DDR, kFLEXSPI_8PAD, 0x04, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0x00),

    /* Write Data */
    [4 * HYPERRAM_CMD_LUT_SEQ_IDX_WRITEDATA] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x20, kFLEXSPI_Command_RADDR_DDR, kFLEXSPI_8PAD, 0x18),
    [4 * HYPERRAM_CMD_LUT_SEQ_IDX_WRITEDATA + 1] = FLEXSPI_LUT_SEQ(
        kFLEXSPI_Command_CADDR_DDR, kFLEXSPI_8PAD, 0x10, kFLEXSPI_Command_DUMMY_RWDS_DDR, kFLEXSPI_8PAD, 0x07),
    [4 * HYPERRAM_CMD_LUT_SEQ_IDX_WRITEDATA + 2] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_WRITE_DDR, kFLEXSPI_8PAD, 0x04, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0x00),

    /* Read Register */
    [4 * HYPERRAM_CMD_LUT_SEQ_IDX_READREG] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0xE0, kFLEXSPI_Command_RADDR_DDR, kFLEXSPI_8PAD, 0x18),
    [4 * HYPERRAM_CMD_LUT_SEQ_IDX_READREG + 1] = FLEXSPI_LUT_SEQ(
        kFLEXSPI_Command_CADDR_DDR, kFLEXSPI_8PAD, 0x10, kFLEXSPI_Command_DUMMY_RWDS_DDR, kFLEXSPI_8PAD, 0x07),
    [4 * HYPERRAM_CMD_LUT_SEQ_IDX_READREG + 2] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_READ_DDR, kFLEXSPI_8PAD, 0x04, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0x00),

    /* Write Register */
    [4 * HYPERRAM_CMD_LUT_SEQ_IDX_WRITEREG] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x60, kFLEXSPI_Command_RADDR_DDR, kFLEXSPI_8PAD, 0x18),
    [4 * HYPERRAM_CMD_LUT_SEQ_IDX_WRITEREG + 1] = FLEXSPI_LUT_SEQ(
        kFLEXSPI_Command_CADDR_DDR, kFLEXSPI_8PAD, 0x10, kFLEXSPI_Command_DUMMY_RWDS_DDR, kFLEXSPI_8PAD, 0x07),
    [4 * HYPERRAM_CMD_LUT_SEQ_IDX_WRITEREG + 2] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_WRITE_DDR, kFLEXSPI_8PAD, 0x04, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0x00),
};

status_t BOARD_InitHYPERRAM(void) {

    flexspi_config_t config;

    /* To store custom's LUT table in local. */
    uint32_t tempLUT[CUSTOM_LUT_LENGTH] = {0x00U};

    /* Copy LUT information from flash region into RAM region, because LUT update maybe corrupt read sequence(LUT[0])
     * and load wrong LUT table from FLASH region. */
    memcpy(tempLUT, customLUT, sizeof(tempLUT));

    /* Get FLEXSPI default settings and configure the flexspi. */
    FLEXSPI_GetDefaultConfig(&config);

    /* Init FLEXSPI. */
    config.rxSampleClock               = EXAMPLE_FLEXSPI_HYPERRAM_RX_SAMPLE_CLOCK;
    config.enableSckBDiffOpt           = true;
    config.ahbConfig.enableAHBPrefetch = true;
    config.enableCombination           = true;

    /*Allow AHB read start address do not follow the alignment requirement. */
    config.ahbConfig.enableReadAddressOpt = true;
    config.ahbConfig.enableAHBBufferable  = true;
    config.ahbConfig.enableAHBCachable    = true;

    FLEXSPI_Init(EXAMPLE_FLEXSPI_HYPERRAM, &config);

    /* Configure flash settings according to serial flash feature. */
    FLEXSPI_SetFlashConfig(EXAMPLE_FLEXSPI_HYPERRAM, &deviceconfig, EXAMPLE_FLEXSPI_HYPERRAM_PORT);

    /* Update LUT table. */
    FLEXSPI_UpdateLUT(EXAMPLE_FLEXSPI_HYPERRAM, 0, tempLUT, CUSTOM_LUT_LENGTH);

    /* Do software reset. */
    FLEXSPI_SoftwareReset(EXAMPLE_FLEXSPI_HYPERRAM);

    return kStatus_Success;
}
#else
status_t BOARD_InitHYPERRAM(void)
{
    return kStatus_Success;
}
#endif
