/*
 * Copyright 2016 Freescale Semiconductor, Inc.
 * Copyright 2016-2021, 2024-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "mc_periph_init.h"
#include "fsl_common.h"
#include "fsl_xbara.h"
#include "fsl_adc_etc.h"
#include "m1_sm_snsless_enc.h"
#include "mcdrv_enc_qd.h"
#include "fsl_lpadc.h"
#include "state_machine.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/

/* Motor 1 */
/* Structure for current and voltage measurement */
mcdrv_adcetc_t g_sM1AdcSensor;

/* Structure for 3-phase PWM MC driver */
mcdrv_pwm3ph_pwma_t g_sM1Pwm3ph;

/* Structure for Encoder driver */
mcdrv_qd_enc_t g_sM1Enc;

/* Clock setup structure */
clock_setup_t g_sClockSetup;

extern const clock_arm_pll_config_t armPllConfig;

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
* @brief   void MCDRV_Init_M1(void)
*           - Motor control driver main initialization
*           - Calls initialization functions of peripherals required for motor
*             control functionality
*
* @param   void
*
* @return  none
*/
void MCDRV_Init_M1(void)
{    
    /* Init application clock dependent variables */
    InitClock();
    
    /* Init ADC */
    M1_MCDRV_ADC_PERIPH_INIT();
        
    /* init XBAR  */
    InitXBARA();
    
    /* Init ADC_ETC */
    InitADC_ETC();

    /* 6-channel PWM peripheral init for M1 */
    M1_MCDRV_PWM_PERIPH_INIT();

    /* Qudrature decoder peripheral init */
    M1_MCDRV_QD_PERIPH_INIT();
}

/*!
* @brief      Core, bus, flash clock setup
*
* @param      void
*
* @return     none
*/
void InitClock(void)
{ 
    uint32_t ui32CyclesNumber = 0U;
    /* Calculate clock dependant variables for PMSM control algorithm */
    g_sClockSetup.ui32FastPeripheralClock = CLOCK_GetRootClockFreq(kCLOCK_Root_Bus);

    /* Parameters for motor M1 */
    g_sClockSetup.ui16M1PwmFreq = M1_PWM_FREQ; /* 10 kHz */
    g_sClockSetup.ui16M1PwmModulo = g_sClockSetup.ui32FastPeripheralClock / g_sClockSetup.ui16M1PwmFreq;
    ui32CyclesNumber = ((M1_PWM_DEADTIME * (g_sClockSetup.ui32FastPeripheralClock / 1000000U))/1000U); /* max 2047 cycles */
    g_sClockSetup.ui16M1PwmDeadTime = ui32CyclesNumber;
    g_sClockSetup.ui16M1SpeedLoopFreq = M1_SPEED_LOOP_FREQ;
}

