/*
 * Copyright 2016 Freescale Semiconductor, Inc.
 * Copyright 2016-2019, 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_wm8960.h"
#include "fsl_common.h"

/*******************************************************************************
 * Definitations
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
/*
 * wm8960 register cache
 * We can't read the WM8960 register space when we are
 * using 2 wire for device control, so we cache them instead.
 */
static const uint16_t wm8960_reg[WM8960_CACHEREGNUM] = {
    0x0097, 0x0097, 0x0000, 0x0000, 0x0000, 0x0008, 0x0000, 0x000a, 0x01c0, 0x0000, 0x00ff, 0x00ff, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x007b, 0x0100, 0x0032, 0x0000, 0x00c3, 0x00c3, 0x01c0, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0100, 0x0100, 0x0050, 0x0050, 0x0050, 0x0050, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0040, 0x0000, 0x0000, 0x0050, 0x0050, 0x0000, 0x0002, 0x0037, 0x004d, 0x0080, 0x0008, 0x0031, 0x0026, 0x00e9,
};

static uint16_t reg_cache[WM8960_CACHEREGNUM];
/*******************************************************************************
 * Code
 ******************************************************************************/


// Power menagment 1 reg
#define MIC_B_BIT           (1<<1) // Microphone Bias Enable

#define ADCL_BIT            (1<<3) // Enable ADC left channel
#define ADCR_BIT            (1<<2) // Enable ADC right channel

#define AINL_BIT            (1<<5) // Left channel input PGA and boost stage enable
#define AINR_BIT            (1<<4) // Right channel input PGA and boost stage enable

#define VREF_BIT            (1<<6) //
#define VMIDSEL_BIT_0       (1<<7)
#define VMIDSEL_BIT_1       (1<<8)

// Power menagement 2 reg
#define DAC_L_BIT           (1<<8) // DAC Left Power up
#define DAC_R_BIT           (1<<7) // DAC Right Power up

#define LOUT1_BIT           (1<<6) // LOUT1 Output buffer Power up
#define ROUT1_BIT           (1<<5) // ROUT1 Output buffer Power up

// Power menagement 3 reg
#define LMIC_BIT            (1<<5) // Left channel input PGA enable
#define RMIC_BIT            (1<<4) // Right channel input PGA enable

#define LOMIX_BIT           (1<<3) // Left Output Mixer Enable
#define ROMIX_BIT           (1<<2) // Right Output Mixer Enable

// ADC Right Signal Path
#define RMIC2B_BIT          (1<<3) // Connect Right Input PGA to Right Input Boost mixer
// Right Channel Input PGA Boost Gain:
// 0 = 0dB
// 1 = +13dB
// 2 = +20dB
// 3 = +29dB
#define RMIC_BOOST          3
#define RMIC_BOOST_BITS     (RMIC_BOOST<<4)
#define RMN1_BIT            (1<<8) // Connect RINPUT1 to inverting input of Right Input PGA
#define RMP2_BIT            (1<<6) // Connect RINPUT2 to non-inverting input of Right Input PGA
#define RMP3_BIT            (1<<7) // Connect RINPUT3 to non-inverting input of Right Input PGA

// ADC Left Signal Path
#define LMIC2B_BIT          (1<<3) // Connect Left Input PGA to Left Input Boost mixer
#define LMIC_BOOST          0 // Left Channel Input PGA Boost Gain: 11 = +29dB
#define LMIC_BOOST_BITS     (LMIC_BOOST<<4)
#define LMN1_BIT            (1<<8) // Connect LINPUT1 to inverting input of Left Input PGA
#define LMP2_BIT            (1<<6) // Connect LINPTU2 to NON-inverting input of Left Input PGA
#define LMP3_BIT            (1<<7) // Connect LINPTU3 to NON-inverting input of Left Input PGA

// Audio Interface
#define ALRCGPIO_BIT        (1<<6) // ADCLRC/GPIO1 Pin Function Select

// Right input volume
#define IPVU_BIT            (1<<8) // Input PGA Volume Update bit
#define RIN_VOL             (0x27) // Right Input PGA Volume Control, +12dB

// Left Inpt Volume
#define LINMUTE_BIT         (1<<7) // Left Input PGA Analogue Mute

