/*
 * Copyright 2016 Freescale Semiconductor, Inc.
 * Copyright 2016-2021, 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "mcdrv_enc_eqd.h"
#include "fsl_eqdc.h"
#ifdef MCDRV_QDDEBUG
#include "fsl_debug_console.h"
#endif

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
#ifdef MCDRV_QDDEBUG
static void MCDRV_QdDebug(mcdrv_eqd_enc_t *this, int16_t rev, int16_t pos)
{
    static int cnt = 0;

    if (cnt++ > 2000) {
        cnt = 0;

        PRINTF("cnt: %hd, sw cnt: %hd, rev: %d, last rev: %d, pos: %d\n",
                pos, this->i16PosRelativeEnc,
                rev, this->i16RevCounter,
                this->i32PosAbsoluteEnc);
    }
}
#else
static inline void MCDRV_QdDebug(mcdrv_eqd_enc_t *this, int16_t rev, int16_t pos){};
#endif

/*!
 * @brief Function returns actual position and speed
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_QdEncGet(mcdrv_eqd_enc_t *this)
{
    int32_t newPos = 0;

   /* The first register read, saves all other values in holding registers */
    int16_t RevCounter = this->pui32QdBase->REV;
    int16_t PosCounter = this->pui32QdBase->LPOSH;

    if (RevCounter == this->i16RevCounter) {
        this->i16PosRelativeEnc += (int16_t)this->pui32QdBase->POSDH;
    }
    /* Re-align absolute position when index is found.
    * Revolution and position counters are not always coherent when the index
    * signal is triggered.
    */
    else {
        /* Going backward, start from a full position counter */
        if (RevCounter < this->i16RevCounter)
            this->i16PosRelativeEnc = this->ui32NbEncoderCounts - 1;
        else
            this->i16PosRelativeEnc = 0;
    }

    if (this->bIndexFound)
       newPos = (RevCounter + this->i16RevCounterCorrection) * this->ui32NbEncoderCounts + this->i16PosRelativeEnc;

    /* Revolution change, we check if the difference of absolute position is consistent
    * If it is not, we must have missed a revolution, then we apply a corrective value to our revolution count */
    if (RevCounter != this->i16RevCounter && this->bIndexFound) {
        uint32_t diffRev;
        int32_t diffPos = this->i32PosAbsoluteEnc - newPos;

        diffRev = (abs(diffPos) + this->ui32NbEncoderCounts / 16) / this->ui32NbEncoderCounts;
        if (diffRev) {
            this->i16RevCounterCorrection += (diffPos > 0) ? diffRev : -diffRev;
            this->ui32CountRevJumps++;
        }
    }

    if (this->bIndexFound)
       this->i32PosAbsoluteEnc = (RevCounter + this->i16RevCounterCorrection) * this->ui32NbEncoderCounts + this->i16PosRelativeEnc;

    MCDRV_QdDebug(this, RevCounter, PosCounter);

    this->i16RevCounter = RevCounter;
    this->ui32RevCounter = this->i16RevCounter;

    if (MCDRV_QdEncGetIndexInterruptStatus(this) && this->bIndexFound) {
        MCDRV_QdEncClearIndexInterruptStatus(this);
        this->ui32LastIndexPosEnc = this->i16PosRelativeEnc;
    }

    /* tracking observer calculation */
    this->f16PosMeEst = (frac16_t)AMCLIB_TrackObsrv_A32af(this->a32PosErr, &this->sTo);

  /* read number of pulses and get mechanical position */
    this->f16PosMe = (frac16_t)(MLIB_Mul_F16as(this->a32PosMeGain, (frac16_t)(this->i16PosRelativeEnc)));

    /* calculation of error function for tracking observer */
    this->a32PosErr = (acc32_t)MLIB_Sub_F16(this->f16PosMe, this->f16PosMeEst);

    /* speed estimation by the tracking observer */
    this->fltSpdMeEst = this->sTo.fltSpeed;

    /* read revolution counter */
    this->f16RevCounter = (frac16_t)(RevCounter);

    /* calculating position for position control */
    this->a32PosMeReal = (acc32_t)( ( ( ((int32_t)(this->f16RevCounter)) << 15) + (((uint16_t)(this->f16PosMe)) >> 1) ) ); 

    /* pass estimator speed values lower than minimal encoder speed */
    if ((MLIB_Abs_FLT(this->fltSpdMeEst) < (this->fltSpdEncMin)))
    {
        this->fltSpdMeEst = 0U;
    }

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
void MCDRV_QdEncClear(mcdrv_eqd_enc_t *this)
{

    this->f16PosMe    = 0;
    this->f16PosMeEst = 0;
    this->fltSpdMeEst = 0;

    /* initilize tracking observer */
    this->sTo.f32Theta = 0;
    this->sTo.fltSpeed = 0;
    this->sTo.fltI_1   = 0;

    /* clear decoder counters */
    this->pui32QdBase->POSD = 0;
    this->pui32QdBase->REV  = 0;
    this->pui32QdBase->LPOS = 0;
    this->pui32QdBase->UPOS = 0;

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
void MCDRV_QdEncSetPosMe(mcdrv_eqd_enc_t *this, frac16_t f16PosMe)
{
    frac16_t f16CntMod;

    f16CntMod    = (frac16_t)(this->pui32QdBase->LMOD >> 1);

    /* set mechnical position */
    this->f16PosMe          = f16PosMe;
    this->sTo.f32Theta      = MLIB_Conv_F32s(f16PosMe);
    this->pui32QdBase->LPOS = (uint16_t)(MLIB_Mul_F16(f16PosMe, f16CntMod) + (uint16_t)f16CntMod);

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
void MCDRV_QdEncSetDirection(mcdrv_eqd_enc_t *this)
{

    /* forward/reverse */
    if (this->bDirection)
        this->pui32QdBase->CTRL |= EQDC_CTRL_REV_MASK;
    else
        this->pui32QdBase->CTRL &= ~EQDC_CTRL_REV_MASK;

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
void MCDRV_QdEncSetPulses(mcdrv_eqd_enc_t *this)
{
     /* Set modulo counter to encoder number of pulses * 4 - 1 */
    this->pui32QdBase->LMOD = (this->ui16PulseNumber * 4U) - 1U;
    this->pui32QdBase->CTRL |= EQDC_CTRL_LDOK(1U);
}

/*!
 * @brief Function enable index interrupt of quadrature encoder
 *
 * @param this       Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_QdEncEnIndexInterrupt(mcdrv_eqd_enc_t *this)
{
    EQDC_EnableInterrupts(this->pui32QdBase, kEQDC_IndexPresetPulseInterruptEnable);
    this->pui32QdBase->CTRL |= (1 << EQDC_CTRL_XIRQ_SHIFT);
}

/*!
 * @brief Function Disable index interrupt of quadrature encoder
 *
 * @param this       Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_QdEncDisIndexInterrupt(mcdrv_eqd_enc_t *this)
{
    EQDC_DisableInterrupts(this->pui32QdBase, kEQDC_IndexPresetPulseInterruptEnable);
}

/*!
 * @brief Function Get Status of Index Interrupt
 *
 * @param this       Pointer to the current object
 *
 * @return bool_t true on when index interrupt occured, false otherwise
 */
RAM_FUNC_LIB
bool_t MCDRV_QdEncGetIndexInterruptStatus(mcdrv_eqd_enc_t *this)
{
    return (this->pui32QdBase->CTRL & (1 << EQDC_CTRL_XIRQ_SHIFT));
}

/*!
 * @brief Function Clear Status of Index Interrupt
 *
 * @param this       Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_QdEncClearIndexInterruptStatus(mcdrv_eqd_enc_t *this)
{
    this->pui32QdBase->CTRL  |= (1 << EQDC_CTRL_XIRQ_SHIFT);
}
