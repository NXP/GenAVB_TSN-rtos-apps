/*
 * Copyright 2023-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _GENAVB_SDK_H_
#define _GENAVB_SDK_H_

#include <stdarg.h>
#include <stdint.h>

#include "fsl_common.h"
#include "fsl_enet.h"
#include "fsl_gpt.h"
#include "fsl_phy.h"

/*! @brief NET configuration */
#define BOARD_NUM_PORTS      1

/*! @brief MDIO configuration */
#define BOARD_NUM_MDIO          1

#define BOARD_MDIO0_DRV_TYPE    ENET_t
#define BOARD_MDIO0_DRV_INDEX   0

/* PHY configuration */
#define BOARD_NUM_PHY        1

/* PHY 0 is KSZ8081 phy */
#define BOARD_PHY0_MDIO_ID      0
#define BOARD_PHY0_ADDRESS      (0x02U) /* Phy address of enet port 0. */
#define BOARD_PHY0_OPS          phyksz8081_ops   /* PHY operations. */

#define BOARD_PHY0_RX_LATENCY_100M (810)
#define BOARD_PHY0_TX_LATENCY_100M (810)

#define BOARD_NUM_ENET_PORTS	1

#define BOARD_NET_PORT0_MII_MODE    kENET_RmiiMode
#define BOARD_NET_PORT0_DRV_TYPE    ENET_t
#define BOARD_NET_PORT0_DRV_BASE    ENET_BASE
#define BOARD_NET_PORT0_DRV_INDEX   (0)
#define BOARD_NET_PORT0_PHY_INDEX   (0)
#define BOARD_NET_PORT0_SPEED       kPHY_Speed100M
#define BOARD_NET_PORT0_DUPLEX      kPHY_FullDuplex
#define BOARD_NET_PORT0_1588_TIMER_EVENT_CHANNEL  kENET_PtpTimerChannel4

#define BOARD_ENET0_1588_TIMER_CHANNEL_0      kENET_PtpTimerChannel1
#define BOARD_ENET0_1588_TIMER_CHANNEL_1      kENET_PtpTimerChannel2
#define BOARD_ENET0_1588_TIMER_CHANNEL_2      kENET_PtpTimerChannel3

#define BOARD_NUM_GPT           2

#define BOARD_GPT_0_BASE        GPT1
#define BOARD_GPT_0_IRQ         GPT1_IRQn
#define BOARD_GPT_0_IRQ_HANDLER GPT1_IRQHandler

#define BOARD_GPT_1_BASE        GPT2
#define BOARD_GPT_1_IRQ         GPT2_IRQn
#define BOARD_GPT_1_IRQ_HANDLER GPT2_IRQHandler

#define BOARD_GPT_REC_BASE      GPT2
#define BOARD_GPT_REC_CHANNEL   1

unsigned int BOARD_GPT_clk_src(void *base);
unsigned int BOARD_GPT_clk_src_div(void *base);
unsigned int BOARD_GPT_clk_freq(void *base);

int BOARD_NetPort_Get_MAC(unsigned int port, uint8_t *mac);

/*
 * Primary clock source for all PLLs
 */
uint32_t dev_get_pll_ref_freq(void);

/*
 * Audio Pll tuning
 */
void dev_write_audio_pll_num(uint32_t num);

uint32_t dev_read_audio_pll_num(void);

uint32_t dev_read_audio_pll_denom(void);

uint32_t dev_read_audio_pll_post_div(void);

/*
 * Enet module frequency (ipg_clk)
 */
uint32_t dev_get_enet_core_freq(void *base);

/*
 * Enet 1588 timer frequency (ipg_clk_time)
 */
uint32_t dev_get_enet_1588_freq(void *base);

/*
 * GPT input frequency (ipg_clk)
 */
uint32_t dev_get_gpt_ipg_freq(void *base);

#endif /* _GENAVB_SDK_H_ */
