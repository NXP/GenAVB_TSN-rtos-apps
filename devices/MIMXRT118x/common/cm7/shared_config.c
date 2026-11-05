/*
 * Copyright 2024-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <string.h>

#include "avb_tsn/common/system_config.h"
#include "avb_tsn/common/genavb.h"

#include "shared_config.h"

#define SHARED_CONFIG_OFFSET 0x200
#define STACK_LEN 0x400
#define DTCM_END 0x20040000

extern struct net_config system_net_cfg[CONFIG_APP_LOGICAL_PORTS];
extern struct rtos_apps_tsn_config system_tsn_app_cfg;

struct system_config_m7 {
    union {
        struct rtos_apps_tsn_config tsn_app_config;
    } app;
    struct net_config net[CONFIG_APP_LOGICAL_PORTS];
};

void shared_system_config_get(void)
{
    struct system_config_m7 *system_config_m7 = (struct system_config_m7 *)((uintptr_t)(DTCM_END - STACK_LEN - SHARED_CONFIG_OFFSET));

    memcpy(&system_net_cfg, (const void *)system_config_m7->net, sizeof(system_config_m7->net));
    memcpy(&system_tsn_app_cfg, (const void *)&system_config_m7->app.tsn_app_config, sizeof(system_config_m7->app.tsn_app_config));
}