// Clocking
#define ADCDIV              0 // ADC Sample rate divider
#define ADCDIV_BITS         (ADCDIV<<6)
#define DACDIV              0 // DAC Sample rate divider
#define DACDIV_BITS         (DACDIV<<3)
#define SYSCLKDIV           0 // SYSCLK Pre-divider
#define SYSCLKDIV_BITS      (SYSCLKDIV<<1)
#define CLKSEL              0 // SYSCLK Select

// Audio Interface
#define FORMAT              1 // 1 == Left justified
#define FORMAT_BITS         (FORMAT<<0)
#define WORD_LENGTH         0 // Audio Data Word Length: 0 == 16bits
#define WORD_LENGTH_BITS    (WORD_LENGTH<<2)

// Left Out Mix
#define LD2LO_BIT           (1<<8) // Left Dac 2 Left Output
// Right Out Mix
#define RD2RO_BIT           (1<<8) // Right Dac 2 Right Output

// Additional Control 1
#define VSEL                3 // Analog Bias optimisation
#define VSEL_BITS           (VSEL<<6) // Analog Bias optimisation
// ADC Data Output Select:
// 0 left data = left ADC; right data = right ADC
// 1: left data = left ADC; right data = left ADC
// 2: left data = right ADC; right data = right ADC
// 3: left data = right ADC; right data = left ADC
#define DATASEL             0
#define DATASEL_BITS        (DATASEL<<2)
#define DMONOMIX_BIT        (1<<4) // DAC Mono Mix

// Headphone Volume
#define LROUT_VU_BIT (1 << 8) // Volume Update
// 0x7F = +6dB
// 0x7E = +5dB
// ...
// 0x79 = 0dB
#define LOUT1_VOL 0x7F
#define ROUT1_VOL 0x7F

void WM8960_Init(wm8960_handle_t *handle, wm8960_config_t *config)
{
    uint32_t i = 4000000;

    memcpy(reg_cache, wm8960_reg, sizeof(wm8960_reg));

    /* NULL pointer means default setting. */
    if (config == NULL)
    {
        // Reset all registers
        WM8960_WriteReg(handle, WM8960_RESET, 0x00);

        /**
         * Audio Interface
         * ALRCGPIO as GPIO Pin
         */
        WM8960_WriteReg(handle, WM8960_IFACE2, ALRCGPIO_BIT);

        // Power menagment 1
        WM8960_WriteReg(handle, WM8960_POWER1, MIC_B_BIT | /*AINL_BIT |*/ AINR_BIT | /*ADCL_BIT |*/ ADCR_BIT | VREF_BIT | VMIDSEL_BIT_0);

        // Power menagment 2
        WM8960_WriteReg(handle, WM8960_POWER2, DAC_L_BIT | DAC_R_BIT | LOUT1_BIT | ROUT1_BIT);

        // Power menagment 3
        WM8960_WriteReg(handle, WM8960_POWER3, /*LMIC_BIT |*/ RMIC_BIT | LOMIX_BIT | ROMIX_BIT);

        /**
         * ADC Sample rate divider == DAC sample rate divider == SYSCLK / 256 == MCLK / 256
         * SYSCLKDIV = 1
         * CLKSEL = SYSCLK derived from MCLK
         */
        WM8960_WriteReg(handle, WM8960_CLOCK1, ADCDIV_BITS | DACDIV_BITS | SYSCLKDIV_BITS | CLKSEL);

        /*
         * Audio Left justified data format
         * Word length done in ConfigDataFrmat funtion
         */
        WM8960_WriteReg(handle, WM8960_IFACE1, FORMAT_BITS /*| WORD_LENGTH_BITS*/);

        // RMICBOOST = 0dB, Connect right PGA to right Input Boost Mixer
        WM8960_WriteReg(handle, WM8960_RINPATH, RMN1_BIT | RMP2_BIT | RMIC_BOOST_BITS | RMIC2B_BIT);
        // Disable Left Input Path
        WM8960_WriteReg(handle, WM8960_LINPATH, 0x000);

        // Update Right Input Path Volume
        WM8960_WriteReg(handle, WM8960_RINVOL, IPVU_BIT | RIN_VOL);
        // Mute Left Input PGA
        WM8960_WriteReg(handle, WM8960_LINVOL, LINMUTE_BIT);

        // Left DAC to left output mixer, LINPUT3 left output mixer volume = 0dB
        WM8960_WriteReg(handle, WM8960_LOUTMIX, LD2LO_BIT);

        // Right DAC to right output mixer, RINPUT3 right output mixer volume = 0dB
        WM8960_WriteReg(handle, WM8960_ROUTMIX, RD2RO_BIT);

        // Disable Bypass
        WM8960_WriteReg(handle, WM8960_BYPASS1, 0x0);
        WM8960_WriteReg(handle, WM8960_BYPASS2, 0x0);

        // Disable Mono Output Mix
        WM8960_WriteReg(handle, WM8960_MONOMIX1, 0x00);
        WM8960_WriteReg(handle, WM8960_MONOMIX2, 0x00);
    }
    else
    {
        WM8960_SetDataRoute(handle, config->route);
        WM8960_SetProtocol(handle, config->bus);
        WM8960_SetMasterSlave(handle, config->master_slave);
    }

    // Additional control 1
    WM8960_WriteReg(handle, WM8960_ADDCTL1, VSEL_BITS | DATASEL_BITS /*| DMONOMIX_BIT*/);

    // ADC volume 0dB, default value
    WM8960_WriteReg(handle, WM8960_LADC, 0x1C3);
    WM8960_WriteReg(handle, WM8960_RADC, 0x1C3);

    // Digital DAC volume, 0dB
    WM8960_WriteReg(handle, WM8960_LDAC, 0x1FF);
    WM8960_WriteReg(handle, WM8960_RDAC, 0x1FF);

    // Headphone volume, LOUT1 and ROUT1, +6dB
    WM8960_WriteReg(handle, WM8960_LOUT1, LOUT1_VOL | LROUT_VU_BIT);
    WM8960_WriteReg(handle, WM8960_ROUT1, ROUT1_VOL | LROUT_VU_BIT);

    /* Delay for some while */
    while (i)
    {
        __ASM("nop");
        i--;
    }

    // Unmute DAC
    WM8960_WriteReg(handle, WM8960_DACCTL1, 0x0000);
}

