/*
 * Copyright 2023-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _GENAVB_SDK_NET_PORT_ENETC0_H_
#define _GENAVB_SDK_NET_PORT_ENETC0_H_

/*! @brief NET configuration */
#define BOARD_NUM_PORTS          2

#define BOARD_NUM_ENETC_PORTS    2
#define BOARD_NUM_NETC_SWITCHES  0
#define BOARD_NUM_NETC_PORTS     0

/* Default: Virtual Pseudo Endpoint, ENETC1/VSI0 */
#define BOARD_NET_PORT0_DRV_TYPE   ENETC_PSEUDO_1G_t
#define BOARD_NET_PORT0_DRV_INDEX  (0)
#define BOARD_NET_PORT0_DRV_BASE   kNETC_ENETC1VSI1
#define BOARD_NET_PORT0_PHY_INDEX  (-1)
#define BOARD_NET_PORT0_MII_MODE   kNETC_RgmiiMode
#define BOARD_NET_PORT0_HW_CLOCK   0
#define BOARD_NET_PORT0_TRAFFIC_CLASS_MAX   (4)
#define BOARD_NET_PORT0_SR_CLASS_MAX        (0)

/*  Standalone EP, ENETC0/PSI0, eth4, RMII, 100M, 8201 phy */
#define BOARD_NET_PORT1_DRV_TYPE   ENETC_1G_t
#define BOARD_NET_PORT1_DRV_INDEX  (1)
#define BOARD_NET_PORT1_DRV_BASE   kNETC_ENETC0PSI0
#define BOARD_NET_PORT1_PHY_INDEX  (0)
#define BOARD_NET_PORT1_MII_MODE   kNETC_RmiiMode
#define BOARD_NET_PORT1_HW_CLOCK   0
#define BOARD_NET_PORT1_TRAFFIC_CLASS_MAX   (4)
#define BOARD_NET_PORT1_SR_CLASS_MAX        (0)

#define BOARD_ENETC0_RX_ZERO_COPY

#define BOARD_NUM_MDIO                     1
#define BOARD_MDIO0_DRV_TYPE               NETC_PORT_EMDIO_t
#define BOARD_MDIO0_DRV_INDEX              0

#define BOARD_NUM_NETC_PORT_EMDIO          1
#define BOARD_NETC_MDIO_FREQ               (dev_get_net_core_freq(SW0_BASE))
#define BOARD_NETC_PORT_EMDIO_PORT0        kNETC_ENETC0EthPort

#define BOARD_NUM_PHY        1

#define BOARD_PHY0_MDIO_ID            0
#define BOARD_PHY0_ADDRESS            3
#define BOARD_PHY0_OPS                phyrtl8201_ops
#define BOARD_PHY0_RX_LATENCY_100M    744
#define BOARD_PHY0_TX_LATENCY_100M    3974

#endif /* _GENAVB_SDK_NET_PORT_ENETC0_H_ */
