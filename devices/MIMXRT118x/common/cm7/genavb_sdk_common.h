/*
 * Copyright 2023-2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _GENAVB_SDK_COMMON_H_
#define _GENAVB_SDK_COMMON_H_

#include <stdarg.h>
#include <stdint.h>

#include "fsl_common.h"
#include "fsl_netc.h"
#include "fsl_netc_soc.h"
#include "fsl_gpt.h"
#include "fsl_phy.h"

#define BOARD_NET_RX_PACKETS 8

#define BOARD_NUM_NETC_HW_CLOCK       1

#define BOARD_NETC_HW_CLOCK_0_ID      0
#define BOARD_NETC_HW_CLOCK_0_OWNER   0
#define BOARD_NETC_HW_CLOCK_0_SELECT  kNETC_TimerExtRefClk
#define BOARD_NETC_HW_CLOCK_0_FREQ    (dev_get_net_1588_freq(SW0_BASE))
#define BOARD_NETC_HW_CLOCK_0_MSGINTR_BASE MSGINTR1
#define BOARD_NETC_HW_CLOCK_0_MSGINTR_CH 0
#define BOARD_HW_CLOCK0_NUM_TIMERS    1
#define BOARD_HW_CLOCK0_TIMER0_ID     1 /* Alarm2 */
#define BOARD_HW_CLOCK0_TIMER0_ENABLED
#define BOARD_HW_CLOCK0_TIMER0_IRQ_ENABLED

#define BOARD_NUM_GPT        1

#define BOARD_GPT_0_BASE        GPT2
#define BOARD_GPT_0_IRQ         GPT2_IRQn
#define BOARD_GPT_0_IRQ_HANDLER GPT2_IRQHandler
#define BOARD_GPT_0_PRESCALE    3

#define QBV_LIST_MAX_ENTRIES NETC_TB_TGS_MAX_ENTRY

/*! @brief MSGINTR block configuration */
#define BOARD_NUM_MSGINTR       1
#define BOARD_MSGINTR0_BASE MSGINTR1

unsigned int BOARD_GPT_clk_src(void *base);
unsigned int BOARD_GPT_clk_freq(void *base);

int BOARD_NetPort_Get_MAC(unsigned int port, uint8_t *mac);

uint32_t dev_get_net_core_freq(void *base);

uint32_t dev_get_net_1588_freq(void *base);

#endif /* _GENAVB_SDK_COMMON_H_ */
