/*
 * Copyright 2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * MCXE3X internal (on-chip) flash storage backend interface.
 *
 * MCXE3X has no usable external QSPI flash for the file system (QSPI pins
 * conflict with Ethernet), so littlefs lives in the on-chip flash. There is no
 * FlexSPI controller involved here, hence the internal_flash_* naming (as
 * opposed to the flexspi_* backends used by the RT boards).
 *
 * The block device operations are implemented in the generic
 * boards/src/demo_apps/avb_tsn/common/lfs_internal_flash.c on top of the
 * SDK mflash driver (components/flash/mflash/mcxe3x). That file also provides
 * the lfs_storage_init() entry point called from storage.c.
 */

#ifndef _INTERNAL_FLASH_OPS_H_
#define _INTERNAL_FLASH_OPS_H_

#include "mflash_drv.h"

/*******************************************************************************
 * File system geometry
 *
 * On-chip code flash spans four 1 MB blocks [0x00400000, 0x007FFFFF]:
 *   Block 0 : 0x00400000 - 0x004FFFFF
 *   Block 1 : 0x00500000 - 0x005FFFFF
 *   Block 2 : 0x00600000 - 0x006FFFFF
 *   Block 3 : 0x00700000 - 0x007FFFFF
 * The top 48 KB of block 3 [0x007F4000, 0x007FFFFF] is reserved for the secure
 * BAF code (HSE_B) and must not be used by the application.
 *
 * The file system occupies (almost) the entirety of code flash block 3, from
 * the block-3 base up to the HSE_B/SBAF boundary. Application code and data
 * live in blocks 0-2 (3 MB), so the C40 read-while-write rule is satisfied:
 * the CPU fetches/reads from blocks 0-2 while littlefs programs/erases block 3.
 *
 * These values MUST stay in sync with the reserved region m_lfs_size in in
 * devices/MCXE3X/common/armgcc/MCXE3x_flash.ld.
 *
 * FLASH_SECTOR_LFS  : number of 8192-byte sectors reserved for littlefs.
 *                     122 sectors * 8192 = 976 KB = block 3 minus the 48 KB
 *                     SBAF region ([0x00700000, 0x007F4000)).
 * FLASH_LFS_OFFSET  : byte offset of the first FS sector, relative to
 *                     MFLASH_BASE_ADDRESS (the mflash driver adds the base).
 *                     Block 3 base 0x00700000 - base 0x00400000 = 0x00300000.
 ******************************************************************************/
#define FLASH_SECTOR_LFS 122U

/* First FS sector = code flash block 3 base, relative to MFLASH_BASE_ADDRESS. */
#define FLASH_LFS_OFFSET (0x00300000U)

#endif /* _INTERNAL_FLASH_OPS_H_ */
