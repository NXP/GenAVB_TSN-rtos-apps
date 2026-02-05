/*
 * Copyright 2023-2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _GENAVB_SDK_H_
#define _GENAVB_SDK_H_

#include <stdarg.h>
#include <stdint.h>

#include "fsl_enet.h"
#include "fsl_enet_qos.h"
#include "fsl_gpt.h"
#include "fsl_phy.h"

#define BOARD_NET_RX_PACKETS 10

/* Port 0: ENET QOS or ENET 1G MAC */
#define BOARD_USE_ENET_QOS      (1)

/* Port 1 : ENET or disabled */
#define BOARD_USE_ENET          (0)

/*! @brief NET configuration */
#define BOARD_NUM_PORTS      1

/*! @brief MDIO configuration */
#define BOARD_NUM_MDIO          1
/* PHY configuration */
#define BOARD_NUM_PHY        1


#if BOARD_USE_ENET_QOS
#define BOARD_NUM_ENET_QOS_PORTS	1

#define BOARD_MDIO0_DRV_TYPE    ENET_QOS_t
#define BOARD_MDIO0_DRV_INDEX   0

#define BOARD_NET_PORT0_MII_MODE    kENET_QOS_RgmiiMode
#define BOARD_NET_PORT0_DRV_TYPE    ENET_QOS_t
#define BOARD_NET_PORT0_DRV_BASE    ENET_QOS_BASE
#define BOARD_NET_PORT0_DRV_INDEX   (0)
#define BOARD_NET_PORT0_PHY_INDEX   (0)
#define BOARD_NET_PORT0_SPEED       kPHY_Speed100M
#define BOARD_NET_PORT0_DUPLEX      kPHY_FullDuplex
#define BOARD_NET_PORT0_1588_TIMER_EVENT_CHANNEL  kENET_QOS_PtpPpsIstance1

#define BOARD_ENET_QOS0_1588_TIMER_CHANNEL_0      kENET_QOS_PtpPpsIstance0
#define BOARD_ENET_QOS0_1588_TIMER_CHANNEL_1      kENET_QOS_PtpPpsIstance2
#define BOARD_ENET_QOS0_1588_TIMER_CHANNEL_2      kENET_QOS_PtpPpsIstance3
#define BOARD_ENET_QOS0_1588_TIMER_PPS            kENET_QOS_PtpPpsIstance3 /* Identify timer channel with PPS output */

#else /* BOARD_USE_ENET_QOS */
#define BOARD_NUM_ENET_PORTS 1

#define BOARD_MDIO0_DRV_TYPE    ENET_1G_t
#define BOARD_MDIO0_DRV_INDEX   0

#define BOARD_NET_PORT0_MII_MODE    kENET_RgmiiMode
#define BOARD_NET_PORT0_DRV_TYPE    ENET_1G_t
#define BOARD_NET_PORT0_DRV_BASE    ENET_1G_BASE
#define BOARD_NET_PORT0_DRV_INDEX   (0)
#define BOARD_NET_PORT0_PHY_INDEX   (0)
#define BOARD_NET_PORT0_SPEED       kPHY_Speed100M
#define BOARD_NET_PORT0_DUPLEX      kPHY_FullDuplex
#define BOARD_NET_PORT0_1588_TIMER_EVENT_CHANNEL  kENET_PtpTimerChannel2

#define BOARD_ENET0_1588_TIMER_CHANNEL_0      kENET_PtpTimerChannel1
#define BOARD_ENET0_1588_TIMER_CHANNEL_1      kENET_PtpTimerChannel3
#define BOARD_ENET0_1588_TIMER_CHANNEL_2      kENET_PtpTimerChannel4

#endif /* BOARD_USE_ENET_QOS */

/* PHY 0 is RTL8211FDI phy */
#define BOARD_PHY0_MDIO_ID      0
#define BOARD_PHY0_ADDRESS      (0x01U)             /* Phy address of port 0. */
#define BOARD_PHY0_OPS          phyrtl8211f_ops     /* PHY operations. */

/* PHY delay compensation values for Tx/Rx timestamps, taking into account PHY
 * delay TX/RX asymmetry and including a 50ns margin for the propagation delay of
 * small cables (to avoid negative propagation delay measurements due to
 * propagation delay jitter)
 */

#define BOARD_PHY0_RX_LATENCY_100M (707)
#define BOARD_PHY0_TX_LATENCY_100M (589)
#define BOARD_PHY0_RX_LATENCY_1G   (541)
#define BOARD_PHY0_TX_LATENCY_1G   (139)

