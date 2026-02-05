/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef GETOPT_H
#define GETOPT_H

extern char *optarg;
extern int optind, opterr, optopt;

int getopt(int argc, char * const argv[], const char *optstring);

#endif /* GETOPT_H */