/*!
* @brief   void InitADC(void)
*           - Initialization of the ADC peripheral
*          
* @param   void
*
* @return  none
*/
void InitADC(void)
{

    lpadc_config_t lpadcConfig;
    lpadc_conv_command_config_t lpadcCommandConfig;
    lpadc_conv_trigger_config_t lpadcTriggerConfig;

    /* Initialize the ADC module. */
    LPADC_GetDefaultConfig(&lpadcConfig);

    lpadcConfig.enableAnalogPreliminary = true;
    lpadcConfig.referenceVoltageSource = kLPADC_ReferenceVoltageAlt2;
    lpadcConfig.powerLevelMode = kLPADC_PowerLevelAlt4;
    lpadcConfig.powerUpDelay = 0U;

    LPADC_Init(LPADC1, &lpadcConfig);
    LPADC_Init(LPADC2, &lpadcConfig);

    /* Phase current measurement */
    /* Sector 1,6 - measured currents Ib & Ic */
    /* ADC1, channel Ib = M1_ADC1_PH_B (ADC_ETC trigger 0, chain 0) */
    g_sM1AdcSensor.sCurrSec16.ui16ChanNumPhaB = M1_ADC1_PH_B_CHNL;
    g_sM1AdcSensor.sCurrSec16.ui16ChanSidePhaB = M1_ADC1_PH_B_SIDE;
    /* ADC2, channel Ic = M1_ADC2_PH_C (ADC_ETC trigger 4, chain 0) */
    g_sM1AdcSensor.sCurrSec16.ui16ChanNumPhaC = M1_ADC2_PH_C_CHNL;
    g_sM1AdcSensor.sCurrSec16.ui16ChanSidePhaC = M1_ADC2_PH_C_SIDE;

    /* Sector 2,3 - measured currents Ia & Ic*/
    /* ADC1, channel Ia = M1_ADC1_PH_A (ADC_ETC trigger 0, chain 0) */
    g_sM1AdcSensor.sCurrSec23.ui16ChanNumPhaA = M1_ADC1_PH_A_CHNL;
    g_sM1AdcSensor.sCurrSec23.ui16ChanSidePhaA = M1_ADC1_PH_A_SIDE;
    /* ADC2, channel Ic = M1_ADC2_PH_C (ADC_ETC trigger 4, chain 0) */
    g_sM1AdcSensor.sCurrSec23.ui16ChanNumPhaC = M1_ADC2_PH_C_CHNL;
    g_sM1AdcSensor.sCurrSec23.ui16ChanSidePhaC = M1_ADC2_PH_C_SIDE;

    /* Sector 4,5 - measured currents Ia & Ib */
    /* ADC1, channel Ia = M1_ADC1_PH_A (ADC_ETC trigger 0, chain 0) */
    g_sM1AdcSensor.sCurrSec45.ui16ChanNumPhaA = M1_ADC1_PH_A_CHNL;
    g_sM1AdcSensor.sCurrSec45.ui16ChanSidePhaA = M1_ADC1_PH_A_SIDE;
    /* ADC2, channel Ib = M1_ADC2_PH_B (ADC_ETC trigger 4, chain 0) */
    g_sM1AdcSensor.sCurrSec45.ui16ChanNumPhaB = M1_ADC2_PH_B_CHNL;
    g_sM1AdcSensor.sCurrSec45.ui16ChanSidePhaB = M1_ADC2_PH_B_SIDE;

    /* UDCbus channel measurement */
    /* ADC1, channel Udcb = M1_ADC1_UDCB (ADC_ETC trigger 0, chain 1) */
    g_sM1AdcSensor.ui16ChanNumVDcb  = M1_ADC1_UDCB_CHNL;
    g_sM1AdcSensor.ui16ChanSideVDcb  = M1_ADC1_UDCB_SIDE;
    /* ADC2, channel AUX (ADC_ETC trigger 4, chain 1) */
    g_sM1AdcSensor.ui16ChanNumAux = 7U;
    g_sM1AdcSensor.ui16ChanSideAux = kLPADC_SampleChannelSingleEndSideB;

    //Default channel assigment for Sector 2

    /**************************************/
    /*             ADC1                   */
    /**************************************/

    /* Set conversion CMD configuration. */
    LPADC_GetDefaultConvCommandConfig(&lpadcCommandConfig);
    lpadcCommandConfig.channelNumber = g_sM1AdcSensor.sCurrSec23.ui16ChanNumPhaA;
    lpadcCommandConfig.sampleTimeMode = kLPADC_SampleTimeADCK3;
    lpadcCommandConfig.sampleScaleMode = kLPADC_SamplePartScale;
    lpadcCommandConfig.sampleChannelMode = g_sM1AdcSensor.sCurrSec23.ui16ChanSidePhaA;
    LPADC_SetConvCommandConfig(LPADC1, 1U, &lpadcCommandConfig); // CMDL[number]
    /* Set trigger configuration. */
    LPADC_GetDefaultConvTriggerConfig(&lpadcTriggerConfig);
    lpadcTriggerConfig.targetCommandId = 1U; //CMDL
    lpadcTriggerConfig.enableHardwareTrigger = true;
    LPADC_SetConvTriggerConfig(LPADC1, 0U, &lpadcTriggerConfig); // trigger from TCTRL[number]

    /* Set conversion CMD configuration. */
    LPADC_GetDefaultConvCommandConfig(&lpadcCommandConfig);
    lpadcCommandConfig.channelNumber = g_sM1AdcSensor.ui16ChanNumVDcb;
    lpadcCommandConfig.sampleTimeMode = kLPADC_SampleTimeADCK3;
    lpadcCommandConfig.sampleScaleMode = kLPADC_SamplePartScale;
    lpadcCommandConfig.sampleChannelMode = g_sM1AdcSensor.ui16ChanSideVDcb;
    LPADC_SetConvCommandConfig(LPADC1, 2U, &lpadcCommandConfig); // CMDL[number]
    /* Set trigger configuration. */
    LPADC_GetDefaultConvTriggerConfig(&lpadcTriggerConfig);
    lpadcTriggerConfig.targetCommandId = 2U; //CMDL
    lpadcTriggerConfig.enableHardwareTrigger = true;
    LPADC_SetConvTriggerConfig(LPADC1, 1U, &lpadcTriggerConfig); // trigger from TCTRL[number]

    /**************************************/
    /*             ADC2                   */
    /**************************************/

    /* Set conversion CMD configuration. */
    LPADC_GetDefaultConvCommandConfig(&lpadcCommandConfig);
    lpadcCommandConfig.channelNumber = g_sM1AdcSensor.sCurrSec23.ui16ChanNumPhaC;
    lpadcCommandConfig.sampleTimeMode = kLPADC_SampleTimeADCK3;
    lpadcCommandConfig.sampleScaleMode = kLPADC_SamplePartScale;
    lpadcCommandConfig.sampleChannelMode = g_sM1AdcSensor.sCurrSec23.ui16ChanSidePhaC;
    LPADC_SetConvCommandConfig(LPADC2, 1U, &lpadcCommandConfig); // CMDL[number]
    /* Set trigger configuration. */
    LPADC_GetDefaultConvTriggerConfig(&lpadcTriggerConfig);
    lpadcTriggerConfig.targetCommandId = 1U; //CMDL
    lpadcTriggerConfig.enableHardwareTrigger = true;
    LPADC_SetConvTriggerConfig(LPADC2, 0U, &lpadcTriggerConfig); // trigger from TCTRL[number]

    
    
    /* Set conversion CMD configuration. */
    LPADC_GetDefaultConvCommandConfig(&lpadcCommandConfig);
    lpadcCommandConfig.channelNumber = g_sM1AdcSensor.ui16ChanNumAux;
    lpadcCommandConfig.sampleTimeMode = kLPADC_SampleTimeADCK3;
    lpadcCommandConfig.sampleScaleMode = kLPADC_SamplePartScale;
    lpadcCommandConfig.sampleChannelMode = g_sM1AdcSensor.ui16ChanSideAux;
    LPADC_SetConvCommandConfig(LPADC2, 2U, &lpadcCommandConfig); // CMDL[number]
    /* Set trigger configuration. */
    LPADC_GetDefaultConvTriggerConfig(&lpadcTriggerConfig);
    lpadcTriggerConfig.targetCommandId = 2U; //CMDL
    lpadcTriggerConfig.enableHardwareTrigger = true;
    LPADC_SetConvTriggerConfig(LPADC2, 1U, &lpadcTriggerConfig); // trigger from TCTRL[number]

}

