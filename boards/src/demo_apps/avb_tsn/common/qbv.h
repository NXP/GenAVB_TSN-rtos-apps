/*
 * Copyright 2022-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __QBV_H__
#define __QBV_H__

#include "genavb/scheduled_traffic.h"
#include "shell.h"

void qbv_init(shell_handle_t shell);
int qbv_write_permanent(shell_handle_t shell, unsigned int port_id, struct genavb_st_config config);
int qbv_apply_permanent(shell_handle_t shell, unsigned int port_id);
int qbv_apply(shell_handle_t shell, unsigned int port_id, struct genavb_st_config *config);

#endif /* __QBV_H__ */