void WM8960_Deinit(wm8960_handle_t *handle)
{
    WM8960_SetModule(handle, kWM8960_ModuleADC, false);
    WM8960_SetModule(handle, kWM8960_ModuleDAC, false);
    WM8960_SetModule(handle, kWM8960_ModuleVREF, false);
    WM8960_SetModule(handle, kWM8960_ModuleLineIn, false);
    WM8960_SetModule(handle, kWM8960_ModuleLineOut, false);
    WM8960_SetModule(handle, kWM8960_ModuleSpeaker, false);
}

void WM8960_SetMasterSlave(wm8960_handle_t *handle, bool master)
{
    if (master == 1)
    {
        WM8960_ModifyReg(handle, WM8960_IFACE1, WM8960_IFACE1_MS_MASK, WM8960_IFACE1_MS(WM8960_IFACE1_MASTER));
    }
    else
    {
        WM8960_ModifyReg(handle, WM8960_IFACE1, WM8960_IFACE1_MS_MASK, WM8960_IFACE1_MS(WM8960_IFACE1_SLAVE));
    }
}

status_t WM8960_SetModule(wm8960_handle_t *handle, wm8960_module_t module, bool isEnabled)
{
    status_t ret = kStatus_Success;
    switch (module)
    {
        case kWM8960_ModuleADC:
            WM8960_ModifyReg(handle, WM8960_POWER1, WM8960_POWER1_ADCL_MASK,
                             ((uint16_t)isEnabled << WM8960_POWER1_ADCL_SHIFT));
            WM8960_ModifyReg(handle, WM8960_POWER1, WM8960_POWER1_ADCR_MASK,
                             ((uint16_t)isEnabled << WM8960_POWER1_ADCR_SHIFT));
            break;
        case kWM8960_ModuleDAC:
            WM8960_ModifyReg(handle, WM8960_POWER2, WM8960_POWER2_DACL_MASK,
                             ((uint16_t)isEnabled << WM8960_POWER2_DACL_SHIFT));
            WM8960_ModifyReg(handle, WM8960_POWER2, WM8960_POWER2_DACR_MASK,
                             ((uint16_t)isEnabled << WM8960_POWER2_DACR_SHIFT));
            break;
        case kWM8960_ModuleVREF:
            WM8960_ModifyReg(handle, WM8960_POWER1, WM8960_POWER1_VREF_MASK,
                             ((uint16_t)isEnabled << WM8960_POWER1_VREF_SHIFT));
            break;
        case kWM8960_ModuleLineIn:
            WM8960_ModifyReg(handle, WM8960_POWER1, WM8960_POWER1_AINL_MASK,
                             ((uint16_t)isEnabled << WM8960_POWER1_AINL_SHIFT));
            WM8960_ModifyReg(handle, WM8960_POWER1, WM8960_POWER1_AINR_MASK,
                             ((uint16_t)isEnabled << WM8960_POWER1_AINR_SHIFT));
            break;
        case kWM8960_ModuleLineOut:
            WM8960_ModifyReg(handle, WM8960_POWER2, WM8960_POWER2_LOUT1_MASK,
                             ((uint16_t)isEnabled << WM8960_POWER2_LOUT1_SHIFT));
            WM8960_ModifyReg(handle, WM8960_POWER2, WM8960_POWER2_ROUT1_MASK,
                             ((uint16_t)isEnabled << WM8960_POWER2_ROUT1_SHIFT));
            break;
        case kWM8960_ModuleSpeaker:
            WM8960_ModifyReg(handle, WM8960_POWER2, WM8960_POWER2_SPKL_MASK,
                             ((uint16_t)isEnabled << WM8960_POWER2_SPKL_SHIFT));
            WM8960_ModifyReg(handle, WM8960_POWER2, WM8960_POWER2_SPKR_MASK,
                             ((uint16_t)isEnabled << WM8960_POWER2_SPKR_SHIFT));
            WM8960_WriteReg(handle, WM8960_CLASSD1, 0xF7);
            break;
        default:
            ret = kStatus_InvalidArgument;
            break;
    }
    return ret;
}

