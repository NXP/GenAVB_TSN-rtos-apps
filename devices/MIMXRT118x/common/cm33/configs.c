/*
 * Copyright 2022, 2024-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "genavb/qos.h"

#include "rtos_apps/tsn/tsn_tasks_config.h"

#include "shared_config.h"
#include "avb_tsn/common/system_config.h"

#ifndef CONFIG_APP_CONTROLLER_NUM_IO_DEVICES
#define CONFIG_APP_CONTROLLER_NUM_IO_DEVICES 2
#endif

/* Default values for M7 configuration */
struct system_config_m7 m7_system_cfg = {
    .net = {
         [0] = {
             .hw_addr = {0x00, 0xBB, 0xCC, 0xDD, 0xEE, 0x10},
             .ip_addr = {192, 168, 1, 5},
             .net_mask = {255, 255, 255, 0},
             .gw_addr = {192, 168, 1, 254},
         },
#if CONFIG_APP_LOGICAL_PORTS_M7 > 1
         [1] = {
             .hw_addr = {0x00, 0xBB, 0xCC, 0xDD, 0xEE, 0x11},
             .ip_addr = {192, 168, 2, 5},
             .net_mask = {255, 255, 255, 0},
             .gw_addr = {192, 168, 2, 254},
         },
#endif
    },
    .app = {
        .tsn_app_config = {
            .mode = NETWORK_ONLY,
            .role = CONTROLLER_0,
            .num_io_devices = CONFIG_APP_CONTROLLER_NUM_IO_DEVICES,
            .motor_offset = 0,
            .period_ns = APP_PERIOD_DEFAULT,
            .offset = APP_OFFSET_DEFAULT,
            .network_budget_ns = APP_NETWORK_BUDGET_DEFAULT,
            .priority = ISOCHRONOUS_DEFAULT_PRIORITY,
            .packets = 1,
            .zero_copy = 1,
            .rx_tc_mask = 0,
        },
    },
};
