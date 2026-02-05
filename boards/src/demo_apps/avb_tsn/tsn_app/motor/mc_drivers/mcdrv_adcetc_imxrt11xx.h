/*
 * Copyright 2013-2015 Freescale Semiconductor, Inc.
 * Copyright 2016-2021, 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _MCDRV_ADCETC_IMXRT11xx_H_
#define _MCDRV_ADCETC_IMXRT11xx_H_

#include "gdflib.h"
#include "mlib_types.h"
#include "gmclib.h"
#include "fsl_device_registers.h"
#include "fsl_adc_etc.h"
#include "fsl_lpadc.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

typedef struct _pha_bc
{
    GDFLIB_FILTER_MA_T_A32 ui16FiltPhaB;           /* phase B offset filter */
    GDFLIB_FILTER_MA_T_A32 ui16FiltPhaC;           /* phase C offset filter */
    uint16_t ui16TriggerGroupPhB;                  /* phase B trigger group */
    uint16_t ui16TriggerGroupPhC;                  /* phase C trigger group */
    uint16_t ui16ChainGroupPhB;                    /* phase B chain group */
    uint16_t ui16ChainGroupPhC;                    /* phase C chain group */
    uint16_t ui16TargetCmdPhB;                     /* phase B target command */
    uint16_t ui16TargetCmdPhC;                     /* phase C target command */
    uint16_t ui16CalibPhaB;                        /* phase B offset calibration */
    uint16_t ui16CalibPhaC;                        /* phase C offset calibration */
    uint16_t ui16OffsetPhaB;                       /* phase B offset result */
    uint16_t ui16OffsetPhaC;                       /* phase C offset result */
    ADC_Type *pAdcBasePhaB;                        /* pointer to phase B base address */
    ADC_Type *pAdcBasePhaC;                        /* pointer to phase C base address */
} pha_bc_t;                                        
                                                   
typedef struct _pha_ac                             
{                                                  
    GDFLIB_FILTER_MA_T_A32 ui16FiltPhaA;           /* phase A offset filter */
    GDFLIB_FILTER_MA_T_A32 ui16FiltPhaC;           /* phase C offset filter */
    uint16_t ui16TriggerGroupPhA;                  /* phase A trigger group */
    uint16_t ui16TriggerGroupPhC;                  /* phase C trigger group */
    uint16_t ui16ChainGroupPhA;                    /* phase A chain group */
    uint16_t ui16ChainGroupPhC;                    /* phase C chain group */
    uint16_t ui16TargetCmdPhA;                     /* phase A target command */
    uint16_t ui16TargetCmdPhC;                     /* phase C target command */
    uint16_t ui16CalibPhaA;                        /* phase A offset calibration */
    uint16_t ui16CalibPhaC;                        /* phase C offset calibration */
    uint16_t ui16OffsetPhaA;                       /* phase A offset result */
    uint16_t ui16OffsetPhaC;                       /* phase C offset result */
    ADC_Type *pAdcBasePhaA;                        /* pointer to phase A base address */
    ADC_Type *pAdcBasePhaC;                        /* pointer to phase C base address */
} pha_ac_t;                                        
                                                   
typedef struct _pha_ab                             
{                                                  
    GDFLIB_FILTER_MA_T_A32 ui16FiltPhaA;           /* phase A offset filter */
    GDFLIB_FILTER_MA_T_A32 ui16FiltPhaB;           /* phase B offset filter */
    uint16_t ui16TriggerGroupPhA;                  /* phase A trigger group */
    uint16_t ui16TriggerGroupPhB;                  /* phase B trigger group */
    uint16_t ui16ChainGroupPhA;                    /* phase A chain group */
    uint16_t ui16ChainGroupPhB;                    /* phase B chain group */
    uint16_t ui16TargetCmdPhA;                     /* phase A target command */
    uint16_t ui16TargetCmdPhB;                     /* phase B target command */
    uint16_t ui16CalibPhaA;                        /* phase A offset calibration */
    uint16_t ui16CalibPhaB;                        /* phase B offset calibration */
    uint16_t ui16OffsetPhaA;                       /* phase A offset result */
    uint16_t ui16OffsetPhaB;                       /* phase B offset result */
    ADC_Type *pAdcBasePhaA;                        /* pointer to phase A base address */
    ADC_Type *pAdcBasePhaB;                        /* pointer to phase B base address */
} pha_ab_t;

typedef struct _mcdrv_adcetc
{
    GMCLIB_3COOR_T_F16 *psIABC; /* pointer to the 3-phase currents */
    pha_bc_t sCurrSec16;        /* ADC setting for SVM sectors 1&6 */
    pha_ac_t sCurrSec23;        /* ADC setting for SVM sectors 2&3 */
    pha_ab_t sCurrSec45;        /* ADC setting for SVM sectors 4&5 */

    ADC_Type *pui32UdcbAdcBase; /* pointer to ADC where Udcb channel is assigned */
    ADC_Type *pui32AuxAdcBase;  /* pointer to ADC where auxiliary channel is assigned */
    uint16_t *pui16SVMSector;   /* pointer to the SVM sector */
    frac16_t *pui16AuxChan;     /* pointer to auxiliary ADC channel number */
    frac16_t *pf16UDcBus;       /* pointer to DC Bus voltage variable */

    uint16_t ui16ChanNumVDcb; /* DCB voltage channel number */
    uint16_t ui16ChanSideVDcb; /* DCB voltage channel side */
    uint16_t ui16TriggerGroupDcBus; /* DCB voltage trigger group */
    uint16_t ui16ChainGroupDcBus;

    uint16_t ui16ChanNumAux; /* Auxiliary channel number */
    uint16_t ui16RsltRegAux; /* Auxiliary result register */
    uint16_t *pui16HCAux;    /* pointer to auxiliary signal HC register */

    ADC_ETC_Type *pui32AdcEtcBase;

    uint16_t ui16OffsetFiltWindow; /* ADC Offset filter window */

    float rawPhaseA;
    float rawPhaseB;
    float rawPhaseC;
    float rawDcBus;
    int16_t i16rawPhaseA;
    int16_t i16rawPhaseB;
    int16_t i16rawPhaseC;
    int16_t i16rawDcBus;
    frac16_t f16rawDcBus;
} mcdrv_adcetc_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief Reads and calculates 3 phase samples based on SVM sector
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_Curr3Ph2ShGet(mcdrv_adcetc_t *this);

/*!
 * @brief Set new channel assignment for next sampling based on SVM sector
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_Curr3Ph2ShChanAssign(mcdrv_adcetc_t *this);

/*!
 * @brief Initializes phase current channel offset measurement
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_Curr3Ph2ShCalibInit(mcdrv_adcetc_t *this);

/*!
 * @brief Function reads current samples and filter them based on SVM sector
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_Curr3Ph2ShCalib(mcdrv_adcetc_t *this);

/*!
 * @brief Function passes measured offset values to main structure
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_Curr3Ph2ShCalibSet(mcdrv_adcetc_t *this);

/*!
 * @brief Function reads and passes DCB voltage sample
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_VoltDcBusGet(mcdrv_adcetc_t *this);

/*!
 * @brief Function reads and passes auxiliary sample
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_AuxValGet(mcdrv_adcetc_t *this);

#ifdef __cplusplus
}
#endif

#endif /* _MCDRV_ADCETC_IMXRT11xx_H_ */