status_t WM8960_SetDataRoute(wm8960_handle_t *handle, wm8960_route_t route)
{
    status_t ret = kStatus_Success;
    switch (route)
    {
        case kWM8960_RouteBypass:
            /* Bypass means from line-in to HP*/
            /*
             * Left LINPUT3 to left output mixer, LINPUT3 left output mixer volume = 0dB
             */
            WM8960_WriteReg(handle, WM8960_LOUTMIX, 0x80);

            /*
             * Right RINPUT3 to right output mixer, RINPUT3 right output mixer volume = 0dB
             */
            WM8960_WriteReg(handle, WM8960_ROUTMIX, 0x80);
            break;
        case kWM8960_RoutePlayback:
            /* Data route I2S_IN-> DAC-> HP */
            /*
             * Left DAC to left output mixer, LINPUT3 left output mixer volume = 0dB
             */
            WM8960_WriteReg(handle, WM8960_LOUTMIX, 0x100);

            /*
             * Right DAC to right output mixer, RINPUT3 right output mixer volume = 0dB
             */
            WM8960_WriteReg(handle, WM8960_ROUTMIX, 0x100);
            break;
        case kWM8960_RoutePlaybackandRecord:
            /* I2S IN->DAC->HP  LINE_IN->ADC->I2S_OUT */
            /*
             * Left and right input boost, LIN3BOOST and RIN3BOOST = 0dB
             */
            WM8960_WriteReg(handle, WM8960_INBMIX1, 0x50);
            WM8960_WriteReg(handle, WM8960_INBMIX2, 0x50);
            /*
             * Left DAC to left output mixer, LINPUT3 left output mixer volume = 0dB
             */
            WM8960_WriteReg(handle, WM8960_LOUTMIX, 0x100);

            /*
             * Right DAC to right output mixer, RINPUT3 right output mixer volume = 0dB
             */
            WM8960_WriteReg(handle, WM8960_ROUTMIX, 0x100);
            break;
        case kWM8960_RoutePlaybackwithDAP:
            /* I2S_IN->DAP->DAC->HP */
            break;
        case kWM8960_RoutePlaybackwithDAPandRecord:
            /* I2S_IN->DAP->DAC->HP,  LINE_IN->ADC->I2S_OUT */
            break;
        case kWM8960_RouteRecord:
            /* LINE_IN->ADC->I2S_OUT */
            /*
             * Left and right input boost, LIN3BOOST and RIN3BOOST = 0dB
             */
            WM8960_WriteReg(handle, WM8960_INBMIX1, 0x50);
            WM8960_WriteReg(handle, WM8960_INBMIX2, 0x50);
            break;
        default:
            ret = kStatus_InvalidArgument;
            break;
    }
    return ret;
}

