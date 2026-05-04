/*
 * Copyright 2019-2021, 2023-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_common.h"
#include "fsl_iomuxc.h"
#include "pin_mux.h"
#include "board.h"
#include "genavb_sdk.h"

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitBootPins
 * Description   : Calls initialization functions.
 *
 * END ****************************************************************************************************************/
void BOARD_InitBootPins(void) {
    BOARD_InitUARTPins();
    BOARD_InitSEMCPins();
    BOARD_InitENETPins();
    BOARD_InitI2CPins();
    BOARD_InitSAIPins();
    BOARD_InitPWMPins();
    BOARD_InitENCPins();
    BOARD_InitFlexSPIPins();
    BOARD_InitADCPins();
#ifdef CONFIG_APP_PPS
    BOARD_InitPinsPPS();
#endif
#ifdef CONFIG_APP_SERIAL
    BOARD_InitLPUARTPins();
#endif
}

void BOARD_InitPinsPPS(void)
{
    IOMUXC_SetPinMux(
        IOMUXC_GPIO_AD_22_ENET_QOS_1588_EVENT3_OUT,
        1U);

    IOMUXC_SetPinConfig(
        IOMUXC_GPIO_AD_22_ENET_QOS_1588_EVENT3_OUT,
        0x6u);
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitFlexSPIPins, assigned for the Cortex-M7F core.
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 * END ****************************************************************************************************************/
void BOARD_InitFlexSPIPins(void) {
  CLOCK_EnableClock(kCLOCK_Iomuxc);

  IOMUXC_SetPinMux(
      IOMUXC_GPIO_SD_B2_05_FLEXSPI1_A_DQS,    /* GPIO_SD_B2_05 is configured as FLEXSPI1_A_DQS */
      1U);                                    /* Software Input On Field: Force input path of pad GPIO_SD_B2_05 */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_SD_B2_06_FLEXSPI1_A_SS0_B,  /* GPIO_SD_B2_06 is configured as FLEXSPI1_A_SS0_B */
      1U);                                    /* Software Input On Field: Force input path of pad GPIO_SD_B2_06 */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_SD_B2_07_FLEXSPI1_A_SCLK,   /* GPIO_SD_B2_07 is configured as FLEXSPI1_A_SCLK */
      1U);                                    /* Software Input On Field: Force input path of pad GPIO_SD_B2_07 */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_SD_B2_08_FLEXSPI1_A_DATA00,  /* GPIO_SD_B2_08 is configured as FLEXSPI1_A_DATA00 */
      1U);                                    /* Software Input On Field: Force input path of pad GPIO_SD_B2_08 */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_SD_B2_09_FLEXSPI1_A_DATA01,  /* GPIO_SD_B2_09 is configured as FLEXSPI1_A_DATA01 */
      1U);                                    /* Software Input On Field: Force input path of pad GPIO_SD_B2_09 */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_SD_B2_10_FLEXSPI1_A_DATA02,  /* GPIO_SD_B2_10 is configured as FLEXSPI1_A_DATA02 */
      1U);                                    /* Software Input On Field: Force input path of pad GPIO_SD_B2_10 */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_SD_B2_11_FLEXSPI1_A_DATA03,  /* GPIO_SD_B2_11 is configured as FLEXSPI1_A_DATA03 */
      1U);                                    /* Software Input On Field: Force input path of pad GPIO_SD_B2_11 */

  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_SD_B2_05_FLEXSPI1_A_DQS,    /* GPIO_SD_B2_05 PAD functional properties : */
      0x0AU);                                 /* PDRV Field: normal driver
                                                 Pull Down Pull Up Field: PD
                                                 Open Drain Field: Disabled */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_SD_B2_06_FLEXSPI1_A_SS0_B,  /* GPIO_SD_B2_06 PAD functional properties : */
      0x0AU);                                 /* PDRV Field: normal driver
                                                 Pull Down Pull Up Field: PD
                                                 Open Drain Field: Disabled */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_SD_B2_07_FLEXSPI1_A_SCLK,   /* GPIO_SD_B2_07 PAD functional properties : */
      0x0AU);                                 /* PDRV Field: normal driver
                                                 Pull Down Pull Up Field: PD
                                                 Open Drain Field: Disabled */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_SD_B2_08_FLEXSPI1_A_DATA00,  /* GPIO_SD_B2_08 PAD functional properties : */
      0x0AU);                                 /* PDRV Field: normal driver
                                                 Pull Down Pull Up Field: PD
                                                 Open Drain Field: Disabled */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_SD_B2_09_FLEXSPI1_A_DATA01,  /* GPIO_SD_B2_09 PAD functional properties : */
      0x0AU);                                 /* PDRV Field: normal driver
                                                 Pull Down Pull Up Field: PD
                                                 Open Drain Field: Disabled */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_SD_B2_10_FLEXSPI1_A_DATA02,  /* GPIO_SD_B2_10 PAD functional properties : */
      0x0AU);                                 /* PDRV Field: normal driver
                                                 Pull Down Pull Up Field: PD
                                                 Open Drain Field: Disabled */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_SD_B2_11_FLEXSPI1_A_DATA03,  /* GPIO_SD_B2_11 PAD functional properties : */
      0x0AU);                                 /* PDRV Field: normal driver
                                                 Pull Down Pull Up Field: PD
                                                 Open Drain Field: Disabled */
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitPWMPins, assigned for the Cortex-M7F core.
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 * END ****************************************************************************************************************/
void BOARD_InitPWMPins(void) {
  CLOCK_EnableClock(kCLOCK_Iomuxc);           /* LPCG on: LPCG is ON. */

  IOMUXC_SetPinMux(
      IOMUXC_GPIO_AD_00_FLEXPWM1_PWM0_A,
      0U);
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_AD_01_FLEXPWM1_PWM0_B,
      0U);
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_AD_02_FLEXPWM1_PWM1_A,
      0U);
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_AD_03_FLEXPWM1_PWM1_B,
      0U);
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_AD_04_FLEXPWM1_PWM2_A,
      0U);
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_AD_05_FLEXPWM1_PWM2_B,
      0U);
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_AD_00_FLEXPWM1_PWM0_A,
      0U);

  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_AD_01_FLEXPWM1_PWM0_B,
      0U);

  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_AD_02_FLEXPWM1_PWM1_A,
      0U);

  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_AD_03_FLEXPWM1_PWM1_B,
      0U);

  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_AD_04_FLEXPWM1_PWM2_A,
      0U);

  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_AD_05_FLEXPWM1_PWM2_B,
      0U);

}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitENCPins, assigned for the Cortex-M7F core.
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 * END ****************************************************************************************************************/
void BOARD_InitENCPins(void) {
  CLOCK_EnableClock(kCLOCK_Iomuxc);

  IOMUXC_SetPinMux(
      IOMUXC_GPIO_DISP_B2_10_XBAR1_INOUT38,
      0U);
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_DISP_B2_11_XBAR1_INOUT39,
      0U);
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_DISP_B2_12_XBAR1_INOUT40,
      0U);
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_DISP_B2_10_XBAR1_INOUT38,
      0U);

  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_DISP_B2_11_XBAR1_INOUT39,
      0U);

  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_DISP_B2_12_XBAR1_INOUT40,
      0U);

  IOMUXC_GPR->GPR21 = (IOMUXC_GPR->GPR21 & ~(IOMUXC_GPR_GPR21_IOMUXC_XBAR_DIR_SEL_38_MASK |
                                             IOMUXC_GPR_GPR21_IOMUXC_XBAR_DIR_SEL_39_MASK |
                                             IOMUXC_GPR_GPR21_IOMUXC_XBAR_DIR_SEL_40_MASK))|
                      IOMUXC_GPR_GPR21_IOMUXC_XBAR_DIR_SEL_38(0x0) |
                      IOMUXC_GPR_GPR21_IOMUXC_XBAR_DIR_SEL_39(0x0) |
                      IOMUXC_GPR_GPR21_IOMUXC_XBAR_DIR_SEL_40(0x0);
}

