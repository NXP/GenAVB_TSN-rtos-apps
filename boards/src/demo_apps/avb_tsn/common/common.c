/*
 * Copyright 2023, 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>

#include "genavb/helpers.h"

#include "common.h"
#include "log.h"

int str2mac(const char *str, uint8_t *mac)
{
    unsigned int tmp[12];

    if (sscanf(str, "%1x%1x:%1x%1x:%1x%1x:%1x%1x:%1x%1x:%1x%1x", &tmp[0], &tmp[1], &tmp[2], &tmp[3], &tmp[4], &tmp[5], &tmp[6], &tmp[7], &tmp[8], &tmp[9], &tmp[10], &tmp[11]) != 12) {
        goto err;
    }

    mac[0] = (tmp[0] << 4) | (tmp[1]);
    mac[1] = (tmp[2] << 4) | (tmp[3]);
    mac[2] = (tmp[4] << 4) | (tmp[5]);
    mac[3] = (tmp[6] << 4) | (tmp[7]);
    mac[4] = (tmp[8] << 4) | (tmp[9]);
    mac[5] = (tmp[10] << 4) | (tmp[11]);

    return 0;

err:
    return -1;
}

int mac2str(uint8_t *mac, char *str, unsigned int len)
{
    int rc;

    rc = h_snprintf_strict(str, len, MAC_STR_FMT, MAC_STR(mac));

    if (rc < 0)
        goto err;

    return 0;

err:
    return -1;
}
