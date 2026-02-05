/*
 * Copyright 2013-2015 Freescale Semiconductor, Inc.
 * Copyright 2016-2022, 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "mcdrv_adcetc_imxrt11xx.h"
#include "fsl_debug_console.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define ADC_VREFH 1.8f            /* Volts */
#define ADC_RANGE 4095.0f         /* 12bit ADC */
#define ADC_SCALE (64.0f / 30.0f) /* v_real = v_measured * ADC_SCALE */

/*******************************************************************************
 * Variables
 ******************************************************************************/

lpadc_conv_command_config_t lpadcCommandConfig1;

/*******************************************************************************
 * Code
 ******************************************************************************/

/*
 * @brief Convert the sample coming from an ADC with a VREFH of 1.8V with a scale
 *        of 30/64 to an int on 15bits that represents a value between 0 and 3.3V
 *
 * @param sample to convert
 *
 * @return a scaled relative value, i.e, (voltage measured / 3.3) * 2^15.
 *
 * @converted value
 */
static int16_t convert_sample_to_legacy_type(float sample)
{
    float voltage;

    voltage = ((sample / ADC_RANGE) * ADC_VREFH) * ADC_SCALE;

    return (voltage / 3.3f) * 32767.0f;
}

/*!
 * @brief Reads and calculates 3 phase samples based on SVM sector
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_Curr3Ph2ShGet(mcdrv_adcetc_t *this)
{
    GMCLIB_3COOR_T_F16 sIABCtemp;

    switch (*this->pui16SVMSector)
    {
        case 2:
        case 3:
            this->rawPhaseA = ADC_ETC_GetADCConversionValue(this->pui32AdcEtcBase, this->sCurrSec23.ui16TriggerGroupPhA, this->sCurrSec23.ui16ChainGroupPhA);
            this->rawPhaseC = ADC_ETC_GetADCConversionValue(this->pui32AdcEtcBase, this->sCurrSec23.ui16TriggerGroupPhC, this->sCurrSec23.ui16ChainGroupPhC);

            this->i16rawPhaseA = convert_sample_to_legacy_type(this->rawPhaseA);
            this->i16rawPhaseC = convert_sample_to_legacy_type(this->rawPhaseC);
            this->i16rawPhaseB = 0;

            sIABCtemp.f16A = MLIB_ShLSat_F16((frac16_t)(this->i16rawPhaseA) - this->sCurrSec23.ui16OffsetPhaA, 1);
            sIABCtemp.f16C = MLIB_ShLSat_F16((frac16_t)(this->i16rawPhaseC) - this->sCurrSec23.ui16OffsetPhaC, 1);
            sIABCtemp.f16B = MLIB_Neg_F16(MLIB_AddSat_F16(sIABCtemp.f16A, sIABCtemp.f16C));
            break;
        case 4:
        case 5:
            this->rawPhaseA = ADC_ETC_GetADCConversionValue(this->pui32AdcEtcBase, this->sCurrSec45.ui16TriggerGroupPhA, this->sCurrSec45.ui16ChainGroupPhA);
            this->rawPhaseB = ADC_ETC_GetADCConversionValue(this->pui32AdcEtcBase, this->sCurrSec45.ui16TriggerGroupPhB, this->sCurrSec45.ui16ChainGroupPhB);
            this->i16rawPhaseA = convert_sample_to_legacy_type(this->rawPhaseA);
            this->i16rawPhaseB = convert_sample_to_legacy_type(this->rawPhaseB);
            this->i16rawPhaseC = 0;
            sIABCtemp.f16A = MLIB_ShLSat_F16((frac16_t)(this->i16rawPhaseA) - this->sCurrSec45.ui16OffsetPhaA, 1);

            sIABCtemp.f16B = MLIB_ShLSat_F16((frac16_t)(this->i16rawPhaseB) - this->sCurrSec45.ui16OffsetPhaB, 1);
            sIABCtemp.f16C = MLIB_Neg_F16(MLIB_AddSat_F16(sIABCtemp.f16A, sIABCtemp.f16B));
            break;
        case 1:
        case 6:
        default:
            this->rawPhaseB = ADC_ETC_GetADCConversionValue(this->pui32AdcEtcBase, this->sCurrSec16.ui16TriggerGroupPhB, this->sCurrSec16.ui16ChainGroupPhB);
            this->rawPhaseC = ADC_ETC_GetADCConversionValue(this->pui32AdcEtcBase, this->sCurrSec16.ui16TriggerGroupPhC, this->sCurrSec16.ui16ChainGroupPhC);

            this->i16rawPhaseB = convert_sample_to_legacy_type(this->rawPhaseB);
            this->i16rawPhaseC = convert_sample_to_legacy_type(this->rawPhaseC);
            this->i16rawPhaseA = 0;
            sIABCtemp.f16B = MLIB_ShLSat_F16((frac16_t)(this->i16rawPhaseB) - this->sCurrSec16.ui16OffsetPhaB, 1);
            sIABCtemp.f16C = MLIB_ShLSat_F16((frac16_t)(this->i16rawPhaseC) - this->sCurrSec16.ui16OffsetPhaC, 1);
            sIABCtemp.f16A = MLIB_Neg_F16(MLIB_AddSat_F16(sIABCtemp.f16B, sIABCtemp.f16C));
            break;
    }
    
    /* pass measured phase currents to the main module structure */
    this->psIABC->f16A = sIABCtemp.f16A;
    this->psIABC->f16B = sIABCtemp.f16B;
    this->psIABC->f16C = sIABCtemp.f16C;

}


