/*
 * Copyright 2018-2019, 2024-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>

#include "genavb/genavb.h"

#include "task_management.h"
#include "task_config.h"
#include "avb_tsn/common/log.h"
#include "avb_tsn/common/common.h"

#define MUTEX_TICKS_TO_WAIT portMAX_DELAY
static SemaphoreHandle_t task_mutex;

static void tasks_handle(void *data)
{
    struct task *task = (struct task *)data;

    while (1) {
        struct task_slot *slot = NULL;

        if (xQueueReceive(task->queue.handle, &slot, pdMS_TO_TICKS(10000)) != pdTRUE)
            continue;

        slot->handler(slot->data);
    }
}

static int __task_init(struct task *task, unsigned int index)
{
    int slot_index;
    char task_name[16];

    sprintf(task_name, "avb_app task%1d", index);

    if ((task->queue.handle = xQueueCreate(task->queue.length, sizeof(struct task_slot *))) == NULL) {
        log_err("xQueueCreate failed\n\r");
        goto err_queue_create;
    }

    if (xTaskCreate(&tasks_handle, task_name, task->stack_depth, (void *)task, task->priority, &task->handle) != pdPASS) {
        log_err("xTaskCreate failed\n\r");
        goto err_task_create;
    }

    for (slot_index = 0; slot_index < MAX_TASK_SLOTS; slot_index++) {
        struct task_slot *slot = &task->slots[slot_index];
        slot->task = task;
    }

    return 0;

err_task_create:
    vQueueDelete(task->queue.handle);
err_queue_create:
    return -1;
}

static void __task_exit(struct task *task)
{
    for (int slot_index = 0; slot_index < MAX_TASK_SLOTS; slot_index++) {
        struct task_slot *slot = &task->slots[slot_index];
        slot->is_used = false;
        slot->handler = NULL;
        slot->data = NULL;
        slot->task = NULL;
    }

    task->used_slots = 0;

    vQueueDelete(task->queue.handle);

    vTaskDelete(task->handle);
}

static struct task_slot *task_get_first_idle_slot(struct task *task)
{
    int i;

    for (i = 0; i < MAX_TASK_SLOTS; i++) {
        if (!task->slots[i].is_used)
            return &task->slots[i];
    }

    return NULL;
}

static int task_check_capability_match(struct task *task, int capabilities)
{
    int tmp = task->task_capabilities;

    // if not match capacity, return 0
    if ((tmp & capabilities) != capabilities) {
        return 0;
    }

    return 1;
}

static int __task_slot_join(struct task *task, void *data, int (*handler)(void *data), struct task_slot **slot_ptr)
{
    struct task_slot *free_slot = NULL;

    if ((free_slot = task_get_first_idle_slot(task)) == NULL)
        return -1;

    free_slot->handler = handler;
    free_slot->data = data;
    free_slot->is_used = true;

    task->used_slots++;

    *slot_ptr = free_slot;

    return 0;
}

int task_slot_notify(struct task_slot *slot)
{
    if (!slot) {
        log_err("Empty slot\n\r");
        goto err;
    }

    if (!slot->is_used) {
        log_err("Slot not used\n\r");
        goto err;
    }

    if (xQueueSendFromISR(slot->task->queue.handle, &slot, NULL) != pdTRUE) {
        log_err("xQueueSendFromISR() failed\n\r");
        goto err;
    }

    return 0;

err:
    return -1;
}

__init int task_init(void)
{
    int task_index;

    for (task_index = 0; task_index < MAX_TASKS; task_index++) {
        struct task *task = &tasks[task_index];

        if (__task_init(task, task_index))
            goto err_task_init;
    }

    if ((task_mutex = xSemaphoreCreateMutex()) == NULL) {
        log_err("xSemaphoreCreateMutex() failed\n");
        goto err_mutex;
    }

    return 0;

err_mutex:
err_task_init:
    while (task_index--) {
        struct task *task = &tasks[task_index];
        __task_exit(task);
    }

    return -1;
}

__exit int task_exit(void)
{
    int task_index;

    for (task_index = 0; task_index < MAX_TASKS; task_index++) {
        struct task *task = &tasks[task_index];
        __task_exit(task);
    }

    vSemaphoreDelete(task_mutex);

    return 0;
}

int task_slot_join(struct task *task, void *data, int (*handler)(void *data), struct task_slot **slot_ptr)
{
    if (!task)
        goto err;

    if (handler == NULL)
        goto err;

    if (slot_ptr == NULL)
        goto err;

    xSemaphoreTake(task_mutex, MUTEX_TICKS_TO_WAIT);

    if (__task_slot_join(task, data, handler, slot_ptr))
        goto err_no_free_slot;

    xSemaphoreGive(task_mutex);

    return 0;

err_no_free_slot:
    xSemaphoreGive(task_mutex);
err:
    return -1;
}

int task_slot_add(int capabilities, void *data, int (*handler)(void *data), struct task_slot **slot)
{
    struct task *task_ptr = NULL;
    int i;

    xSemaphoreTake(task_mutex, MUTEX_TICKS_TO_WAIT);

    for (i = 0; i < MAX_TASKS; i++) {
        struct task *cur_task = &tasks[i];

        if (!task_check_capability_match(cur_task, capabilities))
            continue;

        if (cur_task->used_slots < cur_task->max_slots) {
            task_ptr = cur_task;
            log_debug("task index = %d\n", i);
            break;
        }
    }

    if (!task_ptr) {
        log_err("No taks found\n");
        xSemaphoreGive(task_mutex);
        return -1;
    }

    if (__task_slot_join(task_ptr, data, handler, slot)) {
        log_err("No task slot found\n");
        xSemaphoreGive(task_mutex);
        return -1;
    }

    xSemaphoreGive(task_mutex);

    return 0;
}

int task_slot_free(struct task_slot *slot)
{
    xSemaphoreTake(task_mutex, MUTEX_TICKS_TO_WAIT);

    slot->handler = NULL;
    slot->data = NULL;
    slot->is_used = false;
    slot->task->used_slots--;

    xSemaphoreGive(task_mutex);

    return 0;
}