status_t WM8960_SetProtocol(wm8960_handle_t *handle, wm8960_protocol_t protocol)
{
    status_t ret = kStatus_Success;
    switch (protocol)
    {
        case kWM8960_BusI2S:
            WM8960_ModifyReg(handle, WM8960_IFACE1, WM8960_IFACE1_FORMAT_MASK,
                             WM8960_IFACE1_FORMAT(WM8960_IFACE1_FORMAT_I2S));
            break;
        case kWM8960_BusLeftJustified:
            WM8960_ModifyReg(handle, WM8960_IFACE1, WM8960_IFACE1_FORMAT_MASK,
                             WM8960_IFACE1_FORMAT(WM8960_IFACE1_FORMAT_LJ));
            break;
        case kWM8960_BusRightJustified:
            WM8960_ModifyReg(handle, WM8960_IFACE1, WM8960_IFACE1_FORMAT_MASK,
                             WM8960_IFACE1_FORMAT(WM8960_IFACE1_FORMAT_RJ));
            break;
        case kWM8960_BusPCMA:
            WM8960_ModifyReg(handle, WM8960_IFACE1, WM8960_IFACE1_FORMAT_MASK,
                             WM8960_IFACE1_FORMAT(WM8960_IFACE1_FORMAT_DSP));
            WM8960_ModifyReg(handle, WM8960_IFACE1, WM8960_IFACE1_LRP_MASK, WM8960_IFACE1_LRP(WM8960_IFACE1_DSP_MODEA));
            break;
        case kWM8960_BusPCMB:
            WM8960_ModifyReg(handle, WM8960_IFACE1, WM8960_IFACE1_FORMAT_MASK,
                             WM8960_IFACE1_FORMAT(WM8960_IFACE1_FORMAT_DSP));
            WM8960_ModifyReg(handle, WM8960_IFACE1, WM8960_IFACE1_LRP_MASK, WM8960_IFACE1_LRP(WM8960_IFACE1_DSP_MODEB));
            break;
        default:
            ret = kStatus_InvalidArgument;
            break;
    }
    WM8960_ModifyReg(handle, WM8960_IFACE1, WM8960_IFACE1_WL_MASK, WM8960_IFACE1_WL(WM8960_IFACE1_WL_32BITS));
    return ret;
}

status_t WM8960_SetVolume(wm8960_handle_t *handle, wm8960_module_t module, uint32_t volume)
{
    uint16_t vol = 0;
    status_t ret = kStatus_Success;
    switch (module)
    {
        case kWM8960_ModuleADC:
            vol = 0x100 | volume;
            ret = WM8960_WriteReg(handle, WM8960_LADC, vol);
            ret = WM8960_WriteReg(handle, WM8960_RADC, vol);
            break;
        case kWM8960_ModuleDAC:
            vol = 0x100 | volume;
            ret = WM8960_WriteReg(handle, WM8960_LDAC, vol);
            ret = WM8960_WriteReg(handle, WM8960_RDAC, vol);
            break;
        case kWM8960_ModuleHP:
            vol = 0x100 | volume;
            ret = WM8960_WriteReg(handle, WM8960_LOUT1, vol);
            ret = WM8960_WriteReg(handle, WM8960_ROUT1, vol);
            break;
        case kWM8960_ModuleLineIn:
            vol = 0x100 | volume;
            ret = WM8960_WriteReg(handle, WM8960_LINVOL, vol);
            ret = WM8960_WriteReg(handle, WM8960_RINVOL, vol);
            break;
        case kWM8960_ModuleSpeaker:
            vol = 0x100 | volume;
            ret = WM8960_WriteReg(handle, WM8960_LOUT2, vol);
            ret = WM8960_WriteReg(handle, WM8960_ROUT2, vol);
            break;
        default:
            ret = kStatus_InvalidArgument;
            break;
    }
    return ret;
}