/*!
 * @brief Set new channel assignment for next sampling based on SVM sector
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_Curr3Ph2ShChanAssign(mcdrv_adcetc_t *this)
{
    lpadc_conv_trigger_config_t mLpadcTriggerConfigStruct;
    LPADC_GetDefaultConvTriggerConfig(&mLpadcTriggerConfigStruct);
    
    switch (*this->pui16SVMSector)
    {
        /* direct sensing of phases A and C */
        case 2:
        case 3:
          
            /* Trigger phase A sensing */
            mLpadcTriggerConfigStruct.targetCommandId = this->sCurrSec23.ui16TargetCmdPhA;
            mLpadcTriggerConfigStruct.enableHardwareTrigger = true;
            LPADC_SetConvTriggerConfig(this->sCurrSec23.pAdcBasePhaA, 0, &mLpadcTriggerConfigStruct);
            
            /* Trigger phase C sensing */
            mLpadcTriggerConfigStruct.targetCommandId = this->sCurrSec23.ui16TargetCmdPhC;
            mLpadcTriggerConfigStruct.enableHardwareTrigger = true;
            LPADC_SetConvTriggerConfig(this->sCurrSec23.pAdcBasePhaC, 0, &mLpadcTriggerConfigStruct);
            break;

        /* direct sensing of phases A and B  */
        case 4:
        case 5:

            /* Trigger phase A sensing */
            mLpadcTriggerConfigStruct.targetCommandId = this->sCurrSec45.ui16TargetCmdPhA;
            mLpadcTriggerConfigStruct.enableHardwareTrigger = true;
            LPADC_SetConvTriggerConfig(this->sCurrSec45.pAdcBasePhaA, 0, &mLpadcTriggerConfigStruct);
          
            /* Trigger phase V sensing */
            mLpadcTriggerConfigStruct.targetCommandId = this->sCurrSec45.ui16TargetCmdPhB;
            mLpadcTriggerConfigStruct.enableHardwareTrigger = true;
            LPADC_SetConvTriggerConfig(this->sCurrSec45.pAdcBasePhaB, 0, &mLpadcTriggerConfigStruct);
            break;

        /* direct sensing of phases B and C */
        case 1:
        case 6:
        default:
    
            /* Trigger phase B sensing */
            mLpadcTriggerConfigStruct.targetCommandId       = this->sCurrSec16.ui16TargetCmdPhB;
            mLpadcTriggerConfigStruct.enableHardwareTrigger = true;
            LPADC_SetConvTriggerConfig(this->sCurrSec16.pAdcBasePhaB, 0, &mLpadcTriggerConfigStruct);

            /* Trigger phase C sensing */
            mLpadcTriggerConfigStruct.targetCommandId = this->sCurrSec16.ui16TargetCmdPhC;
            mLpadcTriggerConfigStruct.enableHardwareTrigger = true;
            LPADC_SetConvTriggerConfig(this->sCurrSec16.pAdcBasePhaC, 0, &mLpadcTriggerConfigStruct);
            break;
    }
    
}

