/*
 * Copyright 2021-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _BOARD_H_
#define _BOARD_H_

#include "clock_config.h"
#include "fsl_common.h"
#include "fsl_rgpio.h"
#include "fsl_clock.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @brief The board name */
#define BOARD_NAME "FRDM-IMXRT1186"

#define BOARD_SHELL_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE + 1700)

/* The UART to use for debug messages. */
#define BOARD_DEBUG_UART_CLK_FREQ BOARD_DebugConsoleSrcFreq()
#define BOARD_DEBUG_UART_TYPE     kSerialPort_Uart
#ifndef BOARD_DEBUG_UART_CLK_ROOT
#define BOARD_DEBUG_UART_CLK_ROOT kCLOCK_Root_Lpuart1112
#endif
#ifndef BOARD_DEBUG_UART_BASEADDR
#define BOARD_DEBUG_UART_BASEADDR LPUART3
#endif
#ifndef BOARD_DEBUG_UART_INSTANCE
#define BOARD_DEBUG_UART_INSTANCE 3U
#endif
#ifndef BOARD_UART_IRQ
#define BOARD_UART_IRQ LPUART3_IRQn
#endif
#ifndef BOARD_UART_IRQ_HANDLER
#define BOARD_UART_IRQ_HANDLER LPUART3_IRQHandler
#endif
#ifndef BOARD_DEBUG_UART_BAUDRATE
#define BOARD_DEBUG_UART_BAUDRATE (115200U)
#endif

/* Definitions for eRPC MU transport layer */
#if defined(FSL_FEATURE_MU_SIDE_A)
#define MU_BASE        MU1_MUA
#define MU_IRQ         MU1_IRQn
#define MU_IRQ_HANDLER MU1_IRQHandler
#endif
#if defined(FSL_FEATURE_MU_SIDE_B)
#define MU_BASE        MU1_MUB
#define MU_IRQ         MU1_IRQn
#define MU_IRQ_HANDLER MU1_IRQHandler
#endif
#define MU_IRQ_PRIORITY (2)

/*! @brief The USER_LED used for board */
#define LOGIC_LED_ON  (1U)
#define LOGIC_LED_OFF (0U)
#ifndef BOARD_USER_LED_GPIO
#define BOARD_USER_LED_GPIO RGPIO2
#endif
#ifndef BOARD_USER_LED_GPIO_PIN
#define BOARD_USER_LED_GPIO_PIN (11U)
#endif

#define USER_LED_INIT(output)                                             \
    RGPIO_PinWrite(BOARD_USER_LED_GPIO, BOARD_USER_LED_GPIO_PIN, output); \
    BOARD_USER_LED_GPIO->PDDR |= (1U << BOARD_USER_LED_GPIO_PIN)                        /*!< Enable target USER_LED */
#define USER_LED_OFF() \
    RGPIO_PortClear(BOARD_USER_LED_GPIO, 1U << BOARD_USER_LED_GPIO_PIN)                 /*!< Turn off target USER_LED */
#define USER_LED_ON() RGPIO_PortSet(BOARD_USER_LED_GPIO, 1U << BOARD_USER_LED_GPIO_PIN) /*!<Turn on target USER_LED*/
#define USER_LED_TOGGLE() \
    RGPIO_PortToggle(BOARD_USER_LED_GPIO, 1u << BOARD_USER_LED_GPIO_PIN)                /*!< Toggle target USER_LED */

/*! @brief Define the port interrupt number for the board switches */
#ifndef BOARD_USER_BUTTON_GPIO
#define BOARD_USER_BUTTON_GPIO RGPIO4
#endif
#ifndef BOARD_USER_BUTTON_GPIO_PIN
#define BOARD_USER_BUTTON_GPIO_PIN (12U)
#endif
#define BOARD_USER_BUTTON_IRQ         GPIO4_IRQn
#define BOARD_USER_BUTTON_IRQ_HANDLER GPIO4_0_IRQHandler
#define BOARD_USER_BUTTON_INT_OUTPUT kRGPIO_InterruptOutput0
#define BOARD_USER_BUTTON_NAME        "SW4"

#define BOARD_WAKEUP_BUTTON_NAME      "SW6"

/*! @brief The board flash size */
#define BOARD_FLASH_SIZE (0x1000000U)

#define BOARD_IDLE_COUNT_PER_S 29509714.2959808

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
 * API
 ******************************************************************************/
uint32_t BOARD_DebugConsoleSrcFreq(void);
void BOARD_InitDebugConsole(void);
void BOARD_ConfigMPU(void);
void BOARD_InitNVIC(void);
void BOARD_InitGPIO(void);
uint32_t BOARD_GPIO_Active(void);
void BOARD_GPIO_Clear(void);
void BOARD_UART_ISR(void);
#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _BOARD_H_ */