#if BOARD_USE_ENET

#undef BOARD_NUM_ENET_PORTS
#undef BOARD_NUM_PORTS
#undef BOARD_NUM_MDIO
#undef BOARD_NUM_PHY

#define BOARD_NUM_PORTS 2

#if BOARD_USE_ENET_QOS
#define BOARD_NUM_ENET_PORTS 1
#else
#define BOARD_NUM_ENET_PORTS 2
#endif

#define BOARD_NUM_MDIO 2
#define BOARD_NUM_PHY 2

/* PHY 1 is RTL8201 phy */
#define BOARD_PHY1_MDIO_ID      1
#define BOARD_PHY1_ADDRESS      3
#define BOARD_PHY1_OPS          phyrtl8201_ops

#define BOARD_PHY1_RX_LATENCY_100M (810)
#define BOARD_PHY1_TX_LATENCY_100M (810)

#define BOARD_MDIO1_DRV_TYPE    ENET_t
#if BOARD_USE_ENET_QOS
#define BOARD_MDIO1_DRV_INDEX 0
#else
#define BOARD_MDIO1_DRV_INDEX 1
#endif

#define BOARD_NET_PORT1_MII_MODE    kENET_RmiiMode
#define BOARD_NET_PORT1_DRV_TYPE    ENET_t
#define BOARD_NET_PORT1_DRV_BASE    ENET_BASE
#if BOARD_USE_ENET_QOS
#define BOARD_NET_PORT1_DRV_INDEX   (0)
#else
#define BOARD_NET_PORT1_DRV_INDEX   (1)
#endif

#define BOARD_NET_PORT1_SPEED       kPHY_Speed100M
#define BOARD_NET_PORT1_DUPLEX      kPHY_FullDuplex
#define BOARD_NET_PORT1_1588_TIMER_EVENT_CHANNEL  kENET_PtpTimerChannel2

#define BOARD_NET_PORT1_PHY_INDEX   (1)

#if BOARD_USE_ENET_QOS
#define BOARD_ENET0_1588_TIMER_CHANNEL_0      kENET_PtpTimerChannel1
#define BOARD_ENET0_1588_TIMER_CHANNEL_1      kENET_PtpTimerChannel3
#define BOARD_ENET0_1588_TIMER_CHANNEL_2      kENET_PtpTimerChannel4
#else
#define BOARD_ENET1_1588_TIMER_CHANNEL_0      kENET_PtpTimerChannel1
#define BOARD_ENET1_1588_TIMER_CHANNEL_1      kENET_PtpTimerChannel3
#define BOARD_ENET1_1588_TIMER_CHANNEL_2      kENET_PtpTimerChannel4
#endif

#endif /* BOARD_USE_ENET */


#define BOARD_NUM_GPT           2

#define BOARD_GPT_0_BASE        GPT1
#define BOARD_GPT_0_IRQ         GPT1_IRQn
#define BOARD_GPT_0_IRQ_HANDLER GPT1_IRQHandler

#if BOARD_USE_ENET_QOS
#define BOARD_GPT_1_BASE        GPT3
#define BOARD_GPT_1_IRQ         GPT3_IRQn
#define BOARD_GPT_1_IRQ_HANDLER GPT3_IRQHandler

#define BOARD_GPT_REC_BASE      GPT3
#define BOARD_GPT_REC_CHANNEL   1
#else /* BOARD_USE_ENET_QOS */
#define BOARD_GPT_1_BASE        GPT2
#define BOARD_GPT_1_IRQ         GPT2_IRQn
#define BOARD_GPT_1_IRQ_HANDLER GPT2_IRQHandler

#define BOARD_GPT_REC_BASE      GPT2
#define BOARD_GPT_REC_CHANNEL   2
#endif /* BOARD_USE_ENET_QOS */

unsigned int BOARD_GPT_clk_src(void *base);
unsigned int BOARD_GPT_clk_src_div(void *base);
unsigned int BOARD_GPT_clk_freq(void *base);

int BOARD_NetPort_Get_MAC(unsigned int port, uint8_t *mac);

/* Time Gate Scheduling Table maximum gate control list length */
#define QBV_LIST_MAX_ENTRIES 0x200U

/*
 * 24MHz XTAL oscillator
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

#endif /* _GENAVB_SDK_INTERFACE_H_ */