void BOARD_InitI2CPins(void)
{
  CLOCK_EnableClock(kCLOCK_Iomuxc);

  IOMUXC_SetPinMux(
      IOMUXC_GPIO_LPSR_05_LPI2C5_SCL,
      1U);
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_LPSR_04_LPI2C5_SDA,
      1U);
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_LPSR_05_LPI2C5_SCL,
      0xD8B0u);

  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_LPSR_04_LPI2C5_SDA,
      0xD8B0u);
}

void BOARD_InitSAIPins(void)
{
  CLOCK_EnableClock(kCLOCK_Iomuxc);

  IOMUXC_SetPinMux(
      IOMUXC_GPIO_AD_17_SAI1_MCLK,
      1U);
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_AD_21_SAI1_TX_DATA00,
      1U);
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_AD_20_SAI1_RX_DATA00,
      1U);
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_AD_22_SAI1_TX_BCLK,
      1U);
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_AD_23_SAI1_TX_SYNC,
      1U);
  
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_AD_17_SAI1_MCLK,
      0x6u);

  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_AD_21_SAI1_TX_DATA00,
      0x6u);

  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_AD_20_SAI1_RX_DATA00,
      0xFu);

  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_AD_22_SAI1_TX_BCLK,
      0x6u);

  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_AD_23_SAI1_TX_SYNC,
      0x6u);
}

void BOARD_InitENETPins(void)
{
  CLOCK_EnableClock(kCLOCK_Iomuxc);

  IOMUXC_SetPinMux(
      IOMUXC_GPIO_LPSR_12_GPIO12_IO12,
      0U);
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_DISP_B2_06_ENET_RX_DATA00,
      1U);
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_DISP_B2_07_ENET_RX_DATA01,
      1U);
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_DISP_B2_08_ENET_RX_EN,
      0U);
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_DISP_B2_02_ENET_TX_DATA00,
      0U);
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_DISP_B2_03_ENET_TX_DATA01,
      0U);
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_DISP_B2_04_ENET_TX_EN,
      0U);
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_DISP_B2_05_ENET_REF_CLK,
      1U);
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_DISP_B2_09_ENET_RX_ER,
      0U);
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_AD_32_ENET_MDC,
      0U);
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_AD_33_ENET_MDIO,
      0U);
