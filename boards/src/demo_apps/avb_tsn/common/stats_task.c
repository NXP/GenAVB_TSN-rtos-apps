/*
 * Copyright 2018-2020, 2022-2023, 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "board.h"
#include "clock_config.h"
#include "rtos_apps/async.h"
#include "rtos_apps/log.h"

#include "stats_task.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define STATS_TASK_NAME       "stats"
#define STATS_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE + 185)
#define STATS_TASK_PRIORITY   5

#define STATS_MAX_TASKS     18

#ifndef CONFIG_STATS_CPU_LOAD
#define CONFIG_STATS_CPU_LOAD       0
#endif
#ifndef CONFIG_STATS_HEAP
#define CONFIG_STATS_HEAP           1
#endif
#ifndef CONFIG_STATS_TOTAL_CPU_LOAD
#define CONFIG_STATS_TOTAL_CPU_LOAD 1
#endif
#ifndef CONFIG_STATS_LWIP
#define CONFIG_STATS_LWIP           0
#endif

struct TasksCPULoad_Ctx {
    TaskStatus_t __TaskStatusArray[2][STATS_MAX_TASKS];
    TaskStatus_t *TaskStatusArray;
    TaskStatus_t *LastTaskStatusArray;
    uint32_t LastTotalTime;
    BaseType_t LastNbTasks;
};

struct StatsTask_Ctx {
#if CONFIG_STATS_CPU_LOAD
    struct TasksCPULoad_Ctx TasksCPULoad;
#endif
    void (*PeriodicFn)(void *Data);
    void *PeriodicData;
    unsigned int PeriodMs;
};

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
extern unsigned int malloc_failed_count;

#if (__CORTEX_M == 7U)
extern uint32_t idleCounter;
#elif ((__CORTEX_M == 33U) || (__CORTEX_M == 4U))
extern uint32_t idle_time, busy_time;
#endif

static struct StatsTask_Ctx StatsTask;

/*******************************************************************************
 * Code
 ******************************************************************************/
#if CONFIG_STATS_HEAP
static void STATS_Heap(void)
{
    size_t FreeHeap, UsedHeap;

    FreeHeap = xPortGetFreeHeapSize();
    UsedHeap = configTOTAL_HEAP_SIZE - FreeHeap;

    log_raw_info("Heap used: %u, free: %u, min free: %u\n",
           UsedHeap, FreeHeap,
           xPortGetMinimumEverFreeHeapSize());

    log_raw_info("Malloc failed counter: %u\n\n", malloc_failed_count);
}
#endif

#if CONFIG_STATS_CPU_LOAD
static TaskStatus_t *STATS_FindTask(TaskStatus_t *TaskStatusArray,
                                    BaseType_t NbTasks, BaseType_t TaskNumber)
{
    int i;

    for (i = 0; i < NbTasks; i++) {
        if (TaskStatusArray[i].xTaskNumber == TaskNumber)
            return &TaskStatusArray[i];
    }
    return NULL;
}

static int STATS_TasksCPULoadInit(struct TasksCPULoad_Ctx *Ctx)
{
    memset(Ctx->__TaskStatusArray, 0, sizeof(TaskStatus_t) * STATS_MAX_TASKS * 2);

    Ctx->TaskStatusArray = Ctx->__TaskStatusArray[0];
    Ctx->LastTaskStatusArray = Ctx->__TaskStatusArray[1];

    return 0;
}