/*!
* @brief   void InitPWM_M1(void)
*           - Initialization of the eFlexPWMA peripheral for motor M1
*           - 3-phase center-aligned PWM
*
* @param   void
*
* @return  none
*/
void M1_InitPWM(void)
{
    /* PWM base pointer (affects the entire initialization) */
    PWM_Type *PWMBase = (PWM_Type *)PWM1;

    CLOCK_EnableClock(kCLOCK_Pwm1);

    /* Full cycle reload */
    PWMBase->SM[0].CTRL |= PWM_CTRL_FULL_MASK;
    PWMBase->SM[1].CTRL |= PWM_CTRL_FULL_MASK;
    PWMBase->SM[2].CTRL |= PWM_CTRL_FULL_MASK;

    /* Value register initial values, duty cycle 50% */
    PWMBase->SM[0].INIT = PWM_INIT_INIT((uint16_t)(-(g_sClockSetup.ui16M1PwmModulo / 2)));
    PWMBase->SM[1].INIT = PWM_INIT_INIT((uint16_t)(-(g_sClockSetup.ui16M1PwmModulo / 2)));
    PWMBase->SM[2].INIT = PWM_INIT_INIT((uint16_t)(-(g_sClockSetup.ui16M1PwmModulo / 2)));

    PWMBase->SM[0].VAL0 = PWM_VAL0_VAL0((uint16_t)(0));
    PWMBase->SM[1].VAL0 = PWM_VAL0_VAL0((uint16_t)(0));
    PWMBase->SM[2].VAL0 = PWM_VAL0_VAL0((uint16_t)(0));

    PWMBase->SM[0].VAL1 = PWM_VAL1_VAL1((uint16_t)((g_sClockSetup.ui16M1PwmModulo / 2) - 1));
    PWMBase->SM[1].VAL1 = PWM_VAL1_VAL1((uint16_t)((g_sClockSetup.ui16M1PwmModulo / 2) - 1));
    PWMBase->SM[2].VAL1 = PWM_VAL1_VAL1((uint16_t)((g_sClockSetup.ui16M1PwmModulo / 2) - 1));

    PWMBase->SM[0].VAL2 = PWM_VAL2_VAL2((uint16_t)(-(g_sClockSetup.ui16M1PwmModulo / 4)));
    PWMBase->SM[1].VAL2 = PWM_VAL2_VAL2((uint16_t)(-(g_sClockSetup.ui16M1PwmModulo / 4)));
    PWMBase->SM[2].VAL2 = PWM_VAL2_VAL2((uint16_t)(-(g_sClockSetup.ui16M1PwmModulo / 4)));

    PWMBase->SM[0].VAL3 = PWM_VAL3_VAL3((uint16_t)(g_sClockSetup.ui16M1PwmModulo / 4));
    PWMBase->SM[1].VAL3 = PWM_VAL3_VAL3((uint16_t)(g_sClockSetup.ui16M1PwmModulo / 4));
    PWMBase->SM[2].VAL3 = PWM_VAL3_VAL3((uint16_t)(g_sClockSetup.ui16M1PwmModulo / 4));

    /* Trigger for ADC synchronization */
    PWMBase->SM[0].VAL4 = PWM_VAL4_VAL4((uint16_t)((-(g_sClockSetup.ui16M1PwmModulo / 2) + 10)));
    PWMBase->SM[1].VAL4 = PWM_VAL4_VAL4((uint16_t)(0));
    PWMBase->SM[2].VAL4 = PWM_VAL4_VAL4((uint16_t)(0));

    PWMBase->SM[0].VAL5 = PWM_VAL5_VAL5((uint16_t)(0));
    PWMBase->SM[1].VAL5 = PWM_VAL5_VAL5((uint16_t)(0));
    PWMBase->SM[2].VAL5 = PWM_VAL5_VAL5((uint16_t)(0));

    /* PWM0 module 0 trigger on VAL4 enabled for ADC synchronization */
    PWMBase->SM[0].TCTRL |= PWM_TCTRL_OUT_TRIG_EN(1 << 4);

    /* Set dead-time register */
    PWMBase->SM[0].DTCNT0 = PWM_DTCNT0_DTCNT0(g_sClockSetup.ui16M1PwmDeadTime);
    PWMBase->SM[1].DTCNT0 = PWM_DTCNT0_DTCNT0(g_sClockSetup.ui16M1PwmDeadTime);
    PWMBase->SM[2].DTCNT0 = PWM_DTCNT0_DTCNT0(g_sClockSetup.ui16M1PwmDeadTime);
    PWMBase->SM[0].DTCNT1 = PWM_DTCNT1_DTCNT1(g_sClockSetup.ui16M1PwmDeadTime);
    PWMBase->SM[1].DTCNT1 = PWM_DTCNT1_DTCNT1(g_sClockSetup.ui16M1PwmDeadTime);
    PWMBase->SM[2].DTCNT1 = PWM_DTCNT1_DTCNT1(g_sClockSetup.ui16M1PwmDeadTime);

    /* Channels A and B disabled when fault 0 occurs */
    PWMBase->SM[0].DISMAP[0] = ((PWMBase->SM[0].DISMAP[0] & ~PWM_DISMAP_DIS0A_MASK) | PWM_DISMAP_DIS0A(0x1));
    PWMBase->SM[1].DISMAP[0] = ((PWMBase->SM[0].DISMAP[0] & ~PWM_DISMAP_DIS0A_MASK) | PWM_DISMAP_DIS0A(0x1));
    PWMBase->SM[2].DISMAP[0] = ((PWMBase->SM[0].DISMAP[0] & ~PWM_DISMAP_DIS0A_MASK) | PWM_DISMAP_DIS0A(0x1));
    PWMBase->SM[0].DISMAP[0] = ((PWMBase->SM[0].DISMAP[0] & ~PWM_DISMAP_DIS0B_MASK) | PWM_DISMAP_DIS0B(0x1));
    PWMBase->SM[1].DISMAP[0] = ((PWMBase->SM[0].DISMAP[0] & ~PWM_DISMAP_DIS0B_MASK) | PWM_DISMAP_DIS0B(0x1));
    PWMBase->SM[2].DISMAP[0] = ((PWMBase->SM[0].DISMAP[0] & ~PWM_DISMAP_DIS0B_MASK) | PWM_DISMAP_DIS0B(0x1));

    /* Modules one and two gets clock from module zero */
    PWMBase->SM[1].CTRL2 = (PWMBase->SM[1].CTRL2 & ~PWM_CTRL2_CLK_SEL_MASK) | PWM_CTRL2_CLK_SEL(0x2);
    PWMBase->SM[2].CTRL2 = (PWMBase->SM[2].CTRL2 & ~PWM_CTRL2_CLK_SEL_MASK) | PWM_CTRL2_CLK_SEL(0x2);

    /* Master reload active for modules one and two */
    PWMBase->SM[1].CTRL2 |= PWM_CTRL2_RELOAD_SEL_MASK;
    PWMBase->SM[2].CTRL2 |= PWM_CTRL2_RELOAD_SEL_MASK;

    /* Master reload is generated every one opportunity */
    PWMBase->SM[0].CTRL = (PWMBase->SM[0].CTRL & ~PWM_CTRL_LDFQ_MASK) | PWM_CTRL_LDFQ(M1_FOC_FREQ_VS_PWM_FREQ - 1);

    /* Master sync active for modules one and three*/
    PWMBase->SM[1].CTRL2 = (PWMBase->SM[1].CTRL2 & ~PWM_CTRL2_INIT_SEL_MASK) | PWM_CTRL2_INIT_SEL(0x2);
    PWMBase->SM[2].CTRL2 = (PWMBase->SM[2].CTRL2 & ~PWM_CTRL2_INIT_SEL_MASK) | PWM_CTRL2_INIT_SEL(0x2);

    /* Fault 0 active in logic level one, automatic clearing */
    PWMBase->FCTRL = (PWMBase->FCTRL & ~PWM_FCTRL_FLVL_MASK) | PWM_FCTRL_FLVL(0x1);
    PWMBase->FCTRL = (PWMBase->FCTRL & ~PWM_FCTRL_FAUTO_MASK) | PWM_FCTRL_FAUTO(0x1);

    /* Clear fault flags */
    PWMBase->FSTS = (PWMBase->FCTRL & ~PWM_FSTS_FFLAG_MASK) | PWM_FSTS_FFLAG(0xF);

    /* PWMs are re-enabled at PWM full cycle */
    PWMBase->FSTS = (PWMBase->FSTS & ~PWM_FSTS_FFULL_MASK) | PWM_FSTS_FFULL(0x1);

    /* PWM fault filter - 5 Fast peripheral clocks sample rate, 5 agreeing
       samples to activate */
    PWMBase->FFILT = (PWMBase->FFILT & ~PWM_FFILT_FILT_PER_MASK) | PWM_FFILT_FILT_PER(5);
    PWMBase->FFILT = (PWMBase->FFILT & ~PWM_FFILT_FILT_CNT_MASK) | PWM_FFILT_FILT_CNT(5);

    /* Enable A&B PWM outputs for submodules zero, one and three */
    PWMBase->OUTEN = (PWMBase->OUTEN & ~PWM_OUTEN_PWMA_EN_MASK) | PWM_OUTEN_PWMA_EN(0x7);
    PWMBase->OUTEN = (PWMBase->OUTEN & ~PWM_OUTEN_PWMB_EN_MASK) | PWM_OUTEN_PWMB_EN(0x7);

    /* Start PWMs (set load OK flags and run) */
    PWMBase->MCTRL = (PWMBase->MCTRL & ~PWM_MCTRL_CLDOK_MASK) | PWM_MCTRL_CLDOK(0xF);
    PWMBase->MCTRL = (PWMBase->MCTRL & ~PWM_MCTRL_LDOK_MASK) | PWM_MCTRL_LDOK(0xF);
    PWMBase->MCTRL = (PWMBase->MCTRL & ~PWM_MCTRL_RUN_MASK) | PWM_MCTRL_RUN(0xF);

    /* Initialize MC driver */
    g_sM1Pwm3ph.pui32PwmBaseAddress = (PWM_Type *)PWMBase;

    g_sM1Pwm3ph.ui16PhASubNum = M1_PWM_PAIR_PHA; /* PWMA phase A sub-module number */
    g_sM1Pwm3ph.ui16PhBSubNum = M1_PWM_PAIR_PHB; /* PWMA phase B sub-module number */
    g_sM1Pwm3ph.ui16PhCSubNum = M1_PWM_PAIR_PHC; /* PWMA phase C sub-module number */

    g_sM1Pwm3ph.ui16FaultFixNum = M1_FAULT_NUM; /* PWMA fixed-value over-current fault number */
    g_sM1Pwm3ph.ui16FaultAdjNum = M1_FAULT_NUM; /* PWMA adjustable over-current fault number */
}