#if BOARD_USE_ENET_QOS
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_DISP_B1_00_ENET_QOS_RX_EN,  /* GPIO_DISP_B1_00 is configured as ENET_QOS_RX_EN */
      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_DISP_B1_01_ENET_QOS_RX_CLK,  /* GPIO_DISP_B1_01 is configured as ENET_QOS_RX_CLK */
      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_DISP_B1_02_ENET_QOS_RX_DATA00,  /* GPIO_DISP_B1_02 is configured as ENET_QOS_RX_DATA00 */
      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_DISP_B1_03_ENET_QOS_RX_DATA01,  /* GPIO_DISP_B1_03 is configured as ENET_QOS_RX_DATA01 */
      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_DISP_B1_04_ENET_QOS_RX_DATA02,  /* GPIO_DISP_B1_04 is configured as ENET_QOS_RX_DATA02 */
      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_DISP_B1_05_ENET_QOS_RX_DATA03,  /* GPIO_DISP_B1_05 is configured as ENET_QOS_RX_DATA03 */
      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_DISP_B1_06_ENET_QOS_TX_DATA03,  /* GPIO_DISP_B1_06 is configured as ENET_QOS_TX_DATA03 */
      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_DISP_B1_07_ENET_QOS_TX_DATA02,  /* GPIO_DISP_B1_07 is configured as ENET_QOS_TX_DATA02 */
      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_DISP_B1_08_ENET_QOS_TX_DATA01,  /* GPIO_DISP_B1_08 is configured as ENET_QOS_TX_DATA01 */
      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_DISP_B1_09_ENET_QOS_TX_DATA00,  /* GPIO_DISP_B1_09 is configured as ENET_QOS_TX_DATA00 */
      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_DISP_B1_10_ENET_QOS_TX_EN,  /* GPIO_DISP_B1_10 is configured as ENET_QOS_TX_EN */
      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_DISP_B1_11_ENET_QOS_TX_CLK,  /* GPIO_DISP_B1_11 is configured as ENET_QOS_TX_CLK */
      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B2_19_ENET_QOS_MDC,     /* GPIO_EMC_B2_19 is configured as ENET_QOS_MDC */
      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B2_20_ENET_QOS_MDIO,    /* GPIO_EMC_B2_20 is configured as ENET_QOS_MDIO */
      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
#else /* BOARD_USE_ENET_QOS */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_DISP_B1_00_ENET_1G_RX_EN,   /* GPIO_DISP_B1_00 is configured as ENET_1G_RX_EN */
      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_DISP_B1_01_ENET_1G_RX_CLK,  /* GPIO_DISP_B1_01 is configured as ENET_1G_RX_CLK */
      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_DISP_B1_02_ENET_1G_RX_DATA00,  /* GPIO_DISP_B1_02 is configured as ENET_1G_RX_DATA00 */
      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_DISP_B1_03_ENET_1G_RX_DATA01,  /* GPIO_DISP_B1_03 is configured as ENET_1G_RX_DATA01 */
      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_DISP_B1_04_ENET_1G_RX_DATA02,  /* GPIO_DISP_B1_04 is configured as ENET_1G_RX_DATA02 */
      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_DISP_B1_05_ENET_1G_RX_DATA03,  /* GPIO_DISP_B1_05 is configured as ENET_1G_RX_DATA03 */
      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_DISP_B1_06_ENET_1G_TX_DATA03,  /* GPIO_DISP_B1_06 is configured as ENET_1G_TX_DATA03 */
      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_DISP_B1_07_ENET_1G_TX_DATA02,  /* GPIO_DISP_B1_07 is configured as ENET_1G_TX_DATA02 */
      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_DISP_B1_08_ENET_1G_TX_DATA01,  /* GPIO_DISP_B1_08 is configured as ENET_1G_TX_DATA01 */
      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_DISP_B1_09_ENET_1G_TX_DATA00,  /* GPIO_DISP_B1_09 is configured as ENET_1G_TX_DATA00 */
      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_DISP_B1_10_ENET_1G_TX_EN,   /* GPIO_DISP_B1_10 is configured as ENET_1G_TX_EN */
      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_DISP_B1_11_ENET_1G_TX_CLK_IO,  /* GPIO_DISP_B1_11 is configured as ENET_1G_TX_CLK_IO */
      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B2_19_ENET_1G_MDC,      /* GPIO_EMC_B2_19 is configured as ENET_1G_MDC */
      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B2_20_ENET_1G_MDIO,     /* GPIO_EMC_B2_20 is configured as ENET_1G_MDIO */
      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
#endif /* BOARD_USE_ENET_QOS */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_DISP_B2_13_GPIO11_IO14,     /* GPIO_DISP_B2_13 is configured as GPIO11_IO14 */
      0U);
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_LPSR_12_GPIO12_IO12,
      0x0Eu);

  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_DISP_B2_06_ENET_RX_DATA00,
      0x06u);

  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_DISP_B2_07_ENET_RX_DATA01,
      0x06u);

  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_DISP_B2_08_ENET_RX_EN,
      0x06u);

  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_DISP_B2_02_ENET_TX_DATA00,
      0x02u);

  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_DISP_B2_03_ENET_TX_DATA01,
      0x02u);

  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_DISP_B2_04_ENET_TX_EN,
      0x02u);

  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_DISP_B2_05_ENET_REF_CLK,
      0x3u);

  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_DISP_B2_09_ENET_RX_ER,
      0x06u);

  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_AD_32_ENET_MDC,
      0x06u);

  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_AD_33_ENET_MDIO,
      0x06u);

