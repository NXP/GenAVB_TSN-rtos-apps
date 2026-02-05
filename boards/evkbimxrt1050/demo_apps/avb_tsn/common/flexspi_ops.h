/*
 * Copyright 2016 Freescale Semiconductor, Inc.
 * Copyright 2016-2020 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FLEXSPI_OPS_H_
#define _FLEXSPI_OPS_H_

#include "fsl_flexspi.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define EXAMPLE_FLEXSPI FLEXSPI
#define EXAMPLE_FLEXSPI_AMBA_BASE FlexSPI_AMBA_BASE
#define FLASH_SIZE 0x10000
#define FLASH_PAGE_SIZE 512
#define FLASH_SECTOR_SIZE 0x40000

int flexspi_init(void);

status_t flexspi_erase_sector(FLEXSPI_Type *base, uint32_t address);

status_t flexspi_page_program(FLEXSPI_Type *base, uint32_t address, const uint32_t *src);

#endif /* _FLEXSPI_OPS_H_ */