static void STATS_TasksCPULoad(struct TasksCPULoad_Ctx *Ctx)
{
    int i;
    TaskStatus_t *TaskStatusArray = Ctx->TaskStatusArray;
    TaskStatus_t *LastTaskStatusArray = Ctx->LastTaskStatusArray;
    BaseType_t NbTasks;
    uint32_t TotalTime, DeltaTotalTime, TotalTaskTime = 0;
    double PercentUsage;

    NbTasks = uxTaskGetSystemState(Ctx->TaskStatusArray,
                                   STATS_MAX_TASKS, &TotalTime);
    if (!NbTasks) {
        log_err("uxTaskGetSystemState error, check STATS_MAX_TASKS\n");
        return;
    }

    DeltaTotalTime = TotalTime - Ctx->LastTotalTime;

    log_raw_info("Name          Prio   %%CPU    MinStackFree\n");

    for (i = 0; i < NbTasks; i++) {
        TaskStatus_t *CurrentTaskStatus, *LastTaskStatus;

        CurrentTaskStatus = &TaskStatusArray[i];
        LastTaskStatus = STATS_FindTask(LastTaskStatusArray,
                                        Ctx->LastNbTasks,
                                        CurrentTaskStatus->xTaskNumber);
        if (LastTaskStatus) {
            uint32_t DeltaTaskTime;

            DeltaTaskTime = CurrentTaskStatus->ulRunTimeCounter - LastTaskStatus->ulRunTimeCounter;
            TotalTaskTime += DeltaTaskTime;
            PercentUsage = (double)(DeltaTaskTime * 100) / (double)DeltaTotalTime;

            log_raw_info("%-13s %d     %5.2f    %u\n",
                   CurrentTaskStatus->pcTaskName,
                   CurrentTaskStatus->uxCurrentPriority,
                   PercentUsage,
                   CurrentTaskStatus->usStackHighWaterMark);
        } else {
            log_raw_info("%-13s %d     ---\n",
                   CurrentTaskStatus->pcTaskName,
                   CurrentTaskStatus->uxCurrentPriority);
        }
    }

    if (TotalTaskTime > DeltaTotalTime)
        PercentUsage = 0;
    else
        PercentUsage = (double)((DeltaTotalTime - TotalTaskTime) * 100) / (double)DeltaTotalTime;

    log_raw_info("%-13s %d     %5.2f\n", "other", 0, PercentUsage);

    Ctx->LastTaskStatusArray = TaskStatusArray;
    Ctx->TaskStatusArray = LastTaskStatusArray;
    Ctx->LastTotalTime = TotalTime;
    Ctx->LastNbTasks = NbTasks;
}
#endif

#if CONFIG_STATS_TOTAL_CPU_LOAD
static void STATS_TotalCPULoad(void)
{
    uint32_t idle, busy;
    double load;
#if (__CORTEX_M == 7U)
    static uint32_t lastIdleCounter = 0, lastSysTick = 0;
    uint32_t idleCnt, sysTick;
    double period;

    __disable_irq();
    sysTick = xTaskGetTickCount();
    idleCnt = idleCounter;
    __enable_irq();

    period = (double)(sysTick - lastSysTick) / configTICK_RATE_HZ;
    idle = idleCnt - lastIdleCounter;
    busy = (uint32_t)(BOARD_IDLE_COUNT_PER_S * period - idle);

    lastSysTick = sysTick;
    lastIdleCounter = idleCnt;
#elif ((__CORTEX_M == 33U) || (__CORTEX_M == 4U))
    __disable_irq();

    idle = idle_time;
    busy = busy_time;
    idle_time = 0;
    busy_time = 0;

    __enable_irq();
#else
    busy = 0;
    idle = 0;
#endif

    if ((busy + idle) > 1)
        load = (double)busy / (double)(busy + idle) * 100.0;
    else
        load = 0.0;

    log_raw_info("Total CPU load : %5.2f\n\n", load);
}
#endif

static void STATS_TaskPeriodic(void *data)
{
    struct StatsTask_Ctx *Ctx = data;

    if (Ctx->PeriodicFn)
        Ctx->PeriodicFn(Ctx->PeriodicData);

#if CONFIG_STATS_CPU_LOAD
    STATS_TasksCPULoad(&Ctx->TasksCPULoad);
#endif
#if CONFIG_STATS_HEAP
    STATS_Heap();
#endif
#if CONFIG_STATS_TOTAL_CPU_LOAD
    STATS_TotalCPULoad();
#endif
#if CONFIG_STATS_LWIP
    stats_display();
#endif
}

int STATS_TaskInit(void (*PeriodicFn)(void *Data), void *Data, unsigned int PeriodMs,
                   struct rtos_apps_async **async)
{
    struct StatsTask_Ctx *Ctx = &StatsTask;
    struct rtos_apps_async *_async;
    struct rtos_apps_async_config config = {
        .name = STATS_TASK_NAME,
        .stack_size = STATS_TASK_STACK_SIZE,
        .priority = STATS_TASK_PRIORITY,
        .func = &STATS_TaskPeriodic,
        .data = Ctx,
        .period_ms = PeriodMs,
    };

    Ctx->PeriodicFn = PeriodicFn;
    Ctx->PeriodicData = Data;
    Ctx->PeriodMs = PeriodMs;

#if CONFIG_STATS_CPU_LOAD
    if (STATS_TasksCPULoadInit(&Ctx->TasksCPULoad) < 0)
        log_err("STATS_TasksCPULoadInit failed\n");
#endif

    _async = rtos_apps_async_init(&config);
    if (!_async)
        goto exit;

    if (async)
        *async = _async;

    return 0;

exit:
    return -1;
}
