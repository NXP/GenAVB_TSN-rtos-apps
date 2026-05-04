/*
 * Copyright 2023-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __FP_H__
#define __FP_H__

#include "genavb/frame_preemption.h"
#include "shell.h"

void fp_init(shell_handle_t shell);
int fp_write_802_1q_permanent(shell_handle_t shell, unsigned int port_id, struct genavb_fp_config config);
int fp_write_802_3_permanent(shell_handle_t shell, unsigned int port_id, struct genavb_fp_config config);
int fp_apply_permanent(shell_handle_t shell, unsigned int port_id);
int fp_apply(shell_handle_t shell, unsigned int port_id, struct genavb_fp_config *config_fp_8021q, struct genavb_fp_config *config_fp_8023);

#endif /* __FP_H__ */