#if BOARD_USE_ENET_QOS
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_DISP_B1_01_ENET_QOS_RX_CLK,  /* GPIO_DISP_B1_01 PAD functional properties : */
      0x08U);                                 /* PDRV Field: high driver
                                                 Pull Down Pull Up Field: PD
                                                 Open Drain Field: Disabled */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_DISP_B1_02_ENET_QOS_RX_DATA00,  /* GPIO_DISP_B1_02 PAD functional properties : */
      0x08U);                                 /* PDRV Field: high driver
                                                 Pull Down Pull Up Field: PD
                                                 Open Drain Field: Disabled */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_DISP_B1_03_ENET_QOS_RX_DATA01,  /* GPIO_DISP_B1_03 PAD functional properties : */
      0x08U);                                 /* PDRV Field: high driver
                                                 Pull Down Pull Up Field: PD
                                                 Open Drain Field: Disabled */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_DISP_B1_04_ENET_QOS_RX_DATA02,  /* GPIO_DISP_B1_04 PAD functional properties : */
      0x08U);                                 /* PDRV Field: high driver
                                                 Pull Down Pull Up Field: PD
                                                 Open Drain Field: Disabled */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_DISP_B1_05_ENET_QOS_RX_DATA03,  /* GPIO_DISP_B1_05 PAD functional properties : */
      0x08U);                                 /* PDRV Field: high driver
                                                 Pull Down Pull Up Field: PD
                                                 Open Drain Field: Disabled */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_DISP_B1_06_ENET_QOS_TX_DATA03,  /* GPIO_DISP_B1_06 PAD functional properties : */
      0x0CU);                                 /* PDRV Field: high driver
                                                 Pull Down Pull Up Field: No Pull
                                                 Open Drain Field: Disabled */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_DISP_B1_07_ENET_QOS_TX_DATA02,  /* GPIO_DISP_B1_07 PAD functional properties : */
      0x0CU);                                 /* PDRV Field: high driver
                                                 Pull Down Pull Up Field: No Pull
                                                 Open Drain Field: Disabled */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_DISP_B1_08_ENET_QOS_TX_DATA01,  /* GPIO_DISP_B1_08 PAD functional properties : */
      0x0CU);                                 /* PDRV Field: high driver
                                                 Pull Down Pull Up Field: No Pull
                                                 Open Drain Field: Disabled */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_DISP_B1_09_ENET_QOS_TX_DATA00,  /* GPIO_DISP_B1_09 PAD functional properties : */
      0x0CU);                                 /* PDRV Field: high driver
                                                 Pull Down Pull Up Field: No Pull
                                                 Open Drain Field: Disabled */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_DISP_B1_10_ENET_QOS_TX_EN,  /* GPIO_DISP_B1_10 PAD functional properties : */
      0x0CU);                                 /* PDRV Field: high driver
                                                 Pull Down Pull Up Field: No Pull
                                                 Open Drain Field: Disabled */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_DISP_B1_11_ENET_QOS_TX_CLK,  /* GPIO_DISP_B1_11 PAD functional properties : */
      0x0CU);                                 /* PDRV Field: high driver
                                                 Pull Down Pull Up Field: No Pull
                                                 Open Drain Field: Disabled */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B2_19_ENET_QOS_MDC,     /* GPIO_EMC_B2_19 PAD functional properties : */
      0x08U);                                 /* PDRV Field: high driver
                                                 Pull Down Pull Up Field: PD
                                                 Open Drain Field: Disabled */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B2_20_ENET_QOS_MDIO,    /* GPIO_EMC_B2_20 PAD functional properties : */
      0x08U);                                 /* PDRV Field: high driver
                                                 Pull Down Pull Up Field: PD
                                                 Open Drain Field: Disabled */
#else /* BOARD_USE_ENET_QOS */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_DISP_B1_00_ENET_1G_RX_EN,   /* GPIO_DISP_B1_00 PAD functional properties : */
      0x08U);                                 /* PDRV Field: high driver
                                                 Pull Down Pull Up Field: PD
                                                 Open Drain Field: Disabled */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_DISP_B1_01_ENET_1G_RX_CLK,  /* GPIO_DISP_B1_01 PAD functional properties : */
      0x08U);                                 /* PDRV Field: high driver
                                                 Pull Down Pull Up Field: PD
                                                 Open Drain Field: Disabled */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_DISP_B1_02_ENET_1G_RX_DATA00,  /* GPIO_DISP_B1_02 PAD functional properties : */
      0x08U);                                 /* PDRV Field: high driver
                                                 Pull Down Pull Up Field: PD
                                                 Open Drain Field: Disabled */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_DISP_B1_03_ENET_1G_RX_DATA01,  /* GPIO_DISP_B1_03 PAD functional properties : */
      0x08U);                                 /* PDRV Field: high driver
                                                 Pull Down Pull Up Field: PD
                                                 Open Drain Field: Disabled */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_DISP_B1_04_ENET_1G_RX_DATA02,  /* GPIO_DISP_B1_04 PAD functional properties : */
      0x08U);                                 /* PDRV Field: high driver
                                                 Pull Down Pull Up Field: PD
                                                 Open Drain Field: Disabled */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_DISP_B1_05_ENET_1G_RX_DATA03,  /* GPIO_DISP_B1_05 PAD functional properties : */
      0x08U);                                 /* PDRV Field: high driver
                                                 Pull Down Pull Up Field: PD
                                                 Open Drain Field: Disabled */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_DISP_B1_06_ENET_1G_TX_DATA03,  /* GPIO_DISP_B1_06 PAD functional properties : */
      0x0CU);                                 /* PDRV Field: high driver
                                                 Pull Down Pull Up Field: No Pull
                                                 Open Drain Field: Disabled */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_DISP_B1_07_ENET_1G_TX_DATA02,  /* GPIO_DISP_B1_07 PAD functional properties : */
      0x0CU);                                 /* PDRV Field: high driver
                                                 Pull Down Pull Up Field: No Pull
                                                 Open Drain Field: Disabled */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_DISP_B1_08_ENET_1G_TX_DATA01,  /* GPIO_DISP_B1_08 PAD functional properties : */
      0x0CU);                                 /* PDRV Field: high driver
                                                 Pull Down Pull Up Field: No Pull
                                                 Open Drain Field: Disabled */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_DISP_B1_09_ENET_1G_TX_DATA00,  /* GPIO_DISP_B1_09 PAD functional properties : */
      0x0CU);                                 /* PDRV Field: high driver
                                                 Pull Down Pull Up Field: No Pull
                                                 Open Drain Field: Disabled */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_DISP_B1_10_ENET_1G_TX_EN,   /* GPIO_DISP_B1_10 PAD functional properties : */
      0x0CU);                                 /* PDRV Field: high driver
                                                 Pull Down Pull Up Field: No Pull
                                                 Open Drain Field: Disabled */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_DISP_B1_11_ENET_1G_TX_CLK_IO,  /* GPIO_DISP_B1_11 PAD functional properties : */
      0x0CU);                                 /* PDRV Field: high driver
                                                 Pull Down Pull Up Field: No Pull
                                                 Open Drain Field: Disabled */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B2_19_ENET_1G_MDC,     /* GPIO_EMC_B2_19 PAD functional properties : */
      0x08U);                                 /* PDRV Field: high driver
                                                 Pull Down Pull Up Field: PD
                                                 Open Drain Field: Disabled */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B2_20_ENET_1G_MDIO,    /* GPIO_EMC_B2_20 PAD functional properties : */
      0x08U);                                 /* PDRV Field: high driver
                                                 Pull Down Pull Up Field: PD
                                                 Open Drain Field: Disabled */
