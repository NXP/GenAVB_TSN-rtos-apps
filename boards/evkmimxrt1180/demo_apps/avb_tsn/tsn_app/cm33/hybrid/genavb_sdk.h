/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _GENAVB_SDK_HYBRID_H_
#define _GENAVB_SDK_HYBRID_H_

#include "fsl_netc.h"
#include "fsl_netc_soc.h"
#include "fsl_gpt.h"
#include "fsl_phy.h"

/*! @brief NET configuration */
#define BOARD_NUM_PORTS          5

#define BOARD_NUM_ENETC_PORTS    2
#define BOARD_NUM_NETC_SWITCHES  1
#define BOARD_NUM_NETC_PORTS     5

/*  Standalone EP, ENETC0/PSI0, eth4, RMII, 100M, 8201 phy */
#define BOARD_NET_PORT0_DRV_TYPE   ENETC_1G_t
#define BOARD_NET_PORT0_DRV_INDEX  (0)
#define BOARD_NET_PORT0_DRV_BASE   kNETC_ENETC0PSI0
#define BOARD_NET_PORT0_PHY_INDEX  (2)
#define BOARD_NET_PORT0_MII_MODE   kNETC_RmiiMode
#define BOARD_NET_PORT0_HW_CLOCK   0
#define BOARD_NET_PORT0_TRAFFIC_CLASS_MAX   (4)
#define BOARD_NET_PORT0_SR_CLASS_MAX        (0)

/* EP PSEUDO MAC, ENETC1/PSI0 */
#define BOARD_NET_PORT1_DRV_TYPE    ENETC_PSEUDO_1G_t
#define BOARD_NET_PORT1_DRV_INDEX   (1)
#define BOARD_NET_PORT1_DRV_BASE    kNETC_ENETC1PSI0
#define BOARD_NET_PORT1_PHY_INDEX   (-1)
#define BOARD_NET_PORT1_MII_MODE    kNETC_RmiiMode
#define BOARD_NET_PORT1_HW_CLOCK    0
#define BOARD_NET_PORT1_TRAFFIC_CLASS_MAX   (4)
#define BOARD_NET_PORT1_SR_CLASS_MAX        (0)

/* BRIDGE0 */
#define BOARD_NET_BRIDGE0_DRV_INDEX        0
#define BOARD_NET_BRIDGE0_DRV_TYPE         NETC_SW_t

/* BRIDGE0, eth2, RGMII, 1G,  8211 phy */
#define BOARD_NET_PORT2_DRV_TYPE    BOARD_NET_BRIDGE0_DRV_TYPE
#define BOARD_NET_PORT2_DRV_INDEX   BOARD_NET_BRIDGE0_DRV_INDEX
#define BOARD_NET_PORT2_DRV_BASE    (2)
#define BOARD_NET_PORT2_PHY_INDEX   (0)
#define BOARD_NET_PORT2_MII_MODE    kNETC_RgmiiMode
#define BOARD_NET_PORT2_HW_CLOCK    0

/* BRIDGE0, eth3, RGMII, 1G,  8211 phy */
#define BOARD_NET_PORT3_DRV_TYPE    BOARD_NET_BRIDGE0_DRV_TYPE
#define BOARD_NET_PORT3_DRV_INDEX   BOARD_NET_BRIDGE0_DRV_INDEX
#define BOARD_NET_PORT3_DRV_BASE    (3)
#define BOARD_NET_PORT3_PHY_INDEX   (1)
#define BOARD_NET_PORT3_MII_MODE    kNETC_RgmiiMode
#define BOARD_NET_PORT3_HW_CLOCK    0

/* BRIDGE0, Host Port */
#define BOARD_NET_PORT4_DRV_TYPE    BOARD_NET_BRIDGE0_DRV_TYPE
#define BOARD_NET_PORT4_DRV_INDEX   BOARD_NET_BRIDGE0_DRV_INDEX
#define BOARD_NET_PORT4_DRV_BASE    (4)
#define BOARD_NET_PORT4_PHY_INDEX   (-1)
#define BOARD_NET_PORT4_MII_MODE    kNETC_RgmiiMode
#define BOARD_NET_PORT4_HW_CLOCK    0

#define BOARD_ENETC0_TS_FREERUNNING   1

