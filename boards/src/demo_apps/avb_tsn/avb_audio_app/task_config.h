/*
 * Copyright 2018-2020 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _TASK_CONFIG_H_
#define _TASK_CONFIG_H_

#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#define MAX_TASKS      1
#define MAX_TASK_SLOTS 16

#define TASK_DEFAULT_STACK_SIZE (400)
#define TASK_DEFAULT_PRIORITY   (configMAX_PRIORITIES - 8)

#define TASK_DEFAULT_QUEUE_LENGTH (8)

enum task_capability {
    TASK_CAP_STREAM_TALKER = 0x01,   /**< Task can process AVB talker stream */
    TASK_CAP_STREAM_LISTENER = 0x02, /**< Task can process AVB listener stream */
};

struct task_slot {
    bool is_used;
    int (*handler)(void *data);
    void *data;
    struct task *task;
};

struct task {
    TaskHandle_t handle;
    unsigned short stack_depth;
    UBaseType_t priority;
    struct {
        QueueHandle_t handle;
        UBaseType_t length;
    } queue;
    struct task_slot slots[MAX_TASK_SLOTS];
    unsigned int max_slots;
    unsigned int used_slots;
    int task_capabilities;
};

extern struct task tasks[MAX_TASKS];

#endif /* _TASK_CONFIG_H_ */
