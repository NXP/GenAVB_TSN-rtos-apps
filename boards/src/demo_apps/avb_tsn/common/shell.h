/*
 * Copyright 2019-2020, 2023, 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _SHELL_H_
#define _SHELL_H_

#include "fsl_shell.h"

#define shell_printf(handle, format...) do { if (handle) SHELL_Printf((handle), format); } while(0)

shell_handle_t shell_init(const char *task_id);
int shell_start(void (*init)(shell_handle_t shell));

#endif /* _SHELL_H_ */
