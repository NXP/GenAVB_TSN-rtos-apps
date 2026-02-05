/*
 * Copyright 2016 Freescale
 * Copyright 2016-2022, 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "flexspi_ops.h"

static status_t flexspi_nor_write_enable(FLEXSPI_Type *base, uint32_t baseAddr)
{
    flexspi_transfer_t flashXfer;
    status_t status;

    /* Write neable */
    flashXfer.deviceAddress = baseAddr;
    flashXfer.port          = EXAMPLE_FLEXSPI_PORT;
    flashXfer.cmdType       = kFLEXSPI_Command;
    flashXfer.SeqNumber     = 1;
    flashXfer.seqIndex      = NOR_CMD_LUT_SEQ_IDX_WRITEENABLE;

    status = FLEXSPI_TransferBlocking(base, &flashXfer);

    return status;
}

static status_t flexspi_nor_wait_bus_busy(FLEXSPI_Type *base)
{
    /* Wait status ready. */
    bool isBusy;
    uint32_t readValue;
    status_t status;
    flexspi_transfer_t flashXfer;

    flashXfer.deviceAddress = 0;
    flashXfer.port          = EXAMPLE_FLEXSPI_PORT;
    flashXfer.cmdType       = kFLEXSPI_Read;
    flashXfer.SeqNumber     = 1;
    flashXfer.seqIndex      = NOR_CMD_LUT_SEQ_IDX_READSTATUSREG;
    flashXfer.data          = &readValue;
    flashXfer.dataSize      = 1;

    do
    {
        status = FLEXSPI_TransferBlocking(base, &flashXfer);

        if (status != kStatus_Success)
        {
            return status;
        }
        if (FLASH_BUSY_STATUS_POL)
        {
            if (readValue & (1U << FLASH_BUSY_STATUS_OFFSET))
            {
                isBusy = true;
            }
            else
            {
                isBusy = false;
            }
        }
        else
        {
            if (readValue & (1U << FLASH_BUSY_STATUS_OFFSET))
            {
                isBusy = false;
            }
            else
            {
                isBusy = true;
            }
        }

    } while (isBusy);

    return status;
}

status_t flexspi_nor_enable_quad_mode(FLEXSPI_Type *base)
{
#if defined(FLASH_QUAD_ENABLE) && FLASH_QUAD_ENABLE
    flexspi_transfer_t flashXfer;
    status_t status;
    uint32_t writeValue = FLASH_QUAD_ENABLE;

    /* Write neable */
    status = flexspi_nor_write_enable(base, 0);

    if (status != kStatus_Success)
    {
        return status;
    }

    /* Enable quad mode. */
    flashXfer.deviceAddress = 0;
    flashXfer.port          = EXAMPLE_FLEXSPI_PORT;
    flashXfer.cmdType       = kFLEXSPI_Write;
    flashXfer.SeqNumber     = 1;
    flashXfer.seqIndex      = NOR_CMD_LUT_SEQ_IDX_WRITESTATUSREG;
    flashXfer.data          = &writeValue;
    flashXfer.dataSize      = writeValue <= 0xFFU ? 1 : 2;

    status = FLEXSPI_TransferBlocking(base, &flashXfer);
    if (status != kStatus_Success)
    {
        return status;
    }

    status = flexspi_nor_wait_bus_busy(base);

    return status;
#else
    return kStatus_Success;
#endif
}

static status_t __flexspi_erase_sector(FLEXSPI_Type *base, uint32_t address)
{
    status_t status;
    flexspi_transfer_t flashXfer;

    /* Write enable */
    status = flexspi_nor_write_enable(base, address);

    if (status != kStatus_Success)
    {
        return status;
    }

    flashXfer.deviceAddress = address;
    flashXfer.port          = EXAMPLE_FLEXSPI_PORT;
    flashXfer.cmdType       = kFLEXSPI_Command;
    flashXfer.SeqNumber     = 1;
    flashXfer.seqIndex      = NOR_CMD_LUT_SEQ_IDX_ERASESECTOR;
    status                  = FLEXSPI_TransferBlocking(base, &flashXfer);

    if (status != kStatus_Success)
    {
        return status;
    }

    status = flexspi_nor_wait_bus_busy(base);

    return status;
}

status_t flexspi_erase_sector(FLEXSPI_Type *base, uint32_t address)
{
    uint32_t primask;
    status_t status;

    primask = DisableGlobalIRQ();
    status = __flexspi_erase_sector(base, address);
    EnableGlobalIRQ(primask);

    return status;
}

static status_t __flexspi_page_program(FLEXSPI_Type *base, uint32_t address, const uint32_t *src)
{
    status_t status;
    flexspi_transfer_t flashXfer;

    /* Write neable */
    status = flexspi_nor_write_enable(base, address);

    if (status != kStatus_Success)
    {
        return status;
    }

    /* Prepare page program command */
    flashXfer.deviceAddress = address;
    flashXfer.port          = EXAMPLE_FLEXSPI_PORT;
    flashXfer.cmdType       = kFLEXSPI_Write;
    flashXfer.SeqNumber     = 1;
    flashXfer.seqIndex      = NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_QUAD;
    flashXfer.data          = (uint32_t *)src;
    flashXfer.dataSize      = FLASH_PAGE_SIZE;
    status                  = FLEXSPI_TransferBlocking(base, &flashXfer);

    if (status != kStatus_Success)
    {
        return status;
    }

    status = flexspi_nor_wait_bus_busy(base);

    return status;
}

status_t flexspi_page_program(FLEXSPI_Type *base, uint32_t address, const uint32_t *src)
{
    uint32_t primask;
    status_t status;

    primask = DisableGlobalIRQ();
    status = __flexspi_page_program(base, address, src);
    EnableGlobalIRQ(primask);

    return status;
}

static int __flexspi_init(void)
{
    flexspi_config_t config;
    status_t status;

    flexspi_clock_init();

    /* Get FLEXSPI default settings and configure the flexspi. */
    FLEXSPI_GetDefaultConfig(&config);

    /* Set AHB buffer size for reading data through AHB bus. */
    config.ahbConfig.enableAHBPrefetch = true;

    config.rxSampleClock               = kFLEXSPI_ReadSampleClkLoopbackFromDqsPad;
    /*Allow AHB read start address do not follow the alignment requirement. */
    config.ahbConfig.enableReadAddressOpt = true;
    config.ahbConfig.enableAHBBufferable  = true;
    config.ahbConfig.enableAHBCachable    = true;

    flexspi_config_init(&config);

    FLEXSPI_Init(EXAMPLE_FLEXSPI, &config);

    /* Configure flash settings according to serial flash feature. */
    FLEXSPI_SetFlashConfig(EXAMPLE_FLEXSPI, &deviceconfig, EXAMPLE_FLEXSPI_PORT);

    /* Update LUT table. */
    FLEXSPI_UpdateLUT(EXAMPLE_FLEXSPI, 0, customLUT, CUSTOM_LUT_LENGTH);

    /* Enter quad mode. */
    status = flexspi_nor_enable_quad_mode(EXAMPLE_FLEXSPI);

    /* Do software reset. */
    FLEXSPI_SoftwareReset(EXAMPLE_FLEXSPI);

    return status;
}

int flexspi_init(void)
{
    uint32_t primask;

    primask = DisableGlobalIRQ();
    __flexspi_init();
    EnableGlobalIRQ(primask);

    return 0;
}
