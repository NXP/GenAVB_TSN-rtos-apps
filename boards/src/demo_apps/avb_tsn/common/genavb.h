/*
 * Copyright 2018-2020, 2022-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _GENAVB_H_
#define _GENAVB_H_

#include "genavb/init.h"
#include "genavb/clock.h"


#ifndef CONFIG_APP_EP_NUM_PORTS
#define CONFIG_APP_EP_NUM_PORTS   1
#endif

#ifndef CONFIG_APP_EP_LOGICAL_PORT_LIST
#define CONFIG_APP_EP_LOGICAL_PORT_LIST   {0}
#endif

#ifdef CONFIG_APP_TSN_BRIDGE

#ifndef CONFIG_APP_BR_NUM_PORTS
#define CONFIG_APP_BR_NUM_PORTS   5
#endif

#ifndef CONFIG_APP_BR_LOGICAL_PORT_LIST
#define CONFIG_APP_BR_LOGICAL_PORT_LIST   {2, 3, 4, 5, 6}
#endif

#define CONFIG_APP_LOGICAL_PORTS	(CFG_MAX_ENDPOINTS + CONFIG_APP_BR_NUM_PORTS)

#else

#define CONFIG_APP_BR_NUM_PORTS    0
#define CONFIG_APP_BR_LOGICAL_PORT_LIST   {}

#define CONFIG_APP_LOGICAL_PORTS	CONFIG_APP_EP_NUM_PORTS

#endif


struct gavb_pps {
    genavb_clock_id_t clk_id;
    struct genavb_timer *t;
};

extern const unsigned int br_port_list[CONFIG_APP_BR_NUM_PORTS];

int gavb_pps_init(struct gavb_pps *pps, genavb_clock_id_t clk_id);
int gavb_log_level(char *component_str, char *level_str);
struct genavb_handle *get_genavb_handle(void);
int gavb_stack_init(void);
int gavb_stack_exit(void);

#endif /* _GENAVB_H_ */
