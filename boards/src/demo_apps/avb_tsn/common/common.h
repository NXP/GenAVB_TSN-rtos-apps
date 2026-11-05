/*
 * Copyright 2023-2024, 2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _COMMON_H_
#define _COMMON_H_

#include "stdint.h"

#define __init  __attribute__((section (".text.init")))
#define __exit  __attribute__((section (".text.exit")))

#endif /* _COMMON_H_ */
