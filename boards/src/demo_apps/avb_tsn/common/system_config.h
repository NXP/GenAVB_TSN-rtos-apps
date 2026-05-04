/*
 * Copyright 2018-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * System configuration
 */
#ifndef _SYSTEM_CONFIG_H_
#define _SYSTEM_CONFIG_H_

#include <stdint.h>

#include "rtos_apps/tsn/tsn_entry.h"

struct net_config {
    uint8_t hw_addr[6];
    uint8_t ip_addr[4];
    uint8_t net_mask[4];
    uint8_t gw_addr[4];
};

struct avb_app_config {
    unsigned int mclock_role;
};

struct audio_app_config {
};

void __system_config_get_net(const char *prefix, unsigned int port_id, struct net_config *net);
void __system_config_get_tsn_app(const char *path, struct rtos_apps_tsn_config *tsn);
struct net_config *system_config_get_net(unsigned int port_id);
struct avb_app_config *system_config_get_avb_app(void);

#ifdef CONFIG_APP_TSN_ENDPOINT
struct rtos_apps_tsn_config *system_config_get_tsn_app(void);
#endif

#endif /* _SYSTEM_CONFIG_H_ */