/*!
 * @brief Initializes phase current channel offset measurement
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_Curr3Ph2ShCalibInit(mcdrv_adcetc_t *this)
{

    /* clear offset values */
    this->sCurrSec16.ui16OffsetPhaB = 0;
    this->sCurrSec16.ui16OffsetPhaC = 0;
    this->sCurrSec23.ui16OffsetPhaA = 0;
    this->sCurrSec23.ui16OffsetPhaC = 0;
    this->sCurrSec45.ui16OffsetPhaA = 0;
    this->sCurrSec45.ui16OffsetPhaB = 0;

    this->sCurrSec16.ui16CalibPhaB = 0;
    this->sCurrSec16.ui16CalibPhaC = 0;
    this->sCurrSec23.ui16CalibPhaA = 0;
    this->sCurrSec23.ui16CalibPhaC = 0;
    this->sCurrSec45.ui16CalibPhaA = 0;
    this->sCurrSec45.ui16CalibPhaB = 0;

    /* initialize offset filters */
    this->sCurrSec16.ui16FiltPhaB.u16Sh = this->ui16OffsetFiltWindow;
    this->sCurrSec16.ui16FiltPhaC.u16Sh = this->ui16OffsetFiltWindow;
    this->sCurrSec23.ui16FiltPhaA.u16Sh = this->ui16OffsetFiltWindow;
    this->sCurrSec23.ui16FiltPhaC.u16Sh = this->ui16OffsetFiltWindow;
    this->sCurrSec45.ui16FiltPhaA.u16Sh = this->ui16OffsetFiltWindow;
    this->sCurrSec45.ui16FiltPhaB.u16Sh = this->ui16OffsetFiltWindow;

    GDFLIB_FilterMAInit_F16((frac16_t)0, &this->sCurrSec16.ui16FiltPhaB);
    GDFLIB_FilterMAInit_F16((frac16_t)0, &this->sCurrSec16.ui16FiltPhaC);
    GDFLIB_FilterMAInit_F16((frac16_t)0, &this->sCurrSec23.ui16FiltPhaA);
    GDFLIB_FilterMAInit_F16((frac16_t)0, &this->sCurrSec23.ui16FiltPhaC);
    GDFLIB_FilterMAInit_F16((frac16_t)0, &this->sCurrSec45.ui16FiltPhaA);
    GDFLIB_FilterMAInit_F16((frac16_t)0, &this->sCurrSec45.ui16FiltPhaB);


        LPADC_DoResetFIFO(LPADC1);
        LPADC_DoResetFIFO(LPADC2);
        LPADC1->STAT |= 0x2; // Clear FIFO overflow LPADC1 flag
        LPADC2->STAT |= 0x2; // Clear FIFO overflow LPADC2 flag

}

