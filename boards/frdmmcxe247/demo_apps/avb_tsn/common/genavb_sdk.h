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
#include "fsl_phy.h"

/*! @brief NET configuration */

#define BOARD_NET_RX_PACKETS 2

#define BOARD_NUM_PORTS      1

#define BOARD_NUM_ENET_PORTS    1

#define BOARD_NET_PORT0_MII_MODE    kENET_RmiiMode
#define BOARD_NET_PORT0_DRV_TYPE    ENET_t
#define BOARD_NET_PORT0_DRV_BASE    ENET_BASE
#define BOARD_NET_PORT0_DRV_INDEX   (0)
#define BOARD_NET_PORT0_PHY_INDEX   (0)
#define BOARD_NET_PORT0_SPEED       kPHY_Speed100M
#define BOARD_NET_PORT0_DUPLEX      kPHY_FullDuplex
#define BOARD_NET_PORT0_1588_TIMER_EVENT_CHANNEL  kENET_PtpTimerChannel1
#define BOARD_NET_PORT0_TRAFFIC_CLASS_MAX         (5)
#define BOARD_NET_PORT0_SR_CLASS_MAX              (0)

#define BOARD_ENET0_1588_TIMER_CHANNEL_0      kENET_PtpTimerChannel2
#define BOARD_ENET0_1588_TIMER_CHANNEL_1      kENET_PtpTimerChannel3
#define BOARD_ENET0_1588_TIMER_CHANNEL_2      kENET_PtpTimerChannel4

/*! @brief MDIO configuration */
#define BOARD_NUM_MDIO          1
#define BOARD_MDIO0_DRV_TYPE    ENET_t
#define BOARD_MDIO0_DRV_INDEX   0

/* PHY configuration */
#define BOARD_NUM_PHY           1

/* PHY 0 is LAN8741A phy */
#define BOARD_PHY0_MDIO_ID      0
#define BOARD_PHY0_ADDRESS      (0x0U)             /* Phy address of port 0. */
#define BOARD_PHY0_OPS          phylan8741_ops     /* PHY operations. */

/* PHY delay compensation values for Tx/Rx timestamps, taking into account PHY
 * delay TX/RX asymmetry and including a 50ns margin for the propagation delay of
 * small cables (to avoid negative propagation delay measurements due to
 * propagation delay jitter)
 */

 /* TODO */
#define BOARD_PHY0_RX_LATENCY_100M (0)
#define BOARD_PHY0_TX_LATENCY_100M (0)

#define BOARD_NUM_FTM             1

#define BOARD_FTM_0_BASE               FTM0

#define BOARD_FTM_0_PRESCALE           kFTM_Prescale_Divide_1

#define BOARD_FTM_NUM_IRQS             4

#define BOARD_FTM_0_CH01_IRQ           FTM0_Ch0_Ch1_IRQn
#define BOARD_FTM_0_CH01_IRQ_HANDLER   FTM0_Ch0_Ch1_IRQHandler

#if BOARD_FTM_NUM_IRQS > 1
#define BOARD_FTM_0_CH23_IRQ           FTM0_Ch2_Ch3_IRQn
#define BOARD_FTM_0_CH23_IRQ_HANDLER   FTM0_Ch2_Ch3_IRQHandler
#endif

#if BOARD_FTM_NUM_IRQS > 2
#define BOARD_FTM_0_CH45_IRQ           FTM0_Ch4_Ch5_IRQn
#define BOARD_FTM_0_CH45_IRQ_HANDLER   FTM0_Ch4_Ch5_IRQHandler
#endif

#if BOARD_FTM_NUM_IRQS > 3
#define BOARD_FTM_0_CH67_IRQ           FTM0_Ch6_Ch7_IRQn
#define BOARD_FTM_0_CH67_IRQ_HANDLER   FTM0_Ch6_Ch7_IRQHandler
#endif

unsigned int BOARD_FTM_clk_freq(void *base);
unsigned int BOARD_FTM_clk_src(void *base);

int BOARD_NetPort_Get_MAC(unsigned int port, uint8_t *mac);

/*
 * Enet module frequency (ipg_clk)
 */
uint32_t dev_get_enet_core_freq(void *base);

/*
 * Enet 1588 timer frequency (ipg_clk_time)
 */
uint32_t dev_get_enet_1588_freq(void *base);

#endif /* _GENAVB_SDK_H_ */
