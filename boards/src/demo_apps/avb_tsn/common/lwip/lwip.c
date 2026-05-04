/*
 * Copyright 2018-2020, 2023, 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifdef CONFIG_APP_LWIP
#include "FreeRTOS.h"
#include "lwip/opt.h"
#include "lwip/tcpip.h"

#include "lwip_ethernetif.h"

#include "avb_tsn/common/log.h"
#include "avb_tsn/common/system_config.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
void lwip_stack_init(void)
{
    static struct netif fsl_netif0;
    const struct net_config *net_cfg;
    ip4_addr_t fsl_netif0_ipaddr, fsl_netif0_netmask, fsl_netif0_gw;
    int port_id = LWIP_DEFAULT_LOGICAL_PORT;

    net_cfg = system_config_get_net(port_id);
    if (!net_cfg) {
        log_err("system_config_get_net() failed\n");
        return;
    }

    IP4_ADDR(&fsl_netif0_ipaddr, net_cfg->ip_addr[0], net_cfg->ip_addr[1], net_cfg->ip_addr[2], net_cfg->ip_addr[3]);
    IP4_ADDR(&fsl_netif0_netmask, net_cfg->net_mask[0], net_cfg->net_mask[1], net_cfg->net_mask[2], net_cfg->net_mask[3]);
    IP4_ADDR(&fsl_netif0_gw, net_cfg->gw_addr[0], net_cfg->gw_addr[1], net_cfg->gw_addr[2], net_cfg->gw_addr[3]);

    tcpip_init(NULL, NULL);

    netif_add(&fsl_netif0, &fsl_netif0_ipaddr, &fsl_netif0_netmask, &fsl_netif0_gw,
              NULL, &ethernetif_init, &tcpip_input);
    netif_set_default(&fsl_netif0);
    netif_set_up(&fsl_netif0);

    log_info("************************************************\r\n");
    log_info(" IPv4 Address     : %u.%u.%u.%u\n", ((u8_t *)&fsl_netif0_ipaddr)[0],
        ((u8_t *)&fsl_netif0_ipaddr)[1],
        ((u8_t *)&fsl_netif0_ipaddr)[2],
        ((u8_t *)&fsl_netif0_ipaddr)[3]);
    log_info(" IPv4 Subnet mask : %u.%u.%u.%u\n", ((u8_t *)&fsl_netif0_netmask)[0],
        ((u8_t *)&fsl_netif0_netmask)[1],
        ((u8_t *)&fsl_netif0_netmask)[2],
        ((u8_t *)&fsl_netif0_netmask)[3]);
    log_info(" IPv4 Gateway     : %u.%u.%u.%u\n", ((u8_t *)&fsl_netif0_gw)[0],
        ((u8_t *)&fsl_netif0_gw)[1],
        ((u8_t *)&fsl_netif0_gw)[2],
        ((u8_t *)&fsl_netif0_gw)[3]);
    log_info(" HW Addr          : "MAC_STR_FMT"\n", MAC_STR(fsl_netif0.hwaddr));
    log_info("************************************************\r\n");
}

#else
void lwip_stack_init(void) {return;}
#endif
