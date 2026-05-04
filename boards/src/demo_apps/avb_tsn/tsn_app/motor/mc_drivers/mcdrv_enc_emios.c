/*
* Copyright 2016, Freescale Semiconductor, Inc.
* Copyright 2016-2021, 2024-2026 NXP
*
* NXP Proprietary. This software is owned or controlled by NXP and may
* only be used strictly in accordance with the applicable license terms. 
* By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that
* you have read, and that you agree to comply with and are bound by,
* such license terms.  If you do not agree to be bound by the applicable
* license terms, then you may not retain, install, activate or otherwise
* use the software.
 */

#include "mcdrv_enc_emios.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
   
/*!
 * @brief Function returns actual position and speed
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_QdEncGet(mcdrv_qd_enc_t *this)
{
     
    frac16_t f16Pulses;
    frac16_t f16PosDiff;

    /* number of pulses */
    f16Pulses = (int16_t)EMIOS_0->UC[5U].CNT - (int16_t)EMIOS_0->UC[6U].CNT - this->f16CounterOffset;
        
    /* position difference */
    f16PosDiff = MLIB_Mul_F16as(this->a32PosMeGain, f16Pulses) - (frac16_t)this->a32PosMe;
    
    /* accumulator position */
    this->a32PosMe = this->a32PosMe + f16PosDiff;

    this->i16RevCounter = ((int32_t)this->a32PosMe >> 16);
    this->i16PosRelativeEnc = (int32_t)(((((uint32_t)this->a32PosMe) & 0xFFFFu) * (uint32_t)this->ui32NbEncoderCounts) >> 16);
    this->i32PosAbsoluteEnc = this->i16RevCounter * this->ui32NbEncoderCounts + this->i16PosRelativeEnc;

    /* mechanical position */
    this->f16PosMe = (frac16_t)(this->a32PosMe);

    /* tracking observer calculation */
    this->f16PosMeEst = (frac16_t)AMCLIB_TrackObsrv_A32af(this->a32PosErr, &this->sTo);

    /* calculation of error function for tracking observer */
    this->a32PosErr = (acc32_t)MLIB_Sub_F16(this->f16PosMe, this->f16PosMeEst);

    /* speed estimation by the tracking observer */
    this->fltSpdMeEst = this->sTo.fltSpeed;
    
    /* calculating position for position control */
    *this->pa32PosMeReal = ((this->a32PosMe) >> 1);

    /* store results to user-defined variables */
   *this->pf16PosElEst = (frac16_t)(this->f16PosMeEst * this->ui16Pp);
   *this->pfltSpdMeEst = (this->fltSpdMeEst);

}

/*!
 * @brief Function clears internal variables and decoder counter
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_QdEncClear(mcdrv_qd_enc_t *this)
{

    this->f16PosMe    = 0;
    this->f16PosMeEst = 0;
    this->fltSpdMeEst = 0;

    /* initilize tracking observer */
    this->sTo.f32Theta = 0;
    this->sTo.fltSpeed = 0;
    this->sTo.fltI_1   = 0;

    this->a32PosMe = 0;
     
    /* pulses offset */
    this->f16CounterOffset = (int16_t)EMIOS_0->UC[5U].CNT -((int16_t)EMIOS_0->UC[6U].CNT);

}

/*!
 * @brief Function set mechanical position of quadrature encoder
 *
 * @param this     Pointer to the current object
 *        f16PosMe Mechanical position
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_QdEncSetPosMe(mcdrv_qd_enc_t *this, frac16_t f16PosMe)
{

}

/*!
 * @brief Function set direction of quadrature encoder
 *
 * @param this       Pointer to the current object
 *        bDirection Encoder direction
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_QdEncSetDirection(mcdrv_qd_enc_t *this)
{

}

/*!
 * @brief Function set quadrature encoder pulses per one revolution
 *
 * @param this            Pointer to the current object
 *        ui16PulseNumber Encoder pulses per revolution
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_QdEncSetPulses(mcdrv_qd_enc_t *this)
{

}
