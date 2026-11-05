/*
 * Copyright 2019-2021, 2024-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _PIN_MUX_H_
#define _PIN_MUX_H_

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/

/*! @brief Direction type  */
typedef enum _pin_mux_direction
{
  kPIN_MUX_DirectionInput = 0U,         /* Input direction */
  kPIN_MUX_DirectionOutput = 1U,        /* Output direction */
  kPIN_MUX_DirectionInputOrOutput = 2U  /* Input or output direction */
} pin_mux_direction_t;

/*!
 * @addtogroup pin_mux
 * @{
 */

/***********************************************************************************************************************
 * API
 **********************************************************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Calls initialization functions.
 *
 */
void BOARD_InitBootPins(void);

/*!
 * @brief Calls initialization functions for a pps signal output.
 *
 */
void BOARD_InitPinsPPS(void);

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitUARTPins(void);

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitSEMCPins(void);

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitENETPins(void);

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitPWMPins(void);

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitENCPins(void);

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitSAIPins(void);

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitI2CPins(void);

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitFlexSPIPins(void);

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitLPUARTPins(void);

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitADCPins(void);

/* GPIO_AD_11 (coord P16), CUR_A */
/* Routed pin properties */
#define BOARD_INITADC_CUR_A_ADC1_PERIPHERAL                               LPADC1   /*!< Peripheral name */
#define BOARD_INITADC_CUR_A_ADC1_SIGNAL                                        B   /*!< Signal name */
#define BOARD_INITADC_CUR_A_ADC1_CHANNEL                                      1U   /*!< Signal channel */

/* GPIO_AD_12 (coord P17), CUR_B */
/* Routed pin properties */
#define BOARD_INITADC_CUR_B_ADC1_PERIPHERAL                               LPADC1   /*!< Peripheral name */
#define BOARD_INITADC_CUR_B_ADC1_SIGNAL                                        A   /*!< Signal name */
#define BOARD_INITADC_CUR_B_ADC1_CHANNEL                                      1U   /*!< Signal channel */

/* GPIO_AD_13 (coord L12), CUR_C_ADC1 */
/* Routed pin properties */
#define BOARD_INITADC_CUR_C_ADC1_PERIPHERAL                               LPADC1   /*!< Peripheral name */
#define BOARD_INITADC_CUR_C_ADC1_SIGNAL                                        B   /*!< Signal name */
#define BOARD_INITADC_CUR_C_ADC1_CHANNEL                                      1U   /*!< Signal channel */

/* GPIO_AD_12 (coord P17), CUR_B */
/* Routed pin properties */
#define BOARD_INITADC_CUR_B_ADC2_PERIPHERAL                               LPADC2   /*!< Peripheral name */
#define BOARD_INITADC_CUR_B_ADC2_SIGNAL                                        A   /*!< Signal name */
#define BOARD_INITADC_CUR_B_ADC2_CHANNEL                                      2U   /*!< Signal channel */

/* GPIO_AD_13 (coord L12), CUR_C_ADC1 */
/* Routed pin properties */
#define BOARD_INITADC_CUR_C_ADC2_PERIPHERAL                               LPADC2   /*!< Peripheral name */
#define BOARD_INITADC_CUR_C_ADC2_SIGNAL                                        B   /*!< Signal name */
#define BOARD_INITADC_CUR_C_ADC2_CHANNEL                                      2U   /*!< Signal channel */

/* GPIO_AD_08 (coord R15), VOLT_DCB */
/* Routed pin properties */
#define BOARD_INITADC_VOLT_DCB_PERIPHERAL                                 LPADC1   /*!< Peripheral name */
#define BOARD_INITADC_VOLT_DCB_SIGNAL                                          A   /*!< Signal name */
#define BOARD_INITADC_VOLT_DCB_CHANNEL                                        1U   /*!< Signal channel */

#if defined(__cplusplus)
}
#endif

/*!
 * @}
 */
#endif /* _PIN_MUX_H_ */

/***********************************************************************************************************************
 * EOF
 **********************************************************************************************************************/
