/*
* Copyright 2024 NXP
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef _FREERTOS_DSA_VIRTUAL_SWITCH_H_
#define _FREERTOS_DSA_VIRTUAL_SWITCH_H_

#include "lib_port_genavb.h"

#define CONFIG_MAX_VIRTUAL_SWITCH_PORTS	2

struct virtual_switch_port {
	void *port_dev;
	struct genavb_port_cfg cfg;
};

struct virtual_switch {
	void *switch_dev;
	struct virtual_switch_port ports[CONFIG_MAX_VIRTUAL_SWITCH_PORTS];
};

extern struct virtual_switch virt_sw;

int dsa_virtual_switch_init(void);
void dsa_stack_update_host_mac(void *mac_addr);

#endif /* _FREERTOS_DSA_VIRTUAL_SWITCH_H_ */
