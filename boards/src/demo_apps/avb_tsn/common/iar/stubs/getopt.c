/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stddef.h>

#include "getopt.h"

char *optarg = NULL;
int optind = 1, opterr = 1, optopt = 0;

int getopt(int argc, char * const argv[], const char *optstring)
{
    return -1;
}
