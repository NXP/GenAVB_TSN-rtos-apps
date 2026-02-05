/*
 * Copyright 2023-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_ele_base_api.h"
#include "fsl_trdc.h"

int trdc_init(void)
{
    trdc_memory_access_control_config_t memAccessConfig;
    trdc_hardware_config_t hwConfig;
    uint32_t reg_idx;

    TRDC_Init(TRDC2);

    /* 1. Get the hardware configuration of the TRDC2 module. */
    TRDC_GetHardwareConfig(TRDC2, &hwConfig);

    /* 2. Set control policies for MRC and MBC access control configuration registers. */
    memset(&memAccessConfig, 0, sizeof(memAccessConfig));

    /* 3. Enable all read/write/execute access for MRC/MBC access control. */
    memAccessConfig.nonsecureUsrX  = 1U;
    memAccessConfig.nonsecureUsrW  = 1U;
    memAccessConfig.nonsecureUsrR  = 1U;
    memAccessConfig.nonsecurePrivX = 1U;
    memAccessConfig.nonsecurePrivW = 1U;
    memAccessConfig.nonsecurePrivR = 1U;
    memAccessConfig.secureUsrX     = 1U;
    memAccessConfig.secureUsrW     = 1U;
    memAccessConfig.secureUsrR     = 1U;
    memAccessConfig.securePrivX    = 1U;
    memAccessConfig.securePrivW    = 1U;
    memAccessConfig.securePrivR    = 1U;

    for (reg_idx = 0U; reg_idx < 8U; reg_idx++) {
        /* FlexSPI 1 */
        TRDC_MrcSetMemoryAccessConfig(TRDC2, &memAccessConfig, 1, reg_idx);
        /* OCRAM 1 */
        TRDC_MrcSetMemoryAccessConfig(TRDC2, &memAccessConfig, 3, reg_idx);
    }

    return 0;
}

