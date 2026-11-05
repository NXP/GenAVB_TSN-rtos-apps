/*
 * Copyright 2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * littlefs block device glue for on-chip (internal) flash.
 *
 * On SoCs that have no usable external QSPI/HyperFlash for the FS, the FS
 * lives in the on-chip flash. The block device program/erase operations drive
 * the SDK flash HAL (FLASH_Program/FLASH_Erase) directly from this app-side
 * backend rather than through the SDK mflash driver to provide a better interrupt
 * masking policy (being able to mask only up to syscall priority rather than
 * masking all interrupts including the high-prio ones)
 * 
 * On MCXE devices, writing to the flash and masking the high priotity motor (BTCU/ADC)
 * interrupt is causing a system freeze (due to apperently a bad handling handling of
 * faulty state). As a workaround, we only mask the syscall level interrupts when writing
 * to flash and leaving the motor IRQ handled. This is safe since code and littlefs storage
 * live in two different blocks.
 * Note: the above workaround assume that unmasked interrupts do not access flash or access
 * it in a safe RWW way.
 *
 * Board/SoC specifics (FS size, offset, QSPI vs on-chip rationale) live in
 * the per-board internal_flash_ops.h.
 *
 * Design notes:
 *   - read : on-chip flash is memory mapped, so reads are a plain memory copy
 *            from the mapped window. This preserves the fast, driver-light
 *            read path used by lfs_qspi_nor.c. The copy is wrapped in a local
 *            helper (internalflash_read) to provide a uniform seam.
 *   - prog : must use the flash controller (page programming), so it drives
 *            FLASH_Program directly (internalflash_page_program).
 *   - erase: must use the flash controller, so it drives FLASH_Erase directly
 *            (internalflash_sector_erase).
 *   - sync : no-op. The program/erase helpers below already invalidate D-cache
 *            and issue an ISB, so there is no separate cache/AHB flush to
 *            perform here (contrast the FlexSPI variant which must reset
 *            FlexSPI and clean/invalidate the cache).
 */

#include "internal_flash_ops.h"
#include "lfs.h"

#include "mflash_drv.h"
#include "fsl_flash.h"
#include "fsl_cache.h"

#include "FreeRTOSConfig.h"

/* Flash HAL context, owned by this backend. */
static flash_config_t flash_ctx;

#define LFS_READ_SIZE      16
#define LFS_PROG_SIZE      MFLASH_PAGE_SIZE
#define LFS_CACHE_SIZE     MFLASH_PAGE_SIZE
#define LFS_LOOKAHEAD_SIZE 16

#define LFS_BLOCK_SIZE MFLASH_SECTOR_SIZE
#define LFS_SECTORS    FLASH_SECTOR_LFS

/* Physical byte offset (relative to MFLASH_BASE_ADDRESS) of the first sector */
#define LFS_FIRST_OFFSET FLASH_LFS_OFFSET

/*
 * Enter/exit the flash-controller critical section using BASEPRI scoped at the
 * FreeRTOS syscall ceiling. Interrupts at or above the ceiling are masked;
 */
static inline uint32_t __internalflash_enter_critical(void)
{
    uint32_t saved_basepri = __get_BASEPRI();

    __set_BASEPRI(configMAX_SYSCALL_INTERRUPT_PRIORITY);

    return saved_basepri;
}

static inline void __internalflash_exit_critical(uint32_t saved_basepri)
{
    __set_BASEPRI(saved_basepri);
}

static int32_t internalflash_page_program(uint32_t page_addr, uint32_t *data)
{
    status_t ret;
    uint32_t saved_basepri;
    uint32_t logaddr = MFLASH_BASE_ADDRESS + page_addr;

    if ((page_addr % (uint32_t)MFLASH_PAGE_SIZE) != 0UL)
    {
        return kStatus_InvalidArgument;
    }

    saved_basepri = __internalflash_enter_critical();

    /* Check and unlock sector protection inside the critical section */
    ret = FLASH_GetSectorProtection(&flash_ctx, logaddr);

    if (ret == kStatus_FLASH_SectorLocked)
    {
        ret = FLASH_SetSectorProtection(&flash_ctx, logaddr, false);
        if (ret != kStatus_FLASH_Success)
        {
            __internalflash_exit_critical(saved_basepri);
            return ret;
        }
    }

    ret = FLASH_Program(&flash_ctx, logaddr, data, MFLASH_PAGE_SIZE);

    /* Invalidate DCache if enabled */
    if (SCB_CCR_DC_Msk == (SCB_CCR_DC_Msk & SCB->CCR))
    {
        DCACHE_InvalidateByRange(logaddr, MFLASH_PAGE_SIZE);
    }

    __internalflash_exit_critical(saved_basepri);

    __ISB();

    return ret;
}

