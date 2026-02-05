/*
 * Copyright 2024-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * m7 shared configuration
 */
#ifndef _SHARED_CONFIG_H_
#define _SHARED_CONFIG_H_

#include "avb_tsn/common/system_config.h"

/* Must match CONFIG_APP_LOGICAL_PORTS in M7 application */
#define CONFIG_APP_LOGICAL_PORTS_M7 2

struct system_config_m7 {
    union {
        struct rtos_apps_tsn_config tsn_app_config;
    } app;
    struct net_config net[CONFIG_APP_LOGICAL_PORTS_M7];
};

void shared_system_config_set(void);

#endif /* _SHARED_CONFIG_H_ */
