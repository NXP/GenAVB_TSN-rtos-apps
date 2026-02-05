/*
 * Copyright 2020, 2023-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _APP_SHELL_H_
#define _APP_SHELL_H_

#include "avb_tsn/common/shell.h"

void default_qos_init(shell_handle_t shell);
shell_handle_t tsn_init_shell(const char *task_id);

#endif /* _APP_SHELL_H_ */
