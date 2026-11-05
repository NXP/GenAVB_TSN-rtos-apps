/*
 * Copyright 2019-2020, 2022-2023, 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "avb_tsn/common/system_config.h"
#include "clock_domain.h"

struct avb_app_config system_avb_app_cfg = {
    .mclock_role = MEDIA_CLOCK_SLAVE,
};
