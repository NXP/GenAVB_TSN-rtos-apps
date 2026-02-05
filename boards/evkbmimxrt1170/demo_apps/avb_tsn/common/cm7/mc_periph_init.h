/*
 * Copyright 2016 Freescale Semiconductor, Inc.
 * Copyright 2016-2021, 2024-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _MC_PERIPH_INIT_H_
#define _MC_PERIPH_INIT_H_

#include "rtos_apps/tsn/tsn_tasks_config.h"

#include "mcdrv_adcetc_imxrt11xx_evkb.h"
#include "mcdrv_pwm3ph_pwma.h"
#include "mcdrv_enc_qd.h"
#include "m1_pmsm_appconfig.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Version info */
#define MCRSP_VER       "2.0.0"        /* motor control package version */

/* Structure used during clocks and modulo calculations */
typedef struct _clock_setup
{
    uint32_t ui32FastPeripheralClock;
    uint32_t ui32CpuFrequency;
    uint32_t ui32BusClock;
    uint32_t ui32SysPllClock;
    uint16_t ui16M1SpeedLoopFreq;
    uint16_t ui16M1SpeedLoopModulo;
    uint16_t ui16M1PwmFreq;
    uint16_t ui16M1PwmModulo;
    uint16_t ui16M1PwmDeadTime;    
} clock_setup_t;

/******************************************************************************
 * Clock & PWM definition for motor 1
 ******************************************************************************/
#define M1_PWM_FREQ (10000)         /* PWM frequency - 10kHz */
#define M1_FOC_FREQ_VS_PWM_FREQ (1) /* FOC calculation is called every n-th PWM reload */
#define M1_SPEED_LOOP_FREQ (APP_PERIOD_DEFAULT / 1000) /* Speed loop frequency */
#define M1_PWM_DEADTIME (2270)      /* Output PWM deadtime value in nanoseconds */

#define M1_FAST_LOOP_TS ((float_t)1.0/(float_t)(M1_PWM_FREQ / M1_FOC_FREQ_VS_PWM_FREQ))
#define M1_SLOW_LOOP_TS ((float_t)1.0/(float_t)(M1_SLOW_LOOP_FREQ))
#define M1_TIME_ONESEC_COUNT (M1_PWM_FREQ / M1_FOC_FREQ_VS_PWM_FREQ)

/* Assignment of eFlexPWM channels to motor 1 phases
 * 0 - PWM channels A0&B0 - sub-module 0
 * 1 - PWM channels A1&B1 - sub-module 1
 * 2 - WPM channels A2&B2 - sub-module 3
 */
#define M1_PWM_PAIR_PHA (0)
#define M1_PWM_PAIR_PHB (1)
#define M1_PWM_PAIR_PHC (2)

/* Over Current Fault detection */
#define M1_FAULT_NUM (0)

/* Braking resistor macros */
#define M1_BRAKE_SET() 
#define M1_BRAKE_CLEAR()  

/* DC bus braking threshold hysteresis */
#define M1_U_DCB_HYSTERESIS (0.05F)

/******************************************************************************
 * ADC measurement definition for motor 1
 ******************************************************************************/
/* Configuration table of ADC channels according to the input pin signals:
 * Valid for iMXRT1170 together with FRDM-MC-LVPMSM
 *
 * Motor 1
 * Quantity     | Module A (ADC1)       | Module B (ADC2)
 * --------------------------------------------------------------------------
 * M1_U_DCB -   GPIO_AD_08 - ADC1 CH1-A
 * M1_I_A -     GPIO_AD_11 - ADC1 CH2-B
 * M1_I_B -     GPIO_AD_12 - ADC1 CH1-A; ADC2 CH3-A
 * M1_I_C -     GPIO_AD_13 - ADC1 CH3-B; ADC2 CH3-B
 */

/* Phase current A assigned to ADC1 and ADC2 */
#define M1_ADC1_PH_A_CHNL (2U)
#define M1_ADC1_PH_A_SIDE (kLPADC_SampleChannelSingleEndSideB)

#define M1_ADC2_PH_A_CHNL (N/A) //not available
#define M1_ADC2_PH_A_SIDE (N/A) //not available

/* Phase current B assigned to ADC1 and ADC2 */
#define M1_ADC1_PH_B_CHNL (3U)
#define M1_ADC1_PH_B_SIDE (kLPADC_SampleChannelSingleEndSideA)

#define M1_ADC2_PH_B_CHNL (3U)
#define M1_ADC2_PH_B_SIDE (kLPADC_SampleChannelSingleEndSideA)

