/*
 * Copyright 2024-2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _GENAVB_SDK_H_
#define _GENAVB_SDK_H_

#include "genavb_sdk_common.h"
#include "genavb_sdk_net_port.h"

#ifdef CONFIG_APP_TSN_ENDPOINT

#define BOARD_ENETC0_TS_FREERUNNING   1

#define BOARD_HW_CLOCK0_TIMER0_ENABLED

/*! @brief MSGINTR block configuration */
#undef BOARD_NUM_MSGINTR
#define BOARD_NUM_MSGINTR       1
#define BOARD_MSGINTR0_BASE MSGINTR1

#endif /* CONFIG_APP_TSN_ENDPOINT */

#endif /* _GENAVB_SDK_H_ */