#endif /* BOARD_USE_ENET_QOS */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_DISP_B2_13_GPIO11_IO14,     /* GPIO_DISP_B2_13 PAD functional properties : */
      0x06U);                                 /* Slew Rate Field: Slow Slew Rate
                                                 Drive Strength Field: high driver
                                                 Pull / Keep Select Field: Pull Enable
                                                 Pull Up / Down Config. Field: Weak pull down
                                                 Open Drain Field: Disabled */
}

void BOARD_InitSEMCPins(void)
{
  CLOCK_EnableClock(kCLOCK_Iomuxc);

  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B1_00_SEMC_DATA00, /* GPIO_EMC_B1_00 is configured as SEMC_DATA00 */
      0U);                               /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B1_01_SEMC_DATA01, /* GPIO_EMC_B1_01 is configured as SEMC_DATA01 */
      0U);                               /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B1_02_SEMC_DATA02, /* GPIO_EMC_B1_02 is configured as SEMC_DATA02 */
      0U);                               /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B1_03_SEMC_DATA03, /* GPIO_EMC_B1_03 is configured as SEMC_DATA03 */
      0U);                               /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B1_04_SEMC_DATA04, /* GPIO_EMC_B1_04 is configured as SEMC_DATA04 */
      0U);                               /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B1_05_SEMC_DATA05, /* GPIO_EMC_B1_05 is configured as SEMC_DATA05 */
      0U);                               /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B1_06_SEMC_DATA06, /* GPIO_EMC_B1_06 is configured as SEMC_DATA06 */
      0U);                               /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B1_07_SEMC_DATA07, /* GPIO_EMC_B1_07 is configured as SEMC_DATA07 */
      0U);                               /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B1_08_SEMC_DM00, /* GPIO_EMC_B1_08 is configured as SEMC_DM00 */
      0U);                             /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B1_09_SEMC_ADDR00, /* GPIO_EMC_B1_09 is configured as SEMC_ADDR00 */
      0U);                               /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B1_10_SEMC_ADDR01, /* GPIO_EMC_B1_10 is configured as SEMC_ADDR01 */
      0U);                               /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B1_11_SEMC_ADDR02, /* GPIO_EMC_B1_11 is configured as SEMC_ADDR02 */
      0U);                               /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B1_12_SEMC_ADDR03, /* GPIO_EMC_B1_12 is configured as SEMC_ADDR03 */
      0U);                               /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B1_13_SEMC_ADDR04, /* GPIO_EMC_B1_13 is configured as SEMC_ADDR04 */
      0U);                               /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B1_14_SEMC_ADDR05, /* GPIO_EMC_B1_14 is configured as SEMC_ADDR05 */
      0U);                               /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B1_15_SEMC_ADDR06, /* GPIO_EMC_B1_15 is configured as SEMC_ADDR06 */
      0U);                               /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B1_16_SEMC_ADDR07, /* GPIO_EMC_B1_16 is configured as SEMC_ADDR07 */
      0U);                               /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B1_17_SEMC_ADDR08, /* GPIO_EMC_B1_17 is configured as SEMC_ADDR08 */
      0U);                               /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B1_18_SEMC_ADDR09, /* GPIO_EMC_B1_18 is configured as SEMC_ADDR09 */
      0U);                               /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B1_19_SEMC_ADDR11, /* GPIO_EMC_B1_19 is configured as SEMC_ADDR11 */
      0U);                               /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B1_20_SEMC_ADDR12, /* GPIO_EMC_B1_20 is configured as SEMC_ADDR12 */
      0U);                               /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B1_21_SEMC_BA0, /* GPIO_EMC_B1_21 is configured as SEMC_BA0 */
      0U);                            /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B1_22_SEMC_BA1, /* GPIO_EMC_B1_22 is configured as SEMC_BA1 */
      0U);                            /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B1_23_SEMC_ADDR10, /* GPIO_EMC_B1_23 is configured as SEMC_ADDR10 */
      0U);                               /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B1_24_SEMC_CAS, /* GPIO_EMC_B1_24 is configured as SEMC_CAS */
      0U);                            /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B1_25_SEMC_RAS, /* GPIO_EMC_B1_25 is configured as SEMC_RAS */
      0U);                            /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B1_26_SEMC_CLK, /* GPIO_EMC_B1_26 is configured as SEMC_CLK */
      0U);                            /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B1_27_SEMC_CKE, /* GPIO_EMC_B1_27 is configured as SEMC_CKE */
      0U);                            /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B1_28_SEMC_WE, /* GPIO_EMC_B1_28 is configured as SEMC_WE */
      0U);                           /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B1_29_SEMC_CS0, /* GPIO_EMC_B1_29 is configured as SEMC_CS0 */
      0U);                            /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B1_30_SEMC_DATA08, /* GPIO_EMC_B1_30 is configured as SEMC_DATA08 */
      0U);                               /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B1_31_SEMC_DATA09, /* GPIO_EMC_B1_31 is configured as SEMC_DATA09 */
      0U);                               /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B1_32_SEMC_DATA10, /* GPIO_EMC_B1_32 is configured as SEMC_DATA10 */
      0U);                               /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B1_33_SEMC_DATA11, /* GPIO_EMC_B1_33 is configured as SEMC_DATA11 */
      0U);                               /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B1_34_SEMC_DATA12, /* GPIO_EMC_B1_34 is configured as SEMC_DATA12 */
      0U);                               /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B1_35_SEMC_DATA13, /* GPIO_EMC_B1_35 is configured as SEMC_DATA13 */
      0U);                               /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B1_36_SEMC_DATA14, /* GPIO_EMC_B1_36 is configured as SEMC_DATA14 */
      0U);                               /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B1_37_SEMC_DATA15, /* GPIO_EMC_B1_37 is configured as SEMC_DATA15 */
      0U);                               /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B1_38_SEMC_DM01, /* GPIO_EMC_B1_38 is configured as SEMC_DM01 */
      0U);                             /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B1_39_SEMC_DQS, /* GPIO_EMC_B1_39 is configured as SEMC_DQS */
      1U);                            /* Software Input On Field: Force input path of pad GPIO_EMC_B1_39 */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B1_40_SEMC_RDY, /* GPIO_EMC_B1_40 is configured as SEMC_RDY */
      0U);                            /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B1_41_SEMC_CSX00, /* GPIO_EMC_B1_41 is configured as SEMC_CSX00 */
      0U);                              /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B2_00_SEMC_DATA16, /* GPIO_EMC_B2_00 is configured as SEMC_DATA16 */
      0U);                               /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B2_01_SEMC_DATA17, /* GPIO_EMC_B2_01 is configured as SEMC_DATA17 */
      0U);                               /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B2_02_SEMC_DATA18, /* GPIO_EMC_B2_02 is configured as SEMC_DATA18 */
      0U);                               /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B2_03_SEMC_DATA19, /* GPIO_EMC_B2_03 is configured as SEMC_DATA19 */
      0U);                               /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B2_04_SEMC_DATA20, /* GPIO_EMC_B2_04 is configured as SEMC_DATA20 */
      0U);                               /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B2_05_SEMC_DATA21, /* GPIO_EMC_B2_05 is configured as SEMC_DATA21 */
      0U);                               /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B2_06_SEMC_DATA22, /* GPIO_EMC_B2_06 is configured as SEMC_DATA22 */
      0U);                               /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B2_07_SEMC_DATA23, /* GPIO_EMC_B2_07 is configured as SEMC_DATA23 */
      0U);                               /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B2_08_SEMC_DM02, /* GPIO_EMC_B2_08 is configured as SEMC_DM02 */
      0U);                             /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B2_09_SEMC_DATA24, /* GPIO_EMC_B2_09 is configured as SEMC_DATA24 */
      0U);                               /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B2_10_SEMC_DATA25, /* GPIO_EMC_B2_10 is configured as SEMC_DATA25 */
      0U);                               /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B2_11_SEMC_DATA26, /* GPIO_EMC_B2_11 is configured as SEMC_DATA26 */
      0U);                               /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B2_12_SEMC_DATA27, /* GPIO_EMC_B2_12 is configured as SEMC_DATA27 */
      0U);                               /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B2_13_SEMC_DATA28, /* GPIO_EMC_B2_13 is configured as SEMC_DATA28 */
      0U);                               /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B2_14_SEMC_DATA29, /* GPIO_EMC_B2_14 is configured as SEMC_DATA29 */
      0U);                               /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B2_15_SEMC_DATA30, /* GPIO_EMC_B2_15 is configured as SEMC_DATA30 */
      0U);                               /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B2_16_SEMC_DATA31, /* GPIO_EMC_B2_16 is configured as SEMC_DATA31 */
      0U);                               /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B2_17_SEMC_DM03, /* GPIO_EMC_B2_17 is configured as SEMC_DM03 */
      0U);                             /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_EMC_B2_18_SEMC_DQS4, /* GPIO_EMC_B2_18 is configured as SEMC_DQS4 */
      0U);                             /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B1_00_SEMC_DATA00, /* GPIO_EMC_B1_00 PAD functional properties : */
      0x08U);                            /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B1_01_SEMC_DATA01, /* GPIO_EMC_B1_01 PAD functional properties : */
      0x08U);                            /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B1_02_SEMC_DATA02, /* GPIO_EMC_B1_02 PAD functional properties : */
      0x08U);                            /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B1_03_SEMC_DATA03, /* GPIO_EMC_B1_03 PAD functional properties : */
      0x08U);                            /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B1_04_SEMC_DATA04, /* GPIO_EMC_B1_04 PAD functional properties : */
      0x08U);                            /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B1_05_SEMC_DATA05, /* GPIO_EMC_B1_05 PAD functional properties : */
      0x08U);                            /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B1_06_SEMC_DATA06, /* GPIO_EMC_B1_06 PAD functional properties : */
      0x08U);                            /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B1_07_SEMC_DATA07, /* GPIO_EMC_B1_07 PAD functional properties : */
      0x08U);                            /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B1_08_SEMC_DM00, /* GPIO_EMC_B1_08 PAD functional properties : */
      0x08U);                          /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B1_09_SEMC_ADDR00, /* GPIO_EMC_B1_09 PAD functional properties : */
      0x08U);                            /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B1_10_SEMC_ADDR01, /* GPIO_EMC_B1_10 PAD functional properties : */
      0x08U);                            /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B1_11_SEMC_ADDR02, /* GPIO_EMC_B1_11 PAD functional properties : */
      0x08U);                            /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B1_12_SEMC_ADDR03, /* GPIO_EMC_B1_12 PAD functional properties : */
      0x08U);                            /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B1_13_SEMC_ADDR04, /* GPIO_EMC_B1_13 PAD functional properties : */
      0x08U);                            /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B1_14_SEMC_ADDR05, /* GPIO_EMC_B1_14 PAD functional properties : */
      0x08U);                            /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B1_15_SEMC_ADDR06, /* GPIO_EMC_B1_15 PAD functional properties : */
      0x08U);                            /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B1_16_SEMC_ADDR07, /* GPIO_EMC_B1_16 PAD functional properties : */
      0x08U);                            /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B1_17_SEMC_ADDR08, /* GPIO_EMC_B1_17 PAD functional properties : */
      0x08U);                            /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B1_18_SEMC_ADDR09, /* GPIO_EMC_B1_18 PAD functional properties : */
      0x08U);                            /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B1_19_SEMC_ADDR11, /* GPIO_EMC_B1_19 PAD functional properties : */
      0x08U);                            /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B1_20_SEMC_ADDR12, /* GPIO_EMC_B1_20 PAD functional properties : */
      0x08U);                            /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B1_21_SEMC_BA0, /* GPIO_EMC_B1_21 PAD functional properties : */
      0x08U);                         /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B1_22_SEMC_BA1, /* GPIO_EMC_B1_22 PAD functional properties : */
      0x08U);                         /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B1_23_SEMC_ADDR10, /* GPIO_EMC_B1_23 PAD functional properties : */
      0x08U);                            /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B1_24_SEMC_CAS, /* GPIO_EMC_B1_24 PAD functional properties : */
      0x08U);                         /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B1_25_SEMC_RAS, /* GPIO_EMC_B1_25 PAD functional properties : */
      0x08U);                         /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B1_26_SEMC_CLK, /* GPIO_EMC_B1_26 PAD functional properties : */
      0x08U);                         /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B1_27_SEMC_CKE, /* GPIO_EMC_B1_27 PAD functional properties : */
      0x08U);                         /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B1_28_SEMC_WE, /* GPIO_EMC_B1_28 PAD functional properties : */
      0x08U);                        /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B1_29_SEMC_CS0, /* GPIO_EMC_B1_29 PAD functional properties : */
      0x08U);                         /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B1_30_SEMC_DATA08, /* GPIO_EMC_B1_30 PAD functional properties : */
      0x08U);                            /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B1_31_SEMC_DATA09, /* GPIO_EMC_B1_31 PAD functional properties : */
      0x08U);                            /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B1_32_SEMC_DATA10, /* GPIO_EMC_B1_32 PAD functional properties : */
      0x08U);                            /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B1_33_SEMC_DATA11, /* GPIO_EMC_B1_33 PAD functional properties : */
      0x08U);                            /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B1_34_SEMC_DATA12, /* GPIO_EMC_B1_34 PAD functional properties : */
      0x08U);                            /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B1_35_SEMC_DATA13, /* GPIO_EMC_B1_35 PAD functional properties : */
      0x08U);                            /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B1_36_SEMC_DATA14, /* GPIO_EMC_B1_36 PAD functional properties : */
      0x08U);                            /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B1_37_SEMC_DATA15, /* GPIO_EMC_B1_37 PAD functional properties : */
      0x08U);                            /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B1_38_SEMC_DM01, /* GPIO_EMC_B1_38 PAD functional properties : */
      0x08U);                          /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B1_39_SEMC_DQS, /* GPIO_EMC_B1_39 PAD functional properties : */
      0x08U);                         /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B1_40_SEMC_RDY, /* GPIO_EMC_B1_40 PAD functional properties : */
      0x08U);                         /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B1_41_SEMC_CSX00, /* GPIO_EMC_B1_41 PAD functional properties : */
      0x08U);                           /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B2_00_SEMC_DATA16, /* GPIO_EMC_B2_00 PAD functional properties : */
      0x08U);                            /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B2_01_SEMC_DATA17, /* GPIO_EMC_B2_01 PAD functional properties : */
      0x08U);                            /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B2_02_SEMC_DATA18, /* GPIO_EMC_B2_02 PAD functional properties : */
      0x08U);                            /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B2_03_SEMC_DATA19, /* GPIO_EMC_B2_03 PAD functional properties : */
      0x08U);                            /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B2_04_SEMC_DATA20, /* GPIO_EMC_B2_04 PAD functional properties : */
      0x08U);                            /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B2_05_SEMC_DATA21, /* GPIO_EMC_B2_05 PAD functional properties : */
      0x08U);                            /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B2_06_SEMC_DATA22, /* GPIO_EMC_B2_06 PAD functional properties : */
      0x08U);                            /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B2_07_SEMC_DATA23, /* GPIO_EMC_B2_07 PAD functional properties : */
      0x08U);                            /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B2_08_SEMC_DM02, /* GPIO_EMC_B2_08 PAD functional properties : */
      0x04U);                          /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pullup resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B2_09_SEMC_DATA24, /* GPIO_EMC_B2_09 PAD functional properties : */
      0x08U);                            /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B2_10_SEMC_DATA25, /* GPIO_EMC_B2_10 PAD functional properties : */
      0x08U);                            /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B2_11_SEMC_DATA26, /* GPIO_EMC_B2_11 PAD functional properties : */
      0x04U);                            /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pullup resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B2_12_SEMC_DATA27, /* GPIO_EMC_B2_12 PAD functional properties : */
      0x08U);                            /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B2_13_SEMC_DATA28, /* GPIO_EMC_B2_13 PAD functional properties : */
      0x08U);                            /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B2_14_SEMC_DATA29, /* GPIO_EMC_B2_14 PAD functional properties : */
      0x08U);                            /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B2_15_SEMC_DATA30, /* GPIO_EMC_B2_15 PAD functional properties : */
      0x08U);                            /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B2_16_SEMC_DATA31, /* GPIO_EMC_B2_16 PAD functional properties : */
      0x08U);                            /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B2_17_SEMC_DM03, /* GPIO_EMC_B2_17 PAD functional properties : */
      0x08U);                          /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_EMC_B2_18_SEMC_DQS4, /* GPIO_EMC_B2_18 PAD functional properties : */
      0x08U);                          /* PDRV Field: high drive strength
                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
}

void BOARD_InitUARTPins(void)
{
  CLOCK_EnableClock(kCLOCK_Iomuxc);

  IOMUXC_SetPinMux(
      IOMUXC_GPIO_AD_24_LPUART1_TXD, /* GPIO_AD_24 is configured as LPUART1_TXD */
      0U);                           /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_AD_25_LPUART1_RXD, /* GPIO_AD_25 is configured as LPUART1_RXD */
      0U);                           /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_AD_24_LPUART1_TXD, /* GPIO_AD_24 PAD functional properties : */
      0x06U);                        /* Slew Rate Field: Slow Slew Rate
                                                 Drive Strength Field: high drive strength
                                                 Pull / Keep Select Field: Pull Enable
                                                 Pull Up / Down Config. Field: Weak pull down
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_AD_25_LPUART1_RXD, /* GPIO_AD_25 PAD functional properties : */
      0x06U);                        /* Slew Rate Field: Slow Slew Rate
                                                 Drive Strength Field: high drive strength
                                                 Pull / Keep Select Field: Pull Enable
                                                 Pull Up / Down Config. Field: Weak pull down
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
}

void BOARD_InitLPUARTPins(void)
{
    IOMUXC_SetPinMux(
        IOMUXC_GPIO_DISP_B2_10_LPUART2_TXD,
        0U);
    IOMUXC_SetPinMux(
        IOMUXC_GPIO_DISP_B2_11_LPUART2_RXD,
        0U);
    IOMUXC_SetPinConfig(
        IOMUXC_GPIO_DISP_B2_10_LPUART2_TXD,
        0x02U);
    IOMUXC_SetPinConfig(
        IOMUXC_GPIO_DISP_B2_11_LPUART2_RXD,
        0x0EU);
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitADCPins, assigned for the Cortex-M7F core.
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 * END ****************************************************************************************************************/
void BOARD_InitADCPins(void) {
  CLOCK_EnableClock(kCLOCK_Iomuxc);           /* LPCG on: LPCG is ON. */

  IOMUXC_SetPinMux(
      IOMUXC_GPIO_AD_08_GPIO_MUX3_IO07,       /* GPIO_AD_08 is configured as GPIO_MUX3_IO07 */
      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_AD_11_GPIO_MUX3_IO10,       /* GPIO_AD_11 is configured as GPIO_MUX3_IO10 */
      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_AD_12_GPIO_MUX3_IO11,       /* GPIO_AD_12 is configured as GPIO_MUX3_IO11 */
      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_AD_13_GPIO_MUX3_IO12,       /* GPIO_AD_13 is configured as GPIO_MUX3_IO12 */
      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_AD_08_GPIO_MUX3_IO07,       /* GPIO_AD_08 PAD functional properties : */
      0x02U);                                 /* Slew Rate Field: Slow Slew Rate
                                                 Drive Strength Field: high drive strength
                                                 Pull / Keep Select Field: Pull Disable
                                                 Pull Up / Down Config. Field: Weak pull down
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_AD_11_GPIO_MUX3_IO10,       /* GPIO_AD_11 PAD functional properties : */
      0x02U);                                 /* Slew Rate Field: Slow Slew Rate
                                                 Drive Strength Field: high drive strength
                                                 Pull / Keep Select Field: Pull Disable
                                                 Pull Up / Down Config. Field: Weak pull down
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_AD_12_GPIO_MUX3_IO11,       /* GPIO_AD_12 PAD functional properties : */
      0x02U);                                 /* Slew Rate Field: Slow Slew Rate
                                                 Drive Strength Field: high drive strength
                                                 Pull / Keep Select Field: Pull Disable
                                                 Pull Up / Down Config. Field: Weak pull down
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_AD_13_GPIO_MUX3_IO12,       /* GPIO_AD_13 PAD functional properties : */
      0x02U);                                 /* Slew Rate Field: Slow Slew Rate
                                                 Drive Strength Field: high drive strength
                                                 Pull / Keep Select Field: Pull Disable
                                                 Pull Up / Down Config. Field: Weak pull down
                                                 Open Drain Field: Disabled
                                                 Domain write protection: Both cores are allowed
                                                 Domain write protection lock: Neither of DWP bits is locked */
}
