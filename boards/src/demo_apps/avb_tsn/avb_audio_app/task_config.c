/*
 * Copyright 2018-2019 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "task_config.h"

struct task tasks[MAX_TASKS] = {
    [0] = {
        .handle = NULL,
        .stack_depth = TASK_DEFAULT_STACK_SIZE,
        .priority = TASK_DEFAULT_PRIORITY,
        .queue = {
            .handle = NULL,
            .length = TASK_DEFAULT_QUEUE_LENGTH,
        },
        .slots = {
            {0},
        },
        .max_slots = MAX_TASK_SLOTS,
        .used_slots = 0,
        .task_capabilities = TASK_CAP_STREAM_LISTENER | TASK_CAP_STREAM_TALKER,
    },
};