/*!
 * @brief Function reads current samples and filter them based on SVM sector
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_Curr3Ph2ShCalib(mcdrv_adcetc_t *this)
{
    int16_t temp_phA, temp_phB, temp_phC;
    float ftemp_phA, ftemp_phB, ftemp_phC;

    switch (*this->pui16SVMSector)
    {
        case 2:
        case 3:
            /* sensing of offset IA and IC */
            ftemp_phA = ADC_ETC_GetADCConversionValue(this->pui32AdcEtcBase, this->sCurrSec23.ui16TriggerGroupPhA, this->sCurrSec23.ui16ChainGroupPhA);
            ftemp_phC = ADC_ETC_GetADCConversionValue(this->pui32AdcEtcBase, this->sCurrSec23.ui16TriggerGroupPhC, this->sCurrSec23.ui16ChainGroupPhC);

            temp_phA = convert_sample_to_legacy_type(ftemp_phA);
            temp_phC = convert_sample_to_legacy_type(ftemp_phC);

            this->sCurrSec23.ui16CalibPhaA = GDFLIB_FilterMA_F16((frac16_t)(temp_phA) , &this->sCurrSec23.ui16FiltPhaA);

            this->sCurrSec23.ui16CalibPhaC = GDFLIB_FilterMA_F16((frac16_t)(temp_phC), &this->sCurrSec23.ui16FiltPhaC);
            break;
        case 4:
        case 5:
            /* sensing of offset IA and IB */
            ftemp_phA = ADC_ETC_GetADCConversionValue(this->pui32AdcEtcBase, this->sCurrSec45.ui16TriggerGroupPhA, this->sCurrSec45.ui16ChainGroupPhA);
            ftemp_phB = ADC_ETC_GetADCConversionValue(this->pui32AdcEtcBase, this->sCurrSec45.ui16TriggerGroupPhB, this->sCurrSec45.ui16ChainGroupPhB);

            temp_phA = convert_sample_to_legacy_type(ftemp_phA);
            temp_phB = convert_sample_to_legacy_type(ftemp_phB);

            this->sCurrSec45.ui16CalibPhaA = GDFLIB_FilterMA_F16((frac16_t)(temp_phA), &this->sCurrSec45.ui16FiltPhaA);
            this->sCurrSec45.ui16CalibPhaB = GDFLIB_FilterMA_F16((frac16_t)(temp_phB), &this->sCurrSec45.ui16FiltPhaB);
            break;
        case 1:
        case 6:
        default:
            /* sensing of offset IB and IC */
            ftemp_phB = ADC_ETC_GetADCConversionValue(this->pui32AdcEtcBase, this->sCurrSec16.ui16TriggerGroupPhB, this->sCurrSec16.ui16ChainGroupPhB);
            ftemp_phC = ADC_ETC_GetADCConversionValue(this->pui32AdcEtcBase, this->sCurrSec16.ui16TriggerGroupPhC, this->sCurrSec16.ui16ChainGroupPhC);

            temp_phB = convert_sample_to_legacy_type(ftemp_phB);
            temp_phC = convert_sample_to_legacy_type(ftemp_phC);

            this->sCurrSec16.ui16CalibPhaB = GDFLIB_FilterMA_F16((frac16_t)(temp_phB), &this->sCurrSec16.ui16FiltPhaB);
            this->sCurrSec16.ui16CalibPhaC = GDFLIB_FilterMA_F16((frac16_t)(temp_phC), &this->sCurrSec16.ui16FiltPhaC);
            break;
    }

}

/*!
 * @brief Function passes measured offset values to main structure
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_Curr3Ph2ShCalibSet(mcdrv_adcetc_t *this)
{

    /* pass calibration data for sector 1 and 6 */
    this->sCurrSec16.ui16OffsetPhaB = this->sCurrSec16.ui16CalibPhaB;
    this->sCurrSec16.ui16OffsetPhaC = this->sCurrSec16.ui16CalibPhaC;

    /* pass calibration data for sector 2 and 3 */
    this->sCurrSec23.ui16OffsetPhaA = this->sCurrSec23.ui16CalibPhaA;
    this->sCurrSec23.ui16OffsetPhaC = this->sCurrSec23.ui16CalibPhaC;

    /* pass calibration data for sector 4 and 5 */
    this->sCurrSec45.ui16OffsetPhaA = this->sCurrSec45.ui16CalibPhaA;
    this->sCurrSec45.ui16OffsetPhaB = this->sCurrSec45.ui16CalibPhaB;

    PRINTF("%s: 16b: %hu, 16c: %hu, 23a: %hu, 23c: %hu, 45a: %hu, 45b: %hu\n",
           __func__,
           this->sCurrSec16.ui16OffsetPhaB, this->sCurrSec16.ui16OffsetPhaC,
           this->sCurrSec23.ui16OffsetPhaA, this->sCurrSec23.ui16OffsetPhaC,
           this->sCurrSec45.ui16OffsetPhaA, this->sCurrSec45.ui16OffsetPhaB);

}

/*!
 * @brief Function reads and passes DCB voltage sample
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_VoltDcBusGet(mcdrv_adcetc_t *this)
{
    int16_t temp_DcBus;
    this->rawDcBus = ADC_ETC_GetADCConversionValue(this->pui32AdcEtcBase, this->ui16TriggerGroupDcBus, this->ui16ChainGroupDcBus);

    temp_DcBus = convert_sample_to_legacy_type(this->rawDcBus);

    this->f16rawDcBus = (frac16_t)(temp_DcBus);
    *this->pf16UDcBus = this->f16rawDcBus;
  
}

/*!
 * @brief Function reads and passes auxiliary sample
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_AuxValGet(mcdrv_adcetc_t *this)
{

    /* read Auxiliary channel sample from defined ADC2 result register */
}
