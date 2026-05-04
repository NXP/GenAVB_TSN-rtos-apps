/*
 * Copyright 2019-2020, 2025 NXP
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

int app_log_level_set(char *level_str)
{
    rtos_apps_log_level_t level;

    if (!strcmp(level_str, "crit"))
        level = LOG_CRIT;
    else if (!strcmp(level_str, "err"))
        level = LOG_ERR;
    else if (!strcmp(level_str, "init"))
        level = LOG_INFO;
    else if (!strcmp(level_str, "info"))
        level = LOG_INFO;
    else if (!strcmp(level_str, "dbg"))
        level = LOG_DEBUG;
    else
        return -1;

    rtos_apps_log_level_config_set(level);

    return 0;
}
