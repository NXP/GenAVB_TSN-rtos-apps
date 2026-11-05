/*
 * Copyright 2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _SHELL_CONFIG_H_
#define _SHELL_CONFIG_H_

#include <inttypes.h>
#include <getopt.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

#include "fsl_debug_console.h"
#include "fsl_shell.h"

#include "rtos_apps/log.h"
#include "../genavb.h"

#define shell_printf(handle, format...) do { if (handle) SHELL_Printf((handle), format); } while(0)

#endif /* _SHELL_CONFIG_H_ */
