/*
 * Copyright 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2021, 2024 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _MCDRV_ENC_QD_H_
#define _MCDRV_ENC_QD_H_

#include "mlib.h"
#include "mlib_types.h"
#include "fsl_device_registers.h"
#include "amclib.h"
#include "pmsm_control.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define MCDRV_QD (1)

typedef struct _mcdrv_qd_enc_t
{
    AMCLIB_TRACK_OBSRV_T_FLT sTo; /* tracking observer structure */
    ENC_Type *pui32QdBase;        /* pointer to QD module base address*/
    float_t *pfltSpdMeEst;        /* pointer to measured mechanical speed  */
    frac16_t *pf16PosElEst;       /* pointer to measured electrical position */
    acc32_t a32PosErr;            /* position error to tracking observer  */
    acc32_t a32PosMeGain;         /* encoder pulses to mechanical position scale gain */
    float_t fltSpdMeEst;          /* estimated speed calculated using tracking observer */
    frac16_t f16PosMe;            /* mechanical position calculated using encoder edges */
    frac16_t f16PosMeEst;         /* estimated position calculated using tracking observer */
    uint16_t ui16Pp;              /* number of motor pole pairs */
    bool_t bDirection;            /* encoder direction */
    float_t fltSpdEncMin;         /* encoder minimal speed resolution */
    frac16_t f16PosErr;           /* poisition error to tracking observer  */
    frac16_t f16PosMeGain;        /* encoder pulses to mechanical position scale gain */
    int16_t i16PosMeGainSh;       /* encoder pulses to mechanical position scale shift */
    acc32_t a32PosMeReal;         /* real position (revolution counter + mechanical position) */
    frac16_t f16RevCounter;       /* revolution counter measured by periphery */
    uint16_t ui16PulseNumber;     /* quadrature pulses per one revolution */
    uint32_t ui32RevCounter;         /* revolution counter measured by periphery */
    int32_t i32PosAbsoluteEnc;       /* Absolute position computed by the encoder in counts */
    int16_t i16PosRelativeEnc;       /* Relative position computed by the encoder in counts */
    bool_t bIndexFound;              /* encoder direction */
    uint32_t ui32LastIndexPosEnc;    /* Last position of encoder index */
    int16_t i16RevCounter;           /* Revolution counter in standard format */
    uint32_t ui32NbEncoderCounts;    /* Number of encoder counts */
    float_t fMaxRotationPerPeriod;   /* Maximum rotation feasible during fast loop period*/
    int16_t i16RevCounterCorrection; /* Correct revolution counter if missed revolution happens*/
    uint32_t ui32CountRevJumps;      /* Counter for revolution jumps */
} mcdrv_qd_enc_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief Function returns actual position and speed
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_QdEncGet(mcdrv_qd_enc_t *this);

/*!
 * @brief Function clears internal variables and decoder counter
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_QdEncClear(mcdrv_qd_enc_t *this);

/*!
 * @brief Function se mechanical position of quadrature encoder
 *
 * @param this     Pointer to the current object
 *        f16PosMe Mechanical position
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_QdEncSetPosMe(mcdrv_qd_enc_t *this, frac16_t f16PosMe);

/*!
 * @brief Function set direction of quadrature encoder
 *
 * @param this       Pointer to the current object
 *        bDirection Encoder direction
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_QdEncSetDirection(mcdrv_qd_enc_t *this);

/*!
 * @brief Function set quadrature encoder pulses per one revolution
 *
 * @param this            Pointer to the current object
 *        ui16PulseNumber Encoder pulses per revolution
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_QdEncSetPulses(mcdrv_qd_enc_t *this);

/*!
 * @brief Function enable index interrupt of quadrature encoder
 *
 * @param this       Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_QdEncEnIndexInterrupt(mcdrv_qd_enc_t *this);

/*!
 * @brief Function disable index interrupt of quadrature encoder
 *
 * @param this       Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_QdEncDisIndexInterrupt(mcdrv_qd_enc_t *this);

/*!
 * @brief Function Get Status of Index Interrupt
 *
 * @param this       Pointer to the current object
 *
 * @return bool_t true on when index interrupt occured, false otherwise
 */
RAM_FUNC_LIB
bool_t MCDRV_QdEncGetIndexInterruptStatus(mcdrv_qd_enc_t *this);

/*!
 * @brief Function Clear Status of Index Interrupt
 *
 * @param this       Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_QdEncClearIndexInterruptStatus(mcdrv_qd_enc_t *this);

#ifdef __cplusplus
}
#endif

#endif /* _MCDRV_ENC_QD_H_ */
