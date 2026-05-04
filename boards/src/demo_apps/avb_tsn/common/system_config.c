/*
 * Copyright 2018-2023, 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "genavb/helpers.h"

#include "rtos_apps/tsn/tsn_tasks_config.h"

#include "genavb.h"
#include "storage.h"
#include "system_config.h"

extern struct net_config system_net_cfg[CONFIG_APP_LOGICAL_PORTS];

void __system_config_get_net(const char *prefix, unsigned int port_id, struct net_config *net)
{
    char port[10];

    if (h_snprintf_strict(port, 10, "%s%u", prefix, port_id) < 0)
        goto out;

    if (storage_cd(port, true) == 0) {
        storage_read_mac_address("hw_addr", net[port_id].hw_addr);
        storage_read_ipv4_address("ip_addr", net[port_id].ip_addr);
        storage_read_ipv4_address("net_mask", net[port_id].net_mask);
        storage_read_ipv4_address("gw_addr", net[port_id].gw_addr);

        storage_cd("/", true);
    }

out:
    return;
}

void __system_config_get_tsn_app(const char *path, struct rtos_apps_tsn_config *tsn)
{
    if (storage_cd(path, true) == 0) {
        storage_read_uint("mode", &tsn->mode);
        storage_read_uint("role", &tsn->role);
        storage_read_uint("num_io_devices", &tsn->num_io_devices);
        storage_read_float("motor_offset", &tsn->motor_offset);
        storage_read_uint("control_strategy", &tsn->control_strategy);
        storage_read_uint("cmd_client", &tsn->cmd_client);

        if (tsn->mode == SERIAL)
            tsn->period_ns = APP_PERIOD_SERIAL_DEFAULT;

        storage_read_uint("period_ns", &tsn->period_ns);
        storage_read_uint("offset", &tsn->offset);
        storage_read_uint("network_budget_ns", &tsn->network_budget_ns);
        storage_read_uint("priority", &tsn->priority);
        storage_read_uint("tx_time_offset_ns", &tsn->tx_time_offset_ns);
        storage_read_bool("tx_time_enabled", &tsn->tx_time_enabled);
        storage_read_uint("port_id", &tsn->port_id);
        storage_read_uint("packets", &tsn->packets);
        storage_read_bool("zero_copy", &tsn->zero_copy);
        storage_read_uint("rx_tc_mask", &tsn->rx_tc_mask);

        storage_cd("-", true);
    }
}

struct net_config *system_config_get_net(unsigned int port_id)
{
    if (port_id >= CONFIG_APP_LOGICAL_PORTS)
        goto err;

    __system_config_get_net("/port", port_id, system_net_cfg);

    return &system_net_cfg[port_id];

err:
    return NULL;
}

extern struct avb_app_config system_avb_app_cfg;

struct avb_app_config *system_config_get_avb_app(void)
{
    struct avb_app_config *config = &system_avb_app_cfg;

    if (storage_cd("/avb_app", true) == 0) {
        storage_read_uint("mclock_role", &config->mclock_role);

        storage_cd("/", true);
    }

    return config;
}

#ifdef CONFIG_APP_TSN_ENDPOINT

extern struct rtos_apps_tsn_config system_tsn_app_cfg;

struct rtos_apps_tsn_config *system_config_get_tsn_app(void)
{
    struct rtos_apps_tsn_config *config = &system_tsn_app_cfg;

    __system_config_get_tsn_app("/tsn_app", config);

    return config;
}

#endif
