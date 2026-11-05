/*
 * Copyright 2023-2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <string.h>

#include "fsl_trdc.h"

#include "trdc.h"

static void trdc_init_instance(TRDC_Type *base)
{
    trdc_memory_access_control_config_t memAccessConfig;
    trdc_hardware_config_t hwConfig;
    uint32_t i, j;

    TRDC_Init(base);

    /* 1. Get the hardware configuration of the TRDC module. */
    TRDC_GetHardwareConfig(base, &hwConfig);

    /* 2. Set control policies for MRC access control configuration registers. */
    memset(&memAccessConfig, 0, sizeof(memAccessConfig));

    /* 3. Enable all read/write/execute access for MRC access control. */
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

    for (j = 0U; j < 8U; j++) {
        for (i = 0U; i < hwConfig.mrcNumber; i++)
            TRDC_MrcSetMemoryAccessConfig(base, &memAccessConfig, i, j);
    }
}

int trdc_init(void)
{
    trdc_init_instance(TRDC1);
    trdc_init_instance(TRDC2);

    return 0;
}
