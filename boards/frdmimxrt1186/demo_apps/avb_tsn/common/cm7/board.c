/*
 * Copyright 2023-2025 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_common.h"
#include "fsl_debug_console.h"
#include "board.h"
#if defined(SDK_I2C_BASED_COMPONENT_USED) && SDK_I2C_BASED_COMPONENT_USED
#include "fsl_lpi2c.h"
#endif /* SDK_I2C_BASED_COMPONENT_USED */
#if defined(SDK_NETC_USED) && SDK_NETC_USED
#include "fsl_netc_soc.h"
#include "fsl_netc_ierb.h"
#endif /* SDK_NETC_USED */
#include "fsl_iomuxc.h"
#include "fsl_cache.h"
#include "fsl_ele_base_api.h"
#include "fsl_dcdc.h"
#include "fsl_rgpio.h"
#include "FreeRTOSConfig.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define BOARD_FLEXSPI_DLL_LOCK_RETRY (10)

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
        kRGPIO_DigitalInput,
        0,
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