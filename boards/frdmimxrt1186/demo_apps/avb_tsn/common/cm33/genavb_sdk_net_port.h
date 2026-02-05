/*
 * Copyright 2023-2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _GENAVB_SDK_NET_PORT_H_
#define _GENAVB_SDK_NET_PORT_H_

#include "fsl_netc.h"
#include "fsl_netc_soc.h"
#include "fsl_gpt.h"
#include "fsl_phy.h"

/*! @brief NET configuration */
#define BOARD_NUM_PORTS          4

#define BOARD_NUM_ENETC_PORTS    1
#define BOARD_NUM_NETC_SWITCHES  1
#define BOARD_NUM_NETC_PORTS     5

/* EP PSEUDO MAC, ENETC1/PSI0 */
#define BOARD_NET_PORT0_DRV_TYPE    ENETC_PSEUDO_1G_t
#define BOARD_NET_PORT0_DRV_INDEX   (0)
#define BOARD_NET_PORT0_DRV_BASE    kNETC_ENETC1PSI0
#define BOARD_NET_PORT0_PHY_INDEX   (-1)
#define BOARD_NET_PORT0_MII_MODE    kNETC_RmiiMode
#define BOARD_NET_PORT0_HW_CLOCK    0
#define BOARD_NET_PORT0_TRAFFIC_CLASS_MAX   (4)
#define BOARD_NET_PORT0_SR_CLASS_MAX        (0)

/* BRIDGE0 */
#define BOARD_NET_BRIDGE0_DRV_INDEX        0
#define BOARD_NET_BRIDGE0_DRV_TYPE         NETC_SW_t

/* BRIDGE0, eth2, RGMII, 1G,  8211 phy */
#define BOARD_NET_PORT1_DRV_TYPE    BOARD_NET_BRIDGE0_DRV_TYPE
#define BOARD_NET_PORT1_DRV_INDEX   BOARD_NET_BRIDGE0_DRV_INDEX
#define BOARD_NET_PORT1_DRV_BASE    (0)
#define BOARD_NET_PORT1_PHY_INDEX   (0)
#define BOARD_NET_PORT1_MII_MODE    kNETC_RgmiiMode
#define BOARD_NET_PORT1_HW_CLOCK    0

/* BRIDGE0, eth3, RGMII, 1G,  8211 phy */
#define BOARD_NET_PORT2_DRV_TYPE    BOARD_NET_BRIDGE0_DRV_TYPE
#define BOARD_NET_PORT2_DRV_INDEX   BOARD_NET_BRIDGE0_DRV_INDEX
#define BOARD_NET_PORT2_DRV_BASE    (2)
#define BOARD_NET_PORT2_PHY_INDEX   (1)
#define BOARD_NET_PORT2_MII_MODE    kNETC_RgmiiMode
#define BOARD_NET_PORT2_HW_CLOCK    0

/* BRIDGE0, Host Port */
#define BOARD_NET_PORT3_DRV_TYPE    BOARD_NET_BRIDGE0_DRV_TYPE
#define BOARD_NET_PORT3_DRV_INDEX   BOARD_NET_BRIDGE0_DRV_INDEX
#define BOARD_NET_PORT3_DRV_BASE    (4)
#define BOARD_NET_PORT3_PHY_INDEX   (-1)
#define BOARD_NET_PORT3_MII_MODE    kNETC_RgmiiMode
#define BOARD_NET_PORT3_HW_CLOCK    0

#define BOARD_NUM_MDIO                1
#define BOARD_MDIO0_DRV_TYPE          NETC_EMDIO_t
#define BOARD_MDIO0_DRV_INDEX         0

#define BOARD_NUM_NETC_EMDIO          1
#define BOARD_NETC_MDIO_FREQ          (dev_get_net_core_freq(SW0_BASE))

#define BOARD_NUM_PHY                 2

/* PHY delay compensation values for Tx/Rx timestamps, taking into account PHY
 * delay TX/RX asymmetry and including a 50ns margin for the propagation delay of
 * small cables (to avoid negative propagation delay measurements due to
 * propagation delay jitter)
 */

#define BOARD_PHY0_MDIO_ID            0
#define BOARD_PHY0_ADDRESS            5
#define BOARD_PHY0_OPS                phyyt8521_ops
#define BOARD_PHY0_RX_LATENCY_100M    1000
#define BOARD_PHY0_TX_LATENCY_100M    1000
#define BOARD_PHY0_RX_LATENCY_1G      154
#define BOARD_PHY0_TX_LATENCY_1G      154

#define BOARD_PHY1_MDIO_ID            0
#define BOARD_PHY1_ADDRESS            4
#define BOARD_PHY1_OPS                phyyt8521_ops
#define BOARD_PHY1_RX_LATENCY_100M    1000
#define BOARD_PHY1_TX_LATENCY_100M    1000
#define BOARD_PHY1_RX_LATENCY_1G      154
#define BOARD_PHY1_TX_LATENCY_1G      154

#endif /* _GENAVB_SDK_NET_PORT_H_ */
