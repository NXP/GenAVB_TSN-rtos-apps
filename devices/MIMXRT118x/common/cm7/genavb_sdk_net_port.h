/*
 * Copyright 2023-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _GENAVB_SDK_NET_PORT_H_
#define _GENAVB_SDK_NET_PORT_H_

/*! @brief NET configuration */
#define BOARD_NUM_PORTS          1

#define BOARD_NUM_ENETC_PORTS    1
#define BOARD_NUM_NETC_SWITCHES  0
#define BOARD_NUM_NETC_PORTS     0

/*  Virtual Pseudo Endpoint, ENETC1/VSI0 */
#define BOARD_NET_PORT0_DRV_TYPE   ENETC_PSEUDO_1G_t
#define BOARD_NET_PORT0_DRV_INDEX  (0)
#define BOARD_NET_PORT0_DRV_BASE   kNETC_ENETC1VSI1
#define BOARD_NET_PORT0_PHY_INDEX  (-1)
#define BOARD_NET_PORT0_MII_MODE   kNETC_RgmiiMode
#define BOARD_NET_PORT0_HW_CLOCK   0
#define BOARD_NET_PORT0_TRAFFIC_CLASS_MAX   (4)
#define BOARD_NET_PORT0_SR_CLASS_MAX        (0)

#define BOARD_ENETC0_RX_ZERO_COPY

#define BOARD_NUM_MDIO       0

#define BOARD_NUM_PHY        0

#endif /* _GENAVB_SDK_NET_PORT_H_ */