uint32_t WM8960_GetVolume(wm8960_handle_t *handle, wm8960_module_t module)
{
    uint16_t vol = 0;
    switch (module)
    {
        case kWM8960_ModuleADC:
            WM8960_ReadReg(WM8960_LADC, &vol);
            vol &= 0xFF;
            break;
        case kWM8960_ModuleDAC:
            WM8960_ReadReg(WM8960_LDAC, &vol);
            vol &= 0xFF;
            break;
        case kWM8960_ModuleHP:
            WM8960_ReadReg(WM8960_LOUT1, &vol);
            vol &= 0x7F;
            break;
        case kWM8960_ModuleLineOut:
            WM8960_ReadReg(WM8960_LINVOL, &vol);
            vol &= 0x3F;
            break;
        default:
            vol = 0;
            break;
    }
    return vol;
}

status_t WM8960_SetMute(wm8960_handle_t *handle, wm8960_module_t module, bool isEnabled)
{
    status_t ret = kStatus_Success;
    switch (module)
    {
        case kWM8960_ModuleADC:
            /*
             * Digital Mute
             */
            if (isEnabled)
            {
                ret = WM8960_WriteReg(handle, WM8960_LADC, 0x100);
                ret = WM8960_WriteReg(handle, WM8960_RADC, 0x100);
            }
            else
            {
                ret = WM8960_WriteReg(handle, WM8960_LADC, 0x1C3);
                ret = WM8960_WriteReg(handle, WM8960_RADC, 0x1C3);
            }
            break;
        case kWM8960_ModuleDAC:
            /*
             * Digital mute
             */
            if (isEnabled)
            {
                ret = WM8960_WriteReg(handle, WM8960_LDAC, 0x100);
                ret = WM8960_WriteReg(handle, WM8960_RDAC, 0x100);
            }
            else
            {
                ret = WM8960_WriteReg(handle, WM8960_LDAC, 0x1FF);
                ret = WM8960_WriteReg(handle, WM8960_RDAC, 0x1FF);
            }
            break;
        case kWM8960_ModuleHP:
            /*
             * Analog mute
             */
            if (isEnabled)
            {
                ret = WM8960_WriteReg(handle, WM8960_LOUT1, 0x100);
                ret = WM8960_WriteReg(handle, WM8960_ROUT1, 0x100);
            }
            else
            {
                ret = WM8960_WriteReg(handle, WM8960_LOUT1, 0x179);
                ret = WM8960_WriteReg(handle, WM8960_ROUT1, 0x179);
            }
            break;
        case kWM8960_ModuleLineOut:
            break;
        default:
            ret = kStatus_InvalidArgument;
            break;
    }
    return ret;
}