#define BOARD_NUM_MDIO                1
#define BOARD_MDIO0_DRV_TYPE          NETC_EMDIO_t
#define BOARD_MDIO0_DRV_INDEX         0

#define BOARD_NUM_NETC_EMDIO          1
#define BOARD_NETC_MDIO_FREQ          (dev_get_net_core_freq(SW0_BASE))

#define BOARD_NUM_PHY                 3

/* PHY delay compensation values for Tx/Rx timestamps, taking into account PHY
 * delay TX/RX asymmetry and including a 50ns margin for the propagation delay of
 * small cables (to avoid negative propagation delay measurements due to
 * propagation delay jitter)
 */

#define BOARD_PHY0_MDIO_ID            0
#define BOARD_PHY0_ADDRESS            4
#define BOARD_PHY0_OPS                phyrtl8211f_ops
#define BOARD_PHY0_RX_LATENCY_100M    733
#define BOARD_PHY0_TX_LATENCY_100M    1297
#define BOARD_PHY0_RX_LATENCY_1G      530
#define BOARD_PHY0_TX_LATENCY_1G      202

#define BOARD_PHY1_MDIO_ID            0
#define BOARD_PHY1_ADDRESS            7
#define BOARD_PHY1_OPS                phyrtl8211f_ops
#define BOARD_PHY1_RX_LATENCY_100M    733
#define BOARD_PHY1_TX_LATENCY_100M    1297
#define BOARD_PHY1_RX_LATENCY_1G      530
#define BOARD_PHY1_TX_LATENCY_1G      202

#define BOARD_PHY2_MDIO_ID            0
#define BOARD_PHY2_ADDRESS            3
#define BOARD_PHY2_OPS                phyrtl8201_ops
#define BOARD_PHY2_RX_LATENCY_100M    744
#define BOARD_PHY2_TX_LATENCY_100M    3974
#define BOARD_PHY2_FIXUP              BOARD_Phy8201_fixup

/*! @brief PORT Link configuration */
/* The configuration must be aligned with the MDIO/PHY configuration.*/

#define BOARD_NUM_LINK                0

#define BOARD_NUM_NETC_HW_CLOCK       1

#define BOARD_NETC_HW_CLOCK_0_ID      0
#define BOARD_NETC_HW_CLOCK_0_OWNER   1
#define BOARD_NETC_HW_CLOCK_0_SELECT  kNETC_TimerExtRefClk
#define BOARD_NETC_HW_CLOCK_0_FREQ    (dev_get_net_1588_freq(SW0_BASE))
#define BOARD_NETC_HW_CLOCK_0_MSGINTR_BASE MSGINTR1
#define BOARD_NETC_HW_CLOCK_0_MSGINTR_CH 0
#define BOARD_HW_CLOCK0_NUM_TIMERS    2
#define BOARD_HW_CLOCK0_TIMER0_ID     1 /* Alarm2 */
#define BOARD_HW_CLOCK0_TIMER0_ENABLED
#define BOARD_HW_CLOCK0_TIMER0_IRQ_ENABLED
#define BOARD_HW_CLOCK0_TIMER1_ID     2 /* Fiper1 */
#define BOARD_HW_CLOCK0_TIMER1_ENABLED
#define BOARD_HW_CLOCK0_TIMER1_PPS

#define BOARD_NUM_GPT        1

#define BOARD_GPT_0_BASE        GPT1
#define BOARD_GPT_0_IRQ         GPT1_IRQn
#define BOARD_GPT_0_IRQ_HANDLER GPT1_IRQHandler
#define BOARD_GPT_0_PRESCALE    3

/*! @brief MSGINTR block configuration */
#define BOARD_NUM_MSGINTR       1
#define BOARD_MSGINTR0_BASE MSGINTR1

#define QBV_LIST_MAX_ENTRIES NETC_TB_TGS_MAX_ENTRY

unsigned int BOARD_GPT_clk_src(void *base);
unsigned int BOARD_GPT_clk_freq(void *base);

int BOARD_NetPort_Get_MAC(unsigned int port, uint8_t *mac);

uint32_t dev_get_net_core_freq(void *base);

uint32_t dev_get_net_1588_freq(void *base);

int BOARD_Phy8201_fixup(phy_handle_t *handle);

#endif /* _GENAVB_SDK_HYBRID_H_ */
