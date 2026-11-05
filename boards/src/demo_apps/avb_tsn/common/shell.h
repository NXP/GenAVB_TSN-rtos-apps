/*
 * Copyright 2019-2020, 2023, 2025-2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _SHELL_H_
#define _SHELL_H_

#include "fsl_shell.h"
#include "rtos_apps/shell/shell.h"

#define SHELL_WRAPPER(cmd_name) \
    static shell_status_t _##cmd_name(shell_handle_t shell, int32_t argc, char **argv) \
    { \
        int ret; \
        ret = cmd_name(shell, argc, argv); \
        if (ret == SHELL_ERR_HELP) { \
            return kStatus_SHELL_RetUsage; \
        } else if (ret < 0) { \
            return kStatus_SHELL_Error; \
        } \
        return kStatus_SHELL_Success; \
    }

shell_handle_t shell_init(const char *task_id);
int shell_start(void (*init)(shell_handle_t shell));

#endif /* _SHELL_H_ */
