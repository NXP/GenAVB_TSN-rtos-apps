/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017, 2024-2026 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "board.h"

#include "tsn_app.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Main function
 */

int main(void)
{
    BOARD_InitHardware();
    BOARD_InitDebugConsole();
    BOARD_InitNVIC();

    LED_BLUE_INIT(LOGIC_LED_ON);

    tsn_app_main();

    return 0;
}
