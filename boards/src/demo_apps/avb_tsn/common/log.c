/*
 * Copyright 2019-2020, 2025-2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <string.h>

#include "rtos_apps/types.h"

#include "log.h"

void app_log_update_time(genavb_clock_id_t clk_id)
{
    uint64_t log_time;

    if (genavb_clock_gettime64(clk_id, &log_time) < 0)
        return;

    rtos_apps_log_timestamp_update(log_time / NSECS_PER_SEC);
}

