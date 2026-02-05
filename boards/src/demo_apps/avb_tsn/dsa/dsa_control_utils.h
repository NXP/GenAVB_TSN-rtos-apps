/*
* Copyright 2023 NXP
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef _FREERTOS_DSA_CONTROL_UTILS_H_
#define _FREERTOS_DSA_CONTROL_UTILS_H_

#include <stdint.h>

struct dsa_system_info {
	uint16_t device_id;
	uint16_t vendor_id;
	uint8_t sw_version_major;
	uint8_t sw_version_minor;
	uint8_t sw_version_revsion;
	uint8_t dsa_cpu_port;	/* DSA_NETC_SWITCH or DSA_NETC_ENETC */
};

void dsa_system_info_get(struct dsa_system_info *sys_info);

#endif /* _FREERTOS_DSA_CONTROL_UTILS_H_ */
