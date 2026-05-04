/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FreeRTOS.h"
#include "task.h"

#include "fsl_debug_console.h"

#include "board.h"
#include "log.h"
#include "uart.h"

#if (defined(UART_ADAPTER_NON_BLOCKING_MODE) && (UART_ADAPTER_NON_BLOCKING_MODE > 0U))
#define uart_task_PRIORITY   (4)

static TaskHandle_t uart_task_h;

static void uart_task(void *pvParameters)
{
    EnableIRQ(BOARD_UART_IRQ);

    while (1) {
        xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
        BOARD_UART_ISR();
        EnableIRQ(BOARD_UART_IRQ);
    }
}

void BOARD_UART_IRQ_HANDLER(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    DisableIRQ(BOARD_UART_IRQ);

    vTaskNotifyGiveFromISR(uart_task_h, &xHigherPriorityTaskWoken);

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

static void uart_irq_init(void)
{
    BaseType_t xResult;

    xResult = xTaskCreate(&uart_task, "uart_task",
            configMINIMAL_STACK_SIZE + 200, NULL,
            uart_task_PRIORITY, &uart_task_h);

    if (xResult != pdPASS)
        log_err("uart_task() failed\n");
}
#else
static void uart_irq_init(void) {}
#endif

void uart_init(void)
{
    uint32_t uartClkSrcFreq = BOARD_DebugConsoleSrcFreq();
    DbgConsole_Init(BOARD_DEBUG_UART_INSTANCE, BOARD_DEBUG_UART_BAUDRATE, BOARD_DEBUG_UART_TYPE, uartClkSrcFreq);
    NVIC_SetPriority(BOARD_UART_IRQ, configLIBRARY_LOWEST_INTERRUPT_PRIORITY);

    uart_irq_init();
}

