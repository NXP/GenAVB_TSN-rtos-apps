/*
 * Copyright 2020-2021, 2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FreeRTOS.h"
#include "task.h"

void SerialManager_WriteTimeDelay(uint32_t ms)
{
    vTaskDelay(pdMS_TO_TICKS(ms));
}

void SerialManager_ReadTimeDelay(uint32_t ms)
{
    vTaskDelay(pdMS_TO_TICKS(ms));
}
