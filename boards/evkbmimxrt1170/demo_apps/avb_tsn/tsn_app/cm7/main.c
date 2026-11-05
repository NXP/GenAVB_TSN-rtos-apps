/*
 * Copyright 2024 NXP
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
    BOARD_InitDebugConsole();
    BOARD_InitNetInterfaces();
    BOARD_InitNVIC();

    tsn_app_main();

    return 0;
}