/*!
* @brief   void InitXBARA(void)
*           - Initialization of the XBARA peripheral
*
* @param   void
*
* @return  none
*/
void InitXBARA(void)
{
    /* Init xbara module. */
    XBARA_Init(XBARA1);

    /* PWM Out to ADC_ETC hardware trigger */
    XBARA_SetSignalsConnection(XBARA1, 74, 155);

    /* M1 Enc phase A */
    XBARA_SetSignalsConnection(XBARA1, 38, 108);

    /* M1 Enc phase B */
    XBARA_SetSignalsConnection(XBARA1, 39, 109);

    /* M1 Enc Index */
    XBARA_SetSignalsConnection(XBARA1, 40, 110);
}

/*!
* @brief   void InitADC_ETC(void)
*           - Initialization of the ADC_ETC peripheral
*
* @param   void
*
* @return  none
*/
void InitADC_ETC(void)
{   
    adc_etc_config_t adcEtcConfig;
    adc_etc_trigger_config_t adcEtcTriggerConfig;
    adc_etc_trigger_chain_config_t adcEtcTriggerChainConfig;

    /* Initialize the ADC_ETC. */
    ADC_ETC_GetDefaultConfig(&adcEtcConfig);
    adcEtcConfig.XBARtriggerMask = 1U; /* Enable the external XBAR trigger0. */
    ADC_ETC_Init(ADC_ETC, &adcEtcConfig);

    /* Set the external XBAR trigger0 configuration. */
    adcEtcTriggerConfig.enableSyncMode      = true;
    adcEtcTriggerConfig.enableSWTriggerMode = false;
    adcEtcTriggerConfig.triggerChainLength  = 1U; /* Chain length 1. */
    adcEtcTriggerConfig.triggerPriority     = 0U;
    adcEtcTriggerConfig.sampleIntervalDelay = 0U;
    adcEtcTriggerConfig.initialDelay        = 0U;
    ADC_ETC_SetTriggerConfig(ADC_ETC, 0U, &adcEtcTriggerConfig);

    /* Set the external XBAR trigger0 chain configuration. */
    adcEtcTriggerChainConfig.enableB2BMode       = true;

    adcEtcTriggerChainConfig.ADCHCRegisterSelect = 1U << 0U; /* Select ADC_HC0 register to trigger. */
    adcEtcTriggerChainConfig.ADCChannelSelect = 0U; /* ADC_HC0 will be triggered to sample Corresponding channel. */
    adcEtcTriggerChainConfig.InterruptEnable = kADC_ETC_Done0InterruptEnable; /* Enable the Done0 interrupt. */
    adcEtcTriggerChainConfig.enableIrq = false; /* Enable the IRQ. */
    ADC_ETC_SetTriggerChainConfig(ADC_ETC, 0U, 0U, &adcEtcTriggerChainConfig); /* Configure the trigger0 chain0. */

    adcEtcTriggerChainConfig.ADCHCRegisterSelect = 1U << 1U; /* Select ADC_HC1 register to trigger. */
    adcEtcTriggerChainConfig.ADCChannelSelect = 0U; /* ADC_HC1 will be triggered to sample Corresponding channel. */
    adcEtcTriggerChainConfig.InterruptEnable = kADC_ETC_Done0InterruptEnable; /* Enable the Done1 interrupt. */
    adcEtcTriggerChainConfig.enableIrq = false; /* Enable the IRQ. */
    ADC_ETC_SetTriggerChainConfig(ADC_ETC, 0U, 1U, &adcEtcTriggerChainConfig); /* Configure the trigger0 group chain 1. */


    /* Set the external XBAR trigger4 configuration. */
    adcEtcTriggerConfig.enableSyncMode      = false;
    adcEtcTriggerConfig.enableSWTriggerMode = false;
    adcEtcTriggerConfig.triggerChainLength  = 1U; /* Chain length 2. */
    adcEtcTriggerConfig.triggerPriority     = 0U;
    adcEtcTriggerConfig.sampleIntervalDelay = 0U;
    adcEtcTriggerConfig.initialDelay        = 0U;
    ADC_ETC_SetTriggerConfig(ADC_ETC, 4U, &adcEtcTriggerConfig);

    adcEtcTriggerChainConfig.ADCHCRegisterSelect = 1U << 0U; /* Select ADC_HC0 register to trigger. */
    adcEtcTriggerChainConfig.ADCChannelSelect = 0U; /* ADC_HC0 will be triggered to sample Corresponding channel. */
    adcEtcTriggerChainConfig.InterruptEnable = kADC_ETC_Done0InterruptEnable; /* Enable the Done0 interrupt. */
    adcEtcTriggerChainConfig.enableIrq = false; /* Enable the IRQ. */
    ADC_ETC_SetTriggerChainConfig(ADC_ETC, 4U, 0U, &adcEtcTriggerChainConfig); /* Configure the trigger4 chain0. */

    adcEtcTriggerChainConfig.ADCHCRegisterSelect = 1U << 1U; /* Select ADC_HC1 register to trigger. */
    adcEtcTriggerChainConfig.ADCChannelSelect = 0U; /* ADC_HC1 will be triggered to sample Corresponding channel. */
    adcEtcTriggerChainConfig.InterruptEnable = kADC_ETC_Done0InterruptEnable; /* Enable the Done1 interrupt. */
    adcEtcTriggerChainConfig.enableIrq = true; /* Enable the IRQ. */
    ADC_ETC_SetTriggerChainConfig(ADC_ETC, 4U, 1U, &adcEtcTriggerChainConfig); /* Configure the trigger4 group chain 1. */

    /* Enable the NVIC. */
    EnableIRQ(ADC_ETC_IRQ0_IRQn);
    NVIC_SetPriority(ADC_ETC_IRQ0_IRQn, 1U);
}

