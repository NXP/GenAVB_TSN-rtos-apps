/*
 * Copyright 2023-2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _GENAVB_SDK_H_
#define _GENAVB_SDK_H_

#include <stdarg.h>
#include <stdint.h>

#include "fsl_enet_qos.h"
#include "fsl_stm.h"
#include "fsl_phy.h"

/*! @brief NET configuration */

#define BOARD_NET_RX_PACKETS 2

#define BOARD_NUM_PORTS      1

#define BOARD_NUM_ENET_QOS_PORTS    1

#define BOARD_NET_PORT0_MII_MODE    kENET_QOS_RmiiMode
#define BOARD_NET_PORT0_DRV_TYPE    ENET_QOS_t
#define BOARD_NET_PORT0_DRV_BASE    ENET_QOS_BASE
#define BOARD_NET_PORT0_DRV_INDEX   (0)
#define BOARD_NET_PORT0_PHY_INDEX   (0)
#define BOARD_NET_PORT0_SPEED       kPHY_Speed100M
#define BOARD_NET_PORT0_DUPLEX      kPHY_FullDuplex
#define BOARD_NET_PORT0_1588_TIMER_EVENT_CHANNEL  kENET_QOS_PtpPpsIstance1
#define BOARD_NET_PORT0_TRAFFIC_CLASS_MAX         (2)
#define BOARD_NET_PORT0_SR_CLASS_MAX              (0)
#define BOARD_NET_PORT0_PRIORITY_TX_MAP           {0, 0, 0, 0, 0, 1, 0, 0}

#define BOARD_NET_TX_CACHEABLE
#define BOARD_NET_RX_CACHEABLE

/*! @brief ENET QOS configuration */
#define BOARD_ENET_QOS0_TXQ_NUM      2
#define BOARD_ENET_QOS0_RXQ_NUM      2
#define BOARD_ENET_QOS0_CBS_NUM      0

#define BOARD_ENET_QOS0_1588_TIMER_CHANNEL_0      kENET_QOS_PtpPpsIstance0
#define BOARD_ENET_QOS0_1588_TIMER_CHANNEL_1      kENET_QOS_PtpPpsIstance2
#define BOARD_ENET_QOS0_1588_TIMER_CHANNEL_2      kENET_QOS_PtpPpsIstance3
#define BOARD_ENET_QOS0_1588_TIMER_PPS            kENET_QOS_PtpPpsIstance3 /* Identify timer channel with PPS output */

/*! @brief MDIO configuration */
#define BOARD_NUM_MDIO          1
#define BOARD_MDIO0_DRV_TYPE    ENET_QOS_t
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
#define BOARD_PHY0_RX_LATENCY_100M (600)
#define BOARD_PHY0_TX_LATENCY_100M (600)

#define BOARD_NUM_STM             1

#define BOARD_STM_0_BASE        STM_0
#define BOARD_STM_0_IRQ         STM0_IRQn
#define BOARD_STM_0_IRQ_HANDLER STM0_IRQHandler

unsigned int BOARD_STM_clk_freq(void *base);

int BOARD_NetPort_Get_MAC(unsigned int port, uint8_t *mac);

/* Time Gate Scheduling Table maximum gate control list length */
#define QBV_LIST_MAX_ENTRIES 0x200U

/*
 * Primary clock source for all PLLs
 */
uint32_t dev_get_pll_ref_freq(void);

/*
 * Enet module frequency (ipg_clk)
 */
uint32_t dev_get_enet_core_freq(void *base);

/*
 * Enet 1588 timer frequency (ipg_clk_time)
 */
uint32_t dev_get_enet_1588_freq(void *base);

#endif /* _GENAVB_SDK_INTERFACE_H_ */
