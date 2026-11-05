/*
 * Copyright 2023-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "board.h"
#include "fsl_iomuxc.h"
#include "fsl_rgpio.h"
#include "FreeRTOSConfig.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief   void BOARD_InitGPIO(void)
 *           - Initialization of the GPIO peripherals
 *
 * @param   void
 *
 * @return  none
 */
void BOARD_InitGPIO(void)
{
    /* SW8 Button configuration */
    const rgpio_pin_config_t user_button_config = {
        kRGPIO_DigitalInput,  /* Set pin as digital input */
        0,                   /* Set default output logic, which has no use in input  */
    };

    /* Workaround: Disable interrupt which might be enabled by ROM. */
    RGPIO_SetPinInterruptConfig(RGPIO1, 9U, kRGPIO_InterruptOutput0, kRGPIO_InterruptOrDMADisabled);
    NVIC_ClearPendingIRQ(GPIO1_0_IRQn);

    /* Init input switch GPIO. */
    RGPIO_SetPinInterruptConfig(BOARD_USER_BUTTON_GPIO, BOARD_USER_BUTTON_GPIO_PIN, BOARD_USER_BUTTON_INT_OUTPUT, kRGPIO_InterruptFallingEdge);
    RGPIO_PinInit(BOARD_USER_BUTTON_GPIO, BOARD_USER_BUTTON_GPIO_PIN, &user_button_config);

    NVIC_SetPriority(BOARD_USER_BUTTON_IRQ, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 3);
    NVIC_ClearPendingIRQ(BOARD_USER_BUTTON_IRQ);

    EnableIRQ(BOARD_USER_BUTTON_IRQ);
}

/*!
 * @brief   uint32_t BOARD_GPIO_Active(void)
 *           - Enable the GPIO peripherals
 *
 * @param   void
 *
 * @return  uint32_t
 */
uint32_t BOARD_GPIO_Active(void)
{
    return RGPIO_GetPinsInterruptFlags(BOARD_USER_BUTTON_GPIO, BOARD_USER_BUTTON_INT_OUTPUT) & (1 << BOARD_USER_BUTTON_GPIO_PIN);
}

/*!
 * @brief   void BOARD_GPIO_Clear(void)
 *           - Clear the GPIO peripherals
 *
 * @param   void
 *
 * @return  none
 */
void BOARD_GPIO_Clear(void)
{
    RGPIO_ClearPinsInterruptFlags(BOARD_USER_BUTTON_GPIO, BOARD_USER_BUTTON_INT_OUTPUT, 1 << BOARD_USER_BUTTON_GPIO_PIN);
}
