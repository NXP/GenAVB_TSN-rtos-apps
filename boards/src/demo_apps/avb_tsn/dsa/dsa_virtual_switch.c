/*
* Copyright 2024-2025 NXP
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#include "genavb/error.h"

#include "lib_virtual_switch.h"
#include "lib_port_genavb.h"

#include "avb_tsn/common/log.h"
#include "dsa_virtual_switch.h"

static char ptp_multicast_addr[6] = {0x01, 0x80, 0xc2, 0x00, 0x00, 0x0e};

#ifdef DSA_CPU_PORT_ENETC
struct virtual_switch virt_sw = {
	.ports = {
		[0] = {
			.cfg = {
				.name = "genavb_host_ep",
				.is_hostport = true,
				.ep_port_num = 1,
				.ep_port_id = {0},
				.flags = GENAVB_PORT_FLAGS_LOCAL_PORT,
			}
		},
		[1] = {
			.cfg = {
				.name = "genavb_switch",
				.is_hostport = false,
				.gptp_clock_id = GENAVB_CLOCK_BR_0_0,
				.ep_port_num = 1,
				.ep_port_id = {1},
				.bridge_port_num = 4,
				.bridge_port_id = {{2, 0}, {3, 1}, {4, 2}, {5, 3}},
				.flags = GENAVB_PORT_FLAGS_PTP_SOCKET | GENAVB_PORT_FLAGS_RX_INSERT_VLAN |
							GENAVB_PORT_FLAGS_RX_INSERT_DSA | GENAVB_PORT_FLAGS_TX_REMOVE_VLAN |
							GENAVB_PORT_FLAGS_TX_REMOVE_DSA,
			}
		},
	},
};
#else
struct virtual_switch virt_sw = {
	.ports = {
		[0] = {
			.cfg = {
				.name = "genavb_host_sw",
				.is_hostport = true,
				.ep_port_num = 0,
				.bridge_port_num = 1,
				.bridge_port_id = {{5, 3}},
				.flags = GENAVB_PORT_FLAGS_LOCAL_PORT,
			}
		},
		[1] = {
			.cfg = {
				.name = "genavb_switch",
				.is_hostport = false,
				.gptp_clock_id = GENAVB_CLOCK_BR_0_0,
				.ep_port_num = 1,
				.ep_port_id = {1},
				.bridge_port_num = 3,
				.bridge_port_id = {{2, 0}, {3, 1}, {4, 2}},
				.flags = GENAVB_PORT_FLAGS_PTP_SOCKET | GENAVB_PORT_FLAGS_RX_INSERT_VLAN |
							GENAVB_PORT_FLAGS_RX_INSERT_DSA | GENAVB_PORT_FLAGS_TX_REMOVE_VLAN |
							GENAVB_PORT_FLAGS_TX_REMOVE_DSA,
			}
		},
	},
};
#endif

int dsa_virtual_switch_init(void)
{
	int i;

	virt_sw.switch_dev = switch_init();
	if (!virt_sw.switch_dev) {
		log_err("Virtual switch init failed!\n");
		return -GENAVB_ERR_INVALID;
	}

	for (i = 0; i < CONFIG_MAX_VIRTUAL_SWITCH_PORTS; i++) {
		virt_sw.ports[i].port_dev = port_genavb_init(virt_sw.switch_dev, &virt_sw.ports[i].cfg);

		if (!virt_sw.ports[i].port_dev) {
			log_err("Virtual switch port %d init failed\n", virt_sw.ports[i].cfg.ep_port_id);
			return -GENAVB_ERR_INVALID;
		}
	}

	create_stats_thread(virt_sw.switch_dev);

	port_genavb_add_fdb_entry(virt_sw.ports[0].port_dev, ptp_multicast_addr);

	for (i = 0; i < CONFIG_MAX_VIRTUAL_SWITCH_PORTS; i++) {
		port_genavb_link_up(virt_sw.ports[i].port_dev, true);
	}

	return GENAVB_SUCCESS;
}

void dsa_stack_update_host_mac(void *mac_addr)
{
	port_genavb_update_addr(virt_sw.ports[0].port_dev, mac_addr);
	port_genavb_setup_host_mac(virt_sw.ports[1].port_dev, mac_addr);
}
