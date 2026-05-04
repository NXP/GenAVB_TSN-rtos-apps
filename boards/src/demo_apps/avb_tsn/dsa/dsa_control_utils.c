/*
 * Copyright 2023-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdlib.h>
#include <string.h>

#include "fsl_common.h"

#include "genavb/init.h"
#include "genavb/helpers.h"

#include "dsa_control.h"
#include "dsa_control_utils.h"

#define STRING_SIZE 64

/* PCI device ID and vendor ID register (PCI_CFH_DID_VID) bits[31:16] */
static uint16_t dsa_device_id_get(void)
{
	IERC_PCI_Type *ierc_pci_base = IERC_F0_PCI_HDR_TYPE0;

	return (uint16_t)((ierc_pci_base->PCI_CFH_DID_VID & IERC_PCI_PCI_CFH_DID_VID_DEVICE_ID_MASK) >> IERC_PCI_PCI_CFH_DID_VID_DEVICE_ID_SHIFT);
}

/* PCI device ID and vendor ID register (PCI_CFH_DID_VID) bits[15:0] */
static uint16_t dsa_vendor_id_get(void)
{
	IERC_PCI_Type *ierc_pci_base = IERC_F0_PCI_HDR_TYPE0;

	return (uint16_t)((ierc_pci_base->PCI_CFH_DID_VID & IERC_PCI_PCI_CFH_DID_VID_VENDOR_ID_MASK));
}

void dsa_system_info_get(struct dsa_system_info *sys_info)
{
	char version[STRING_SIZE];	/* i.e. "5_11_0" */
	unsigned long val;
	char *delim = "_";
	char *token;

	memset(sys_info, 0, sizeof(struct dsa_system_info));

	sys_info->device_id = dsa_device_id_get();
	sys_info->vendor_id = dsa_vendor_id_get();

	h_strncpy(version, genavb_version(), STRING_SIZE);
	token = strtok(version, delim);
	if (token) {
		h_strtoul(&val, token, NULL, 0);
		sys_info->sw_version_major = val;

		token = strtok(NULL, delim);
		if (token) {
			h_strtoul(&val, token, NULL, 0);
			sys_info->sw_version_minor = val;

			token = strtok(NULL, delim);
			if (token) {
				h_strtoul(&val, token, NULL, 0);
				sys_info->sw_version_revsion = val;
			}
		}
	}

#if defined(DSA_CPU_PORT_NETC_SWITCH)
	sys_info->dsa_cpu_port = DSA_NETC_SWITCH;
#elif defined(DSA_CPU_PORT_ENETC)
	sys_info->dsa_cpu_port = DSA_NETC_ENETC;
#else
	sys_info->dsa_cpu_port = DSA_NETC_NONE;
#endif
}