/* Phase current C assigned to ADC1 and ADC2 */
#define M1_ADC1_PH_C_CHNL (3U)
#define M1_ADC1_PH_C_SIDE (kLPADC_SampleChannelSingleEndSideB)

#define M1_ADC2_PH_C_CHNL (3U)
#define M1_ADC2_PH_C_SIDE (kLPADC_SampleChannelSingleEndSideB)

/* Phase voltage UDCB assigned to ADC1 and ADC2 */
#define M1_ADC1_UDCB_CHNL (1U)
#define M1_ADC1_UDCB_SIDE (kLPADC_SampleChannelSingleEndSideA)

#define M1_ADC2_UDCB_CHNL (N/A) //not available
#define M1_ADC2_UDCB_SIDE (N/A) //not available

/* offset measurement filter window */
#define ADC_OFFSET_WINDOW (3)
/******************************************************************************
 * MC driver macro definition and check - do not change this part
 ******************************************************************************/
/******************************************************************************
 * Define common ADC control functions for motor 1
 ******************************************************************************/
#define M1_MCDRV_ADC_PERIPH_INIT() (InitADC())
#define M1_MCDRV_ADC_GET(par) \
    MCDRV_Curr3Ph2ShGet(par); \
    MCDRV_VoltDcBusGet(par); \
    MCDRV_AuxValGet(par);
#define M1_MCDRV_CURR_3PH_CHAN_ASSIGN(par) (MCDRV_Curr3Ph2ShChanAssign(par))
#define M1_MCDRV_CURR_3PH_CALIB_INIT(par) (MCDRV_Curr3Ph2ShCalibInit(par))
#define M1_MCDRV_CURR_3PH_CALIB(par) (MCDRV_Curr3Ph2ShCalib(par))
#define M1_MCDRV_CURR_3PH_CALIB_SET(par) (MCDRV_Curr3Ph2ShCalibSet(par))
    
/******************************************************************************
 * Define 3-ph PWM control functions for motor 1
 ******************************************************************************/
#define M1_MCDRV_PWM_PERIPH_INIT() (M1_InitPWM())
#define M1_MCDRV_PWM3PH_SET(par) (MCDRV_eFlexPwm3PhSet(par))
#define M1_MCDRV_PWM3PH_EN(par) (MCDRV_eFlexPwm3PhOutEn(par))
#define M1_MCDRV_PWM3PH_DIS(par) (MCDRV_eFlexPwm3PhOutDis(par))
#define M1_MCDRV_PWM3PH_FLT_GET(par) (MCDRV_eFlexPwm3PhFltGet(par))
#define M1_MCDRV_QD_EN_INDEX_INT(par) (MCDRV_QdEncEnIndexInterrupt(par))
#define M1_MCDRV_QD_DIS_INDEX_INT(par) (MCDRV_QdEncDisIndexInterrupt(par))
#define M1_MCDRV_QD_GET_INDEX_INT_STATUS(par) (MCDRV_QdEncGetIndexInterruptStatus(par))
#define M1_MCDRV_QD_CLEAR_INDEX_INT_STATUS(par) (MCDRV_QdEncClearIndexInterruptStatus(par))

/******************************************************************************
 * Define position and speed sensor - quadrature encoder for motor 1
 ******************************************************************************/
#define M1_MCDRV_QD_PERIPH_INIT() M1_InitQD()
#define M1_MCDRV_QD_GET(par) (MCDRV_QdEncGet(par))
#define M1_MCDRV_QD_SET_DIRECTION(par) (MCDRV_QdEncSetDirection(par))
#define M1_MCDRV_QD_CLEAR(par) (MCDRV_QdEncClear(par))
#define M1_MCDRV_QD_SET_PULSES(par)

/******************************************************************************
 * Global variable definitions
 ******************************************************************************/
extern mcdrv_adcetc_t g_sM1AdcSensor;
extern mcdrv_pwm3ph_pwma_t g_sM1Pwm3ph;
extern mcdrv_qd_enc_t g_sM1Enc;

extern clock_setup_t g_sClockSetup;

/*******************************************************************************
 * API
 ******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

void MCDRV_Init_M1(void);
void InitClock(void);
void InitADC(void);
void M1_InitPWM(void);
void InitXBARA(void);
void InitADC_ETC(void);
void M1_InitQD(void);

#ifdef __cplusplus
}
#endif
#endif /* _MC_PERIPH_INIT_H_  */
