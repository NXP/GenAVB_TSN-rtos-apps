/*
 * Copyright 2020, 2022-2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FLEXSPI_OPS_H_
#define _FLEXSPI_OPS_H_

#include "fsl_flexspi.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define EXAMPLE_FLEXSPI FLEXSPI1
#define EXAMPLE_FLEXSPI_AMBA_BASE FlexSPI1_AMBA_BASE
#define EXAMPLE_FLEXSPI_PORT kFLEXSPI_PortA1
#define FLASH_SIZE 0x10000 /* 512Mb/KiB (512*1024*1024 / 1024 / 8) */
#define FLASH_PAGE_SIZE 256
#define FLASH_SECTOR_SIZE 0x1000 /* 4KiB */
#define FLASH_SECTOR_TOTAL	((FLASH_SIZE * 1024) / FLASH_SECTOR_SIZE)
#define FLASH_SECTOR_LFS	32 /* 128KiB */

#define NOR_CMD_LUT_SEQ_IDX_READ_NORMAL 7
#define NOR_CMD_LUT_SEQ_IDX_READ_FAST 13
#define NOR_CMD_LUT_SEQ_IDX_READ_FAST_QUAD 0
#define NOR_CMD_LUT_SEQ_IDX_READSTATUS  1
#define NOR_CMD_LUT_SEQ_IDX_WRITEENABLE 2
#define NOR_CMD_LUT_SEQ_IDX_ERASESECTOR 3
#define NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_SINGLE 6
#define NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_QUAD 4
#define NOR_CMD_LUT_SEQ_IDX_READID 8
#define NOR_CMD_LUT_SEQ_IDX_WRITESTATUSREG 9
#define NOR_CMD_LUT_SEQ_IDX_ENTERQPI 10
#define NOR_CMD_LUT_SEQ_IDX_EXITQPI 11
#define NOR_CMD_LUT_SEQ_IDX_READSTATUSREG 12
#define NOR_CMD_LUT_SEQ_IDX_ERASECHIP 5
#define CUSTOM_LUT_LENGTH 60
#define FLASH_QUAD_ENABLE 0x02
#define FLASH_BUSY_STATUS_POL 1
#define FLASH_BUSY_STATUS_OFFSET 0

extern flexspi_device_config_t deviceconfig;
extern const uint32_t customLUT[CUSTOM_LUT_LENGTH];

int flexspi_init(void);

void flexspi_clock_init(void);

void flexspi_config_init(flexspi_config_t *config);

status_t flexspi_page_program(FLEXSPI_Type *base, uint32_t address, const uint32_t *src);

status_t flexspi_erase_sector(FLEXSPI_Type *base, uint32_t address);

#endif /* _FLEXSPI_OPS_H_ */
