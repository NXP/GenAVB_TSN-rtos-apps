/*
 * Copyright 2016 Freescale Semiconductor, Inc.
 * Copyright 2016-2020, 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "flexspi_ops.h"
#include "lfs.h"

#define LFS_READ_SIZE           16
#define LFS_PROG_SIZE           FLASH_PAGE_SIZE
#define LFS_CACHE_SIZE          1024
#define LFS_LOOKAHEAD_SIZE      16
#define LFS_FIRST_SECTOR        240 /* use end of the flash */
#define LFS_SECTORS             16

static int lfs_hyperflash_read(
    const struct lfs_config *lfsc, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size)
{
    uint32_t *src;
    uint32_t *dst;

    block += LFS_FIRST_SECTOR;

    src = (uint32_t *)(EXAMPLE_FLEXSPI_AMBA_BASE + block * lfsc->block_size + off);
    dst = (uint32_t *)buffer;

    if (((uint32_t)src & 0x03) || ((uint32_t)dst & 0x03) || (size & 0x03))
    {
        return LFS_ERR_IO; /* unaligned access */
    }

    for (; size; size -= 4)
    {
        *dst++ = *src++;
    }

    return LFS_ERR_OK;
}

static int lfs_hyperflash_prog(
    const struct lfs_config *lfsc, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size)
{
    status_t status;

    block += LFS_FIRST_SECTOR;
    status = flexspi_page_program(EXAMPLE_FLEXSPI, block * lfsc->block_size + off, (void *)buffer);

    if (status == kStatus_Fail)
    {
        return LFS_ERR_CORRUPT;
    }
    else if (status != kStatus_Success)
    {
        return LFS_ERR_IO;
    }

    return LFS_ERR_OK;
}

static int lfs_hyperflash_erase(const struct lfs_config *lfsc, lfs_block_t block)
{
    status_t status;

    block += LFS_FIRST_SECTOR;
    status = flexspi_erase_sector(EXAMPLE_FLEXSPI, block * lfsc->block_size);

    if (status == kStatus_Fail)
    {
        return LFS_ERR_CORRUPT;
    }
    else if (status != kStatus_Success)
    {
        return LFS_ERR_IO;
    }

    return LFS_ERR_OK;
}

static int lfs_hyperflash_sync(const struct lfs_config *lfsc)
{
    /* Workaround to flush FlexSPI AHB read/write buffers */
    FLEXSPI_SoftwareReset(EXAMPLE_FLEXSPI);

    SCB_InvalidateDCache_by_Addr((uint8_t *)EXAMPLE_FLEXSPI_AMBA_BASE + LFS_FIRST_SECTOR * lfsc->block_size, LFS_SECTORS * lfsc->block_size);

    return LFS_ERR_OK;
}

SDK_ALIGN(static uint8_t read_buffer[LFS_CACHE_SIZE], 4);
SDK_ALIGN(static uint8_t prog_buffer[LFS_CACHE_SIZE], 4);
SDK_ALIGN(static uint8_t lookahead_buffer[LFS_LOOKAHEAD_SIZE], 4);

static struct lfs_config lfsc_default = {
    // block device driver context data
    .context = NULL,

    // block device operations
    .read  = &lfs_hyperflash_read,
    .prog  = &lfs_hyperflash_prog,
    .erase = &lfs_hyperflash_erase,
    .sync  = &lfs_hyperflash_sync,

    .block_cycles = -1,
    
    // block device configuration
    .cache_size       = LFS_CACHE_SIZE,
    .read_size        = LFS_READ_SIZE,
    .prog_size        = LFS_PROG_SIZE,
    .block_size       = FLASH_SECTOR_SIZE,
    .block_count      = LFS_SECTORS,
    .lookahead_size   = LFS_LOOKAHEAD_SIZE,
    .read_buffer      = &read_buffer[0],
    .prog_buffer      = &prog_buffer[0],
    .lookahead_buffer = &lookahead_buffer[0],
};

int lfs_get_default_config(struct lfs_config **lfsc)
{
    *lfsc = &lfsc_default;

    return 0;
}
