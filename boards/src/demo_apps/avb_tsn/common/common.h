/*
 * Copyright 2023-2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _COMMON_H_
#define _COMMON_H_

#include "stdint.h"

#define __init  __attribute__((section (".text.init")))
#define __exit  __attribute__((section (".text.exit")))

int str2mac(const char *str, uint8_t *mac);
int mac2str(uint8_t *mac, char *str, unsigned int len);

#endif /* _COMMON_H_ */