static int32_t internalflash_sector_erase(uint32_t sector_addr)
{
    status_t ret;
    uint32_t saved_basepri;
    uint32_t logaddr = MFLASH_BASE_ADDRESS + sector_addr;

    if ((sector_addr % MFLASH_SECTOR_SIZE) != 0UL)
    {
        return kStatus_InvalidArgument;
    }

    saved_basepri = __internalflash_enter_critical();

    /* Check and unlock sector protection inside the critical section */
    ret = FLASH_GetSectorProtection(&flash_ctx, logaddr);

    if (ret == kStatus_FLASH_SectorLocked)
    {
        ret = FLASH_SetSectorProtection(&flash_ctx, logaddr, false);
        if (ret != kStatus_FLASH_Success)
        {
            __internalflash_exit_critical(saved_basepri);
            return ret;
        }
    }

    ret = FLASH_Erase(&flash_ctx, logaddr, MFLASH_SECTOR_SIZE, kFLASH_ApiEraseKey);

    /* Invalidate DCache if enabled */
    if (SCB_CCR_DC_Msk == (SCB_CCR_DC_Msk & SCB->CCR))
    {
        DCACHE_InvalidateByRange(logaddr, MFLASH_SECTOR_SIZE);
    }

    __internalflash_exit_critical(saved_basepri);

    __ISB();

    return ret;
}

static int internalflash_read(uint32_t offset, void *buffer, uint32_t size)
{
    if (mflash_drv_read(offset, (uint32_t *)buffer, size) != kStatus_Success)
    {
        return LFS_ERR_IO;
    }

    return LFS_ERR_OK;
}

static int internalflash_sync(void)
{
    return LFS_ERR_OK;
}

static int lfs_internalflash_read(
    const struct lfs_config *lfsc, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size)
{
    uint32_t offset = LFS_FIRST_OFFSET + block * lfsc->block_size + off;

    return internalflash_read(offset, buffer, size);
}

static int lfs_internalflash_prog(
    const struct lfs_config *lfsc, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size)
{
    uint32_t base = LFS_FIRST_OFFSET + block * lfsc->block_size + off;
    uint32_t page_ofs;

    /* littlefs prog_size is one page; loop defensively in case that changes */
    for (page_ofs = 0; page_ofs < size; page_ofs += MFLASH_PAGE_SIZE)
    {
        int32_t status =
            internalflash_page_program(base + page_ofs, (uint32_t *)((uintptr_t)buffer + page_ofs));

        if (status != kStatus_Success)
        {
            return LFS_ERR_IO;
        }
    }

    return LFS_ERR_OK;
}

static int lfs_internalflash_erase(const struct lfs_config *lfsc, lfs_block_t block)
{
    uint32_t base = LFS_FIRST_OFFSET + block * lfsc->block_size;
    uint32_t sector_ofs;

    for (sector_ofs = 0; sector_ofs < lfsc->block_size; sector_ofs += MFLASH_SECTOR_SIZE)
    {
        int32_t status = internalflash_sector_erase(base + sector_ofs);

        if (status != kStatus_Success)
        {
            return LFS_ERR_IO;
        }
    }

    return LFS_ERR_OK;
}

static int lfs_internalflash_sync(const struct lfs_config *lfsc)
{
    (void)lfsc;

    return internalflash_sync();
}

SDK_ALIGN(static uint8_t read_buffer[LFS_CACHE_SIZE], 4);
SDK_ALIGN(static uint8_t prog_buffer[LFS_CACHE_SIZE], 4);
SDK_ALIGN(static uint8_t lookahead_buffer[LFS_LOOKAHEAD_SIZE], 4);

static struct lfs_config lfsc_default = {
    /* block device driver context data */
    .context = NULL,

    /* block device operations */
    .read  = &lfs_internalflash_read,
    .prog  = &lfs_internalflash_prog,
    .erase = &lfs_internalflash_erase,
    .sync  = &lfs_internalflash_sync,

    .block_cycles = -1,

    /* block device configuration */
    .read_size        = LFS_READ_SIZE,
    .prog_size        = LFS_PROG_SIZE,
    .block_size       = LFS_BLOCK_SIZE,
    .block_count      = LFS_SECTORS,
    .cache_size       = LFS_CACHE_SIZE,
    .lookahead_size   = LFS_LOOKAHEAD_SIZE,
    .read_buffer      = &read_buffer[0],
    .prog_buffer      = &prog_buffer[0],
    .lookahead_buffer = &lookahead_buffer[0],
};

/*
 * littlefs backend init: this board's FS lives in on-chip (internal) flash.
 * Initializes the flash HAL context used by the program/erase helpers.
 */
int lfs_storage_init(void)
{
    (void)memset(&flash_ctx, 0, sizeof(flash_ctx));

    return FLASH_Init(&flash_ctx);
}

int lfs_get_default_config(const struct lfs_config **lfsc)
{
    *lfsc = &lfsc_default;

    return 0;
}
