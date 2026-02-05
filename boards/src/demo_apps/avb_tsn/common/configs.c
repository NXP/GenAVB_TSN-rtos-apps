/*
 * Copyright 2022, 2024-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "genavb.h"
#include "system_config.h"

struct net_config system_net_cfg[CONFIG_APP_LOGICAL_PORTS] = {
    [0] = {
        .hw_addr = {0x00, 0xBB, 0xCC, 0xDD, 0xEE, 0x00},
        .ip_addr = {192, 168, 1, 4},
        .net_mask = {255, 255, 255, 0},
        .gw_addr = {192, 168, 1, 254},
    },
#if CONFIG_APP_LOGICAL_PORTS > 1
    [1] = {
        .hw_addr = {0x00, 0xBB, 0xCC, 0xDD, 0xEE, 0x01},
        .ip_addr = {192, 168, 2, 4},
        .net_mask = {255, 255, 255, 0},
        .gw_addr = {192, 168, 2, 254},
    },
#endif
#if CONFIG_APP_LOGICAL_PORTS > 2
    [2] = {
        .hw_addr = {0x00, 0x11, 0x22, 0x33, 0x44, 0x02},
    },
#endif
#if CONFIG_APP_LOGICAL_PORTS > 3
    [3] = {
        .hw_addr = {0x00, 0x11, 0x22, 0x33, 0x44, 0x03},
    },
#endif
#if CONFIG_APP_LOGICAL_PORTS > 4
    [4] = {
        .hw_addr = {0x00, 0x11, 0x22, 0x33, 0x44, 0x04},
     },
#endif
#if CONFIG_APP_LOGICAL_PORTS > 5
    [5] = {
        .hw_addr = {0x00, 0x11, 0x22, 0x33, 0x44, 0x05},
	},
#endif
#if CONFIG_APP_LOGICAL_PORTS > 6
    [6] = {
        .hw_addr = {0x00, 0x11, 0x22, 0x33, 0x44, 0x06},
    },
#endif
};
