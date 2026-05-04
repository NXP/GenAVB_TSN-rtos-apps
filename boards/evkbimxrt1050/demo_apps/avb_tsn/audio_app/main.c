/*
 * Copyright 2024-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "audio_app.h"
#include "board.h"

int main(void)
{
    BOARD_InitDebugConsole();
    BOARD_InitNetInterfaces();
    BOARD_InitMediaClock();
    BOARD_InitNVIC();

    audio_app_main();

    /* unreacheable */

    return 0;
}
