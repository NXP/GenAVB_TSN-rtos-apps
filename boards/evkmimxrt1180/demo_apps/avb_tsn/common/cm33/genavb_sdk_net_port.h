/*
 * Copyright 2023-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _GENAVB_SDK_NET_PORT_H_
#define _GENAVB_SDK_NET_PORT_H_

/*! @brief NET configuration */
#define BOARD_NUM_PORTS          7

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

/* BRIDGE0, eth0, RMII, 100M, 8201 phy */
#define BOARD_NET_PORT2_DRV_TYPE    BOARD_NET_BRIDGE0_DRV_TYPE
#define BOARD_NET_PORT2_DRV_INDEX   BOARD_NET_BRIDGE0_DRV_INDEX
#define BOARD_NET_PORT2_DRV_BASE    (0) /* index of this port within bridge0 */
#define BOARD_NET_PORT2_PHY_INDEX   (0)
#define BOARD_NET_PORT2_MII_MODE    kNETC_RmiiMode
#define BOARD_NET_PORT2_HW_CLOCK    0

/* BRIDGE0, eth1, RGMII, 1G,  8211 phy */
#define BOARD_NET_PORT3_DRV_TYPE    BOARD_NET_BRIDGE0_DRV_TYPE
#define BOARD_NET_PORT3_DRV_INDEX   BOARD_NET_BRIDGE0_DRV_INDEX
#define BOARD_NET_PORT3_DRV_BASE    (1)
#define BOARD_NET_PORT3_PHY_INDEX   (1)
#define BOARD_NET_PORT3_MII_MODE    kNETC_RgmiiMode
#define BOARD_NET_PORT3_HW_CLOCK    0

/* BRIDGE0, eth2, RGMII, 1G,  8211 phy */
#define BOARD_NET_PORT4_DRV_TYPE    BOARD_NET_BRIDGE0_DRV_TYPE
#define BOARD_NET_PORT4_DRV_INDEX   BOARD_NET_BRIDGE0_DRV_INDEX
#define BOARD_NET_PORT4_DRV_BASE    (2)
#define BOARD_NET_PORT4_PHY_INDEX   (3)
#define BOARD_NET_PORT4_MII_MODE    kNETC_RgmiiMode
#define BOARD_NET_PORT4_HW_CLOCK    0

/* BRIDGE0, eth3, RGMII, 1G,  8211 phy */
#define BOARD_NET_PORT5_DRV_TYPE    BOARD_NET_BRIDGE0_DRV_TYPE
#define BOARD_NET_PORT5_DRV_INDEX   BOARD_NET_BRIDGE0_DRV_INDEX
#define BOARD_NET_PORT5_DRV_BASE    (3)
#define BOARD_NET_PORT5_PHY_INDEX   (4)
#define BOARD_NET_PORT5_MII_MODE    kNETC_RgmiiMode
#define BOARD_NET_PORT5_HW_CLOCK    0

/* BRIDGE0, Host Port */
#define BOARD_NET_PORT6_DRV_TYPE    BOARD_NET_BRIDGE0_DRV_TYPE
#define BOARD_NET_PORT6_DRV_INDEX   BOARD_NET_BRIDGE0_DRV_INDEX
#define BOARD_NET_PORT6_DRV_BASE    (4)
#define BOARD_NET_PORT6_PHY_INDEX   (-1)
#define BOARD_NET_PORT6_MII_MODE    kNETC_RgmiiMode
#define BOARD_NET_PORT6_HW_CLOCK    0

#define BOARD_NUM_MDIO                1
#define BOARD_MDIO0_DRV_TYPE          NETC_EMDIO_t
#define BOARD_MDIO0_DRV_INDEX         0

#define BOARD_NUM_NETC_EMDIO          1
#define BOARD_NETC_MDIO_FREQ          (dev_get_net_core_freq(SW0_BASE))

#define BOARD_NUM_PHY                 5

/* PHY delay compensation values for Tx/Rx timestamps, taking into account PHY
 * delay TX/RX asymmetry and including a 50ns margin for the propagation delay of
 * small cables (to avoid negative propagation delay measurements due to
 * propagation delay jitter)
 */

#define BOARD_PHY0_MDIO_ID            0
#define BOARD_PHY0_ADDRESS            2
#define BOARD_PHY0_OPS                phyrtl8201_ops
#define BOARD_PHY0_RX_LATENCY_100M    744
#define BOARD_PHY0_TX_LATENCY_100M    3974
#define BOARD_PHY0_FIXUP              BOARD_Phy8201_fixup

#define BOARD_PHY1_MDIO_ID            0
#define BOARD_PHY1_ADDRESS            5
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

#define BOARD_PHY3_MDIO_ID            0
#define BOARD_PHY3_ADDRESS            4
#define BOARD_PHY3_OPS                phyrtl8211f_ops
#define BOARD_PHY3_RX_LATENCY_100M    733
#define BOARD_PHY3_TX_LATENCY_100M    1297
#define BOARD_PHY3_RX_LATENCY_1G      530
#define BOARD_PHY3_TX_LATENCY_1G      202

#define BOARD_PHY4_MDIO_ID            0
#define BOARD_PHY4_ADDRESS            7
#define BOARD_PHY4_OPS                phyrtl8211f_ops
#define BOARD_PHY4_RX_LATENCY_100M    733
#define BOARD_PHY4_TX_LATENCY_100M    1297
#define BOARD_PHY4_RX_LATENCY_1G      530
#define BOARD_PHY4_TX_LATENCY_1G      202

#endif /* _GENAVB_SDK_NET_PORT_H_ */
