/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "errno.h"

#if (!defined(errno) && !defined(CONFIG_APP_LWIP))
int errno = 0;
#endif

int *__errno(void) {
    return &errno;
}
