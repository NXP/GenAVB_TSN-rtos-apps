/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017, 2024-2026 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "board.h"

#include "tsn_app.h"
#include "rtos_apps/tsn/tsn_tasks_config.h"
#include "rtos_apps/tsn/tsn_entry.h"

#define MCXE31B_APP_PERIOD_DEFAULT     250000
#define MCXE31B_NETWORK_BUDGET_DEFAULT 20000
#define MCXE31B_APP_OFFSET_DEFAULT     1

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Main function
 */

extern struct rtos_apps_tsn_config system_tsn_app_cfg;

int main(void)
{
    struct rtos_apps_tsn_config *config = &system_tsn_app_cfg;

    BOARD_InitHardware();
    BOARD_InitDebugConsole();
    BOARD_InitNVIC();

    LED_BLUE_INIT(LOGIC_LED_ON);

    /* Override default configuration */
    config->role = IO_DEVICE_0;
    config->period_ns = MCXE31B_APP_PERIOD_DEFAULT;
    config->network_budget_ns = MCXE31B_NETWORK_BUDGET_DEFAULT;
#if defined(CONFIG_APP_MOTOR_IO_DEVICE) || defined(CONFIG_APP_MOTOR_CONTROLLER)
    config->offset = MCXE31B_APP_OFFSET_DEFAULT;
#endif

    tsn_app_main();

    return 0;
}