/*
 * Copyright 2019-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "genavb/qos.h"

#include "rtos_apps/tsn/tsn_entry.h"
#include "rtos_apps/tsn/tsn_tasks_config.h"

#ifndef CONFIG_APP_CONTROLLER_NUM_IO_DEVICES
#define CONFIG_APP_CONTROLLER_NUM_IO_DEVICES 1
#endif

struct rtos_apps_tsn_config system_tsn_app_cfg = {
#ifdef CONFIG_APP_MOTOR_CONTROLLER
    .mode = MOTOR_NETWORK,
    .role = CONTROLLER_0,
#elif defined(CONFIG_APP_MOTOR_IO_DEVICE)
    .mode = MOTOR_NETWORK,
    .role = IO_DEVICE_0,
    .motor_offset = 0,
#else
    .mode = NETWORK_ONLY,
    .role = CONTROLLER_0,
#endif
    .cmd_client = 0,
    .control_strategy = CTRL_STRAT_SYNCHRONIZED,
    .num_io_devices = CONFIG_APP_CONTROLLER_NUM_IO_DEVICES,
    .period_ns = APP_PERIOD_DEFAULT,
    .offset = APP_OFFSET_DEFAULT,
    .network_budget_ns = APP_NETWORK_BUDGET_DEFAULT,
    .priority = ISOCHRONOUS_DEFAULT_PRIORITY,
    .port_id = 0,
    .packets = 1,
    .zero_copy = 1,
    .rx_tc_mask = 0,
};
