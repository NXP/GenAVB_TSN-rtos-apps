/*
 * Copyright 2018-2020, 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _TASK_MANAGEMENT_H_
#define _TASK_MANAGEMENT_H_

#include "task_config.h"

static inline struct task *get_task(struct task_slot *slot)
{
    return (slot) ? slot->task : NULL;
}

/**
 *
 */
int task_init(void);

/**
 *
 */
int task_exit(void);

/**
 *
 */
int task_slot_add(int capabilities, void *data, int (*handler)(void *data), struct task_slot **slot_ptr);

/**
 *
 */
int task_slot_join(struct task *task, void *data, int (*handler)(void *data), struct task_slot **slot_ptr);

/**
 *
 */
int task_slot_free(struct task_slot *slot);

/**
 *
 */
int task_slot_notify(struct task_slot *slot);

#endif /* _TASK_MANAGEMENT_H_ */