status_t WM8960_ConfigDataFormat(wm8960_handle_t *handle, uint32_t mclk, uint32_t sample_rate, uint8_t bits)
{
    status_t retval = kStatus_Success;

    switch (sample_rate)
    {
        case 8000:
            retval = WM8960_WriteReg(handle, WM8960_CLOCK1, 0x1B0);
            break;
        case 11025:
            retval = WM8960_WriteReg(handle, WM8960_CLOCK1, 0xD8);
            break;
        case 12000:
            retval = WM8960_WriteReg(handle, WM8960_CLOCK1, 0x120);
            break;
        case 16000:
            retval = WM8960_WriteReg(handle, WM8960_CLOCK1, 0xD8);
            break;
        case 22050:
            retval = WM8960_WriteReg(handle, WM8960_CLOCK1, 0xD8);
            break;
        case 24000:
            retval = WM8960_WriteReg(handle, WM8960_CLOCK1, 0x90);
            break;
        case 32000:
            retval = WM8960_WriteReg(handle, WM8960_CLOCK1, 0x48);
            break;
        case 44100:
            retval = WM8960_WriteReg(handle, WM8960_CLOCK1, 0xD8);
            break;
        case 48000:
            retval = WM8960_WriteReg(handle, WM8960_CLOCK1, 0x00);
            break;
        default:
            retval = kStatus_InvalidArgument;
            break;
    }

    /*
     * Slave mode (MS = 0), LRP = 0, 32bit WL, left justified (FORMAT[1:0]=0b01)
     */
    switch (bits)
    {
        case 16:
            retval = WM8960_ModifyReg(handle, WM8960_IFACE1, WM8960_IFACE1_WL_MASK,
                                      WM8960_IFACE1_WL(WM8960_IFACE1_WL_16BITS));
            break;
        case 20:
            retval = WM8960_ModifyReg(handle, WM8960_IFACE1, WM8960_IFACE1_WL_MASK,
                                      WM8960_IFACE1_WL(WM8960_IFACE1_WL_20BITS));
            break;
        case 24:
            retval = WM8960_ModifyReg(handle, WM8960_IFACE1, WM8960_IFACE1_WL_MASK,
                                      WM8960_IFACE1_WL(WM8960_IFACE1_WL_24BITS));
            break;
        case 32:
            retval = WM8960_ModifyReg(handle, WM8960_IFACE1, WM8960_IFACE1_WL_MASK,
                                      WM8960_IFACE1_WL(WM8960_IFACE1_WL_32BITS));
            break;
        default:
            retval = kStatus_InvalidArgument;
            break;
    }

    return retval;
}

status_t WM8960_SetJackDetect(wm8960_handle_t *handle, bool isEnabled)
{
    uint8_t retval = 0;
    uint16_t val = 0;

    WM8960_ReadReg(WM8960_ADDCTL2, &val);

    if (isEnabled)
    {
        val |= 0x40U;
    }
    else
    {
        val &= 0xCF;
    }

    retval = WM8960_WriteReg(handle, WM8960_ADDCTL2, val);

    return retval;
}

status_t WM8960_WriteReg(wm8960_handle_t *handle, uint8_t reg, uint16_t val)
{
    uint8_t cmd, buff;
    uint8_t retval = 0;

    /* The register address */
    cmd = (reg << 1) | ((val >> 8U) & 0x0001U);
    /* Data */
    buff = val & 0xFF;

    /* Copy data to cache */
    reg_cache[reg] = val;
#if defined(USE_MEDIA_STACK_CONTROL_INTERFACE)
    uint8_t data[2];
    data[0] = cmd;
    data[1] = buff;
    retval = handle->send_data(handle->instance, data, 2);
#elif defined(FSL_FEATURE_SOC_LPI2C_COUNT) && (FSL_FEATURE_SOC_LPI2C_COUNT)
    uint8_t data[2];
    data[0] = cmd;
    data[1] = buff;
    retval = LPI2C_MasterStart(handle->base, WM8960_I2C_ADDR, kLPI2C_Write);
    retval = LPI2C_MasterSend(handle->base, data, 2);
    retval = LPI2C_MasterStop(handle->base);
#else
    /* Config the I2C xfer */
    handle->xfer.direction = kI2C_Write;
    handle->xfer.subaddress = cmd;
    handle->xfer.subaddressSize = 1U;
    handle->xfer.data = &buff;
    handle->xfer.dataSize = 1U;

    retval = I2C_MasterTransferBlocking(handle->base, &handle->xfer);
#endif

    if (retval != kStatus_Success)
    {
        return kStatus_Fail;
    }
    return kStatus_Success;
}

status_t WM8960_ReadReg(uint8_t reg, uint16_t *val)
{
    if (reg >= WM8960_CACHEREGNUM)
    {
        return kStatus_InvalidArgument;
    }

    *val = reg_cache[reg];

    return kStatus_Success;
}

status_t WM8960_ModifyReg(wm8960_handle_t *handle, uint8_t reg, uint16_t mask, uint16_t val)
{
    uint8_t retval = 0;
    uint16_t reg_val = 0;
    retval = WM8960_ReadReg(reg, &reg_val);
    if (retval != kStatus_Success)
    {
        return kStatus_Fail;
    }
    reg_val &= (uint16_t)~mask;
    reg_val |= val;
    retval = WM8960_WriteReg(handle, reg, reg_val);
    if (retval != kStatus_Success)
    {
        return kStatus_Fail;
    }
    return kStatus_Success;
}
