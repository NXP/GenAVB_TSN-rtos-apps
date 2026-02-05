/*
 * Copyright 2018-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FreeRTOS.h"
#include "task.h"
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#if ((__CORTEX_M == 33U) || (__CORTEX_M == 4U))
#include "board.h"
#endif
unsigned int malloc_failed_count = 0;

#ifndef BOARD_TIMER_MAX_VALUE
#define BOARD_TIMER_MAX_VALUE 0xFFFFFFFF
#endif

#if (__CORTEX_M == 7U)
uint32_t idleCounter;
#elif ((__CORTEX_M == 33U) || (__CORTEX_M == 4U))
uint32_t idle_time = 0, busy_time = 0;
bool idle_hook_enabled = false;
#endif

void sync_dcache_icache(void)
{
#if __CORTEX_M == 7U
    SCB_CleanDCache();
    SCB_InvalidateICache();
#endif
}

void app_assert(const char *func_name, const char *file_name, unsigned int line_number)
{
    DbgConsole_Flush();
    DisableGlobalIRQ();
    DbgConsole_BlockingPrintf("\nassert: In function %s %s:%u\r\n", func_name, file_name, line_number);
    for (;;);
}

/*!
 * @brief Malloc failed hook.
 */
void vApplicationMallocFailedHook(void)
{
    /* The malloc failed hook is enabled by setting
    configUSE_MALLOC_FAILED_HOOK to 1 in FreeRTOSConfig.h.

    Called if a call to pvPortMalloc() fails because there is insufficient
    free memory available in the FreeRTOS heap.  pvPortMalloc() is called
    internally by FreeRTOS API functions that create tasks, queues, software
    timers, and semaphores.  The size of the FreeRTOS heap is set by the
    configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
    malloc_failed_count++;
}

/*!
 * @brief Stack overflow hook.
 */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    (void)pcTaskName;
    (void)xTask;

    /* Run time stack overflow checking is performed if
    configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
    function is called if a stack overflow is detected.  pxCurrentTCB can be
    inspected in the debugger if the task name passed into this function is
    corrupt. */
    DbgConsole_BlockingPrintf("\n%s: %s\n", __func__, pcTaskName);
    for (;;)
        ;
}

/* configUSE_STATIC_ALLOCATION is set to 1, so the application must provide an
implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
used by the Idle task. */
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                   StackType_t **ppxIdleTaskStackBuffer,
                                   uint32_t *pulIdleTaskStackSize)
{
    /* If the buffers to be provided to the Idle task are declared inside this
    function then they must be declared static - otherwise they will be allocated on
    the stack and so not exists after this function exits. */
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
    state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
/*-----------------------------------------------------------*/

/* configUSE_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
application must provide an implementation of vApplicationGetTimerTaskMemory()
to provide the memory that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                                    StackType_t **ppxTimerTaskStackBuffer,
                                    uint32_t *pulTimerTaskStackSize)
{
    /* If the buffers to be provided to the Timer task are declared inside this
    function then they must be declared static - otherwise they will be allocated on
    the stack and so not exists after this function exits. */
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];

    /* Pass out a pointer to the StaticTask_t structure in which the Timer
    task's state will be stored. */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /* Pass out the array that will be used as the Timer task's stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configTIMER_TASK_STACK_DEPTH is specified in words, not bytes. */
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

#ifdef BOARD_TIMER_GET_COUNT
static uint32_t timer_delta(uint32_t start, uint32_t end)
{
    if (end > start)
        return (end - start);
    else
        return (end + BOARD_TIMER_MAX_VALUE - start);
}
#endif

void vApplicationIdleHook(void)
{
#if (__CORTEX_M == 7U)
    idleCounter++;
#elif ((__CORTEX_M == 33U) || (__CORTEX_M == 4U))
    if (idle_hook_enabled) {
#ifdef BOARD_TIMER_GET_COUNT
        static uint32_t idle_start, idle_end = 0;

        __disable_irq();

        idle_start = BOARD_TIMER_GET_COUNT();
        busy_time += timer_delta(idle_end, idle_start);
#endif

        __DSB();
        __WFI();

#ifdef BOARD_TIMER_GET_COUNT
        idle_end = BOARD_TIMER_GET_COUNT();
        idle_time += timer_delta(idle_start, idle_end);

        __enable_irq();
#endif
    }
#endif
}