/*!
* @brief   void InitQD(void)
*           - Initialization of the Quadrature Encoder peripheral
*           - performs speed and position sensor processing
*
* @param   void
*
* @return  none
*/
void M1_InitQD(void)
{
    /* Enable clock to ENC modules */
    CLOCK_EnableClock(kCLOCK_Enc1);

    /* Enable position counter reset w/ index signal */
    ENC1->CTRL = ENC_CTRL_XIP_MASK;
    ENC1->CTRL2 = 0;

    /* Pass initialization data into encoder driver structure */
    /* encoder position and speed measurement */
    g_sM1Enc.pui32QdBase = (ENC_Type *) ENC1;
    g_sM1Enc.sTo.fltPGain = M1_POSPE_KP_GAIN;
    g_sM1Enc.sTo.fltIGain = M1_POSPE_KI_GAIN;
    g_sM1Enc.sTo.fltThGain = M1_POSPE_INTEG_GAIN;
    g_sM1Enc.a32PosMeGain = M1_POSPE_MECH_POS_GAIN;
    g_sM1Enc.ui16Pp = M1_MOTOR_PP;
    g_sM1Enc.bDirection = M1_POSPE_ENC_DIRECTION;
    g_sM1Enc.fltSpdEncMin = M1_POSPE_ENC_N_MIN;
    g_sM1Enc.ui32NbEncoderCounts = NB_COUNTS_ENCODER;
    g_sM1Enc.fMaxRotationPerPeriod = ((float)MAX_VEL_SPEC_RPS / (float)M1_PWM_FREQ) * (float)NB_COUNTS_ENCODER;
    g_sM1Enc.ui32CountRevJumps = 0;
    MCDRV_QdEncSetDirection(&g_sM1Enc);
}

void ADC_ETC_IRQ0_IRQHandler(void)
{
    /* M1 State machine */
    SM_StateMachineFast(&g_sM1Ctrl);

    ADC_ETC_ClearInterruptStatusFlags(ADC_ETC, kADC_ETC_Trg0TriggerSource, kADC_ETC_Done0StatusFlagMask);
    ADC_ETC_ClearInterruptStatusFlags(ADC_ETC, kADC_ETC_Trg4TriggerSource, kADC_ETC_Done0StatusFlagMask);
    // FIXME, workaround for extra samples seen in the fifo after interrupt that trigger unwanted calls of the handler
    LPADC_DoResetFIFO(LPADC1);
    LPADC_DoResetFIFO(LPADC2);
}
