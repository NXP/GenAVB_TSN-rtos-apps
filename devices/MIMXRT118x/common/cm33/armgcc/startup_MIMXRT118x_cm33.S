/* ------------------------------------------------------------------------- */
/*  @file:    startup_MIMXRT1189_cm33.s                                      */
/*  @purpose: CMSIS Cortex-M33 Core Device Startup File                      */
/*            MIMXRT1189_cm33                                                */
/*  @version: 0.1                                                            */
/*  @date:    2021-3-9                                                       */
/*  @build:   b220309                                                        */
/* ------------------------------------------------------------------------- */
/*                                                                           */
/* Copyright 1997-2016 Freescale Semiconductor, Inc.                         */
/* Copyright 2016-2024 NXP                                                   */
/* All rights reserved.                                                      */
/*                                                                           */
/* SPDX-License-Identifier: BSD-3-Clause                                     */
/*****************************************************************************/
/* Version: GCC for ARM Embedded Processors                                  */
/*****************************************************************************/
    .syntax unified
    .arch armv8-m.main

    .section .isr_vector, "a"
    .align 2
    .globl __isr_vector
__isr_vector:
    .long   __StackTop                                      /* Top of Stack */
    .long   Reset_Handler                                   /* Reset Handler */
    .long   NMI_Handler                                     /* NMI Handler*/
    .long   Fault_Handler                                   /* Hard Fault Handler*/
    .long   Fault_Handler                                   /* MPU Fault Handler*/
    .long   Fault_Handler                                   /* Bus Fault Handler*/
    .long   Fault_Handler                                   /* Usage Fault Handler*/
    .long   Fault_Handler                                   /* Secure Fault Handler*/
    .long   0                                               /* Reserved*/
    .long   0                                               /* Reserved*/
    .long   0                                               /* Reserved*/
    .long   SVC_Handler                                     /* SVCall Handler*/
    .long   DebugMon_Handler                                /* Debug Monitor Handler*/
    .long   0                                               /* Reserved*/
    .long   PendSV_Handler                                  /* PendSV Handler*/
    .long   SysTick_Handler                                 /* SysTick Handler*/

                                                            /* External Interrupts*/
    .long   TMR1_IRQHandler                                 /* TMR1 interrupt*/
    .long   DAP_IRQHandler                                  /* DAP interrupt*/
    .long   M7_CTI_TRIGGER_OUTPUT_IRQHandler                /* CTI trigger outputs from CM7*/
    .long   M33_CTI_TRIGGER_OUTPUT_IRQHandler               /* CTI trigger outputs from CM33*/
    .long   TMR5_IRQHandler                                 /* TMR5 interrupt*/
    .long   TMR6_IRQHandler                                 /* TMR6 interrupt*/
    .long   TMR7_IRQHandler                                 /* TMR7 interrupt*/
    .long   TMR8_IRQHandler                                 /* TMR8 interrupt*/
    .long   CAN1_IRQHandler                                 /* CAN1 interrupt*/
    .long   CAN1_ERROR_IRQHandler                           /* CAN1 error interrupt*/
    .long   GPIO1_0_IRQHandler                              /* GPIO1 interrupt 0*/
    .long   GPIO1_1_IRQHandler                              /* GPIO1 interrupt 1*/
    .long   I3C1_IRQHandler                                 /* I3C1 interrupt*/
    .long   LPI2C1_IRQHandler                               /* LPI2C1 interrupt*/
    .long   LPI2C2_IRQHandler                               /* LPI2C2 interrupt*/
    .long   LPIT1_IRQHandler                                /* LPIT1 interrupt*/
    .long   LPSPI1_IRQHandler                               /* LPSPI1 interrupt*/
    .long   LPSPI2_IRQHandler                               /* LPSPI2 interrupt*/
    .long   LPTMR1_IRQHandler                               /* LPTMR1 interrupt*/
    .long   LPUART1_IRQHandler                              /* LPUART1 interrupt*/
    .long   LPUART2_IRQHandler                              /* LPUART2 interrupt*/
    .long   MU1_IRQHandler                                  /* MU1 interrupt*/
    .long   MU2_IRQHandler                                  /* MU2 interrupt*/
    .long   PWM1_FAULT_IRQHandler                           /* PWM1 fault or reload error interrupt*/
    .long   PWM1_0_IRQHandler                               /* PWM1 capture 0, compare 0, or reload 0 interrupt*/
    .long   PWM1_1_IRQHandler                               /* PWM1 capture 1, compare 1, or reload 1 interrupt*/
    .long   PWM1_2_IRQHandler                               /* PWM1 capture 2, compare 2, or reload 2 interrupt*/
    .long   PWM1_3_IRQHandler                               /* PWM1 capture 3, compare 3, or reload 3 interrupt*/
    .long   SENTINEL_TRUST_MUA_RX_FULL_IRQHandler           /* Sentinel Trust MUA RX full interrupt*/
    .long   SENTINEL_TRUST_MUA_TX_EMPTY_IRQHandler          /* Sentinel Trust MUA TX empty interrupt*/
    .long   SENTINEL_APPS_CORE_MUA_RX_FULL_IRQHandler       /* Sentinel Apps Core MUA RX full interrupt*/
    .long   SENTINEL_APPS_CORE_MUA_TX_EMPTY_IRQHandler      /* Sentinel Apps Core MUA TX empty interrupt*/
    .long   SENTINEL_REALTIME_CORE_MUA_RX_FULL_IRQHandler   /* Sentinel Realtime Core MUA RX full interrupt*/
    .long   SENTINEL_REALTIME_CORE_MUA_TX_EMPTY_IRQHandler  /* Sentinel Realtime Core MUA TX empty interrupt*/
    .long   SENTINEL_SECURE_IRQHandler                      /* Sentinel secure interrupt*/
    .long   SENTINEL_NONSECURE_IRQHandler                   /* Sentinel non-secure interrupt*/
    .long   TPM1_IRQHandler                                 /* TPM1 interrupt*/
    .long   TPM2_IRQHandler                                 /* TPM2 interrupt*/
    .long   RTWDOG1_IRQHandler                              /* RTWDOG1 interrupt*/
    .long   RTWDOG2_IRQHandler                              /* RTWDOG2 interrupt*/
    .long   Reserved56_IRQHandler                           /* Reserved interrupt*/
    .long   PDM_HWVAD_EVENT_IRQHandler                      /* HWVAD event interrupt*/
    .long   PDM_HWVAD_ERROR_IRQHandler                      /* HWVAD error interrupt*/
    .long   PDM_EVENT_IRQHandler                            /* PDM event interrupt*/
    .long   PDM_ERROR_IRQHandler                            /* PDM error interrupt*/
    .long   SAI1_IRQHandler                                 /* SAI interrupt*/
    .long   M33_PS_IRQHandler                               /* M33 PS Tag/Data Parity Error*/
    .long   M33_TCM_ECC_IRQHandler                          /* M33 TCM ECC interrupt*/
    .long   M33_TCM_ERROR_IRQHandler                        /* M33 TCM Error interrupt*/
    .long   M7_TCM_ECC_IRQHandler                           /* M7 TCM ECC interrupt*/
    .long   M7_TCM_ERROR_IRQHandler                         /* M7 TCM Error interrupt*/
    .long   CAN2_IRQHandler                                 /* CAN2 interrupt*/
    .long   CAN2_ERROR_IRQHandler                           /* CAN2 error interrupt*/
    .long   FLEXIO1_IRQHandler                              /* FLEXIO1 interrupt*/
    .long   FLEXIO2_IRQHandler                              /* FLEXIO2 interrupt*/
    .long   FLEXSPI1_IRQHandler                             /* FLEXSPI1 interrupt*/
    .long   FLEXSPI2_IRQHandler                             /* FLEXSPI2 interrupt*/
    .long   GPIO2_0_IRQHandler                              /* GPIO2 interrupt 0*/
    .long   GPIO2_1_IRQHandler                              /* GPIO2 interrupt 1*/
    .long   GPIO3_0_IRQHandler                              /* GPIO3 interrupt 0*/
    .long   GPIO3_1_IRQHandler                              /* GPIO3 interrupt 1*/
    .long   I3C2_IRQHandler                                 /* I3C2 interrupt*/
    .long   LPI2C3_IRQHandler                               /* LPI2C3 interrupt*/
    .long   LPI2C4_IRQHandler                               /* LPI2C4 interrput*/
    .long   LPIT2_IRQHandler                                /* LPIT2 interrupt*/
    .long   LPSPI3_IRQHandler                               /* LPSPI3 interrupt*/
    .long   LPSPI4_IRQHandler                               /* LPSPI4 interrupt*/
    .long   LPTMR2_IRQHandler                               /* LPTMR2 interrupt*/
    .long   LPUART3_IRQHandler                              /* LPUART3 interrupt*/
    .long   LPUART4_IRQHandler                              /* LPUART4 interrupt*/
    .long   LPUART5_IRQHandler                              /* LPUART5 interrupt*/
    .long   LPUART6_IRQHandler                              /* LPUART6 interrupt*/
    .long   ASRC_IRQHandler                                 /* ASRC interrupt*/
    .long   BBNSM_IRQHandler                                /* BBNSM iterrupt*/
    .long   SYS_CTRL1_IRQHandler                            /* System Counter compare interrupt 0 and 1*/
    .long   TPM3_IRQHandler                                 /* TPM3 interrupt*/
    .long   TPM4_IRQHandler                                 /* TPM4 interrupt*/
    .long   TPM5_IRQHandler                                 /* TPM5 interrupt*/
    .long   TPM6_IRQHandler                                 /* TPM6 interrupt*/
    .long   RTWDOG3_IRQHandler                              /* RTWDOG3 interrupt*/
    .long   RTWDOG4_IRQHandler                              /* RTWDOG4 interrupt*/
    .long   RTWDOG5_IRQHandler                              /* RTWDOG5 interrupt*/
    .long   Reserved98_IRQHandler                           /* Reserved interrupt*/
    .long   TEMPSENSOR_IRQHandler                           /* TempSensor alarm*/
    .long   Reserved100_IRQHandler                          /* Reserved interrupt*/
    .long   LDO_1P8_IRQHandler                              /* LDO 1P8 interrupt*/
    .long   USDHC1_IRQHandler                               /* USDHC1*/
    .long   USDHC2_IRQHandler                               /* USDHC2*/
    .long   Reserved104_IRQHandler                          /* Reserved interrupt*/
    .long   TMR2_IRQHandler                                 /* TMR2 interrupt*/
    .long   LDO_1P0_IRQHandler                              /* LDO 1P0 interrupt*/
    .long   MECC1_IRQHandler                                /* MECC1 interrupt*/
    .long   MECC2_IRQHandler                                /* MECC2 interrupt*/
    .long   ADC1_IRQHandler                                 /* ADC1 interrupt*/
    .long   DMA_ERROR_IRQHandler                            /* DMA error interrupt*/
    .long   DMA_0_IRQHandler                                /* DMA channel 0 interrupt*/
    .long   DMA_1_IRQHandler                                /* DMA channel 1 interrupt*/
    .long   DMA_2_IRQHandler                                /* DMA channel 2 interrupt*/
    .long   DMA_3_IRQHandler                                /* DMA channel 3 interrupt*/
    .long   DMA_4_IRQHandler                                /* DMA channel 4 interrupt*/
    .long   DMA_5_IRQHandler                                /* DMA channel 5 interrupt*/
    .long   DMA_6_IRQHandler                                /* DMA channel 6 interrupt*/
    .long   DMA_7_IRQHandler                                /* DMA channel 7 interrupt*/
    .long   DMA_8_IRQHandler                                /* DMA channel 8 interrupt*/
    .long   DMA_9_IRQHandler                                /* DMA channel 9 interrupt*/
    .long   DMA_10_IRQHandler                               /* DMA channel 10 interrupt*/
    .long   DMA_11_IRQHandler                               /* DMA channel 11 interrupt*/
    .long   DMA_12_IRQHandler                               /* DMA channel 12 interrupt*/
    .long   DMA_13_IRQHandler                               /* DMA channel 13 interrupt*/
    .long   DMA_14_IRQHandler                               /* DMA channel 14 interrupt*/
    .long   DMA_15_IRQHandler                               /* DMA channel 15 interrupt*/
    .long   DMA_16_IRQHandler                               /* DMA channel 16 interrupt*/
    .long   DMA_17_IRQHandler                               /* DMA channel 17 interrupt*/
    .long   DMA_18_IRQHandler                               /* DMA channel 18 interrupt*/
    .long   DMA_19_IRQHandler                               /* DMA channel 19 interrupt*/
    .long   DMA_20_IRQHandler                               /* DMA channel 20 interrupt*/
    .long   DMA_21_IRQHandler                               /* DMA channel 21 interrupt*/
    .long   DMA_22_IRQHandler                               /* DMA channel 22 interrupt*/
    .long   DMA_23_IRQHandler                               /* DMA channel 23 interrupt*/
    .long   DMA_24_IRQHandler                               /* DMA channel 24 interrupt*/
    .long   DMA_25_IRQHandler                               /* DMA channel 25 interrupt*/
    .long   DMA_26_IRQHandler                               /* DMA channel 26 interrupt*/
    .long   DMA_27_IRQHandler                               /* DMA channel 27 interrupt*/
    .long   DMA_28_IRQHandler                               /* DMA channel 28 interrupt*/
    .long   DMA_29_IRQHandler                               /* DMA channel 29 interrupt*/
    .long   DMA_30_IRQHandler                               /* DMA channel 30 interrupt*/
    .long   DMA_31_IRQHandler                               /* DMA channel 31 interrupt*/
    .long   DMA4_ERROR_IRQHandler                           /* DMA error interrupt*/
    .long   DMA4_0_1_IRQHandler                             /* DMA channel 0/1 interrupt*/
    .long   DMA4_2_3_IRQHandler                             /* DMA channel 2/3 interrupt*/
    .long   DMA4_4_5_IRQHandler                             /* DMA channel 4/5 interrupt*/
    .long   DMA4_6_7_IRQHandler                             /* DMA channel 6/7 interrupt*/
    .long   DMA4_8_9_IRQHandler                             /* DMA channel 8/9 interrupt*/
    .long   DMA4_10_11_IRQHandler                           /* DMA channel 10/11 interrupt*/
    .long   DMA4_12_13_IRQHandler                           /* DMA channel 12/13 interrupt*/
    .long   DMA4_14_15_IRQHandler                           /* DMA channel 14/15 interrupt*/
    .long   DMA4_16_17_IRQHandler                           /* DMA channel 16/17 interrupt*/
    .long   DMA4_18_19_IRQHandler                           /* DMA channel 18/19 interrupt*/
    .long   DMA4_20_21_IRQHandler                           /* DMA channel 20/21 interrupt*/
    .long   DMA4_22_23_IRQHandler                           /* DMA channel 22/23 interrupt*/
    .long   DMA4_24_25_IRQHandler                           /* DMA channel 24/25 interrupt*/
    .long   DMA4_26_27_IRQHandler                           /* DMA channel 26/27 interrupt*/
    .long   DMA4_28_29_IRQHandler                           /* DMA channel 28/29 interrupt*/
    .long   DMA4_30_31_IRQHandler                           /* DMA channel 30/31 interrupt*/
    .long   Reserved160_IRQHandler                          /* Reserved interrupt*/
    .long   Reserved161_IRQHandler                          /* Reserved interrupt*/
    .long   Reserved162_IRQHandler                          /* Reserved interrupt*/
    .long   Reserved163_IRQHandler                          /* Reserved interrupt*/
    .long   Reserved164_IRQHandler                          /* Reserved interrupt*/
    .long   Reserved165_IRQHandler                          /* Reserved interrupt*/
    .long   Reserved166_IRQHandler                          /* Reserved interrupt*/
    .long   Reserved167_IRQHandler                          /* Reserved interrupt*/
    .long   LPI2C5_IRQHandler                               /* LPI2C5 interrupt*/
    .long   LPI2C6_IRQHandler                               /* LPI2C6 interrupt*/
    .long   SAI4_IRQHandler                                 /* SAI4 interrupt*/
    .long   SPDIF_IRQHandler                                /* SPDIF interrupt*/
    .long   LPUART9_IRQHandler                              /* LPUART9 interrupt*/
    .long   LPUART10_IRQHandler                             /* LPUART10 interrupt*/
    .long   LPUART11_IRQHandler                             /* LPUART11 interrupt*/
    .long   LPUART12_IRQHandler                             /* LPUART12 interrupt*/
    .long   Reserved176_IRQHandler                          /* Reserved interrupt*/
    .long   Reserved177_IRQHandler                          /* Reserved interrupt*/
    .long   Reserved178_IRQHandler                          /* Reserved interrupt*/
    .long   Reserved179_IRQHandler                          /* Reserved interrupt*/
    .long   TMR3_IRQHandler                                 /* TMR3 interrupt*/
    .long   TMR4_IRQHandler                                 /* TMR4 interrupt*/
    .long   M33_SYSRESET_REQ_IRQHandler                     /* CM33 SYSREQRST SRC reset source*/
    .long   M33_LOCKUP_IRQHandler                           /* CM33 LOCKUP SRC reset source*/
    .long   M7_SYSRESET_REQ_IRQHandler                      /* CM33 SYSREQRST SRC reset source*/
    .long   M7_LOCKUP_IRQHandler                            /* CM33 LOCKUP SRC reset source*/
    .long   PWM2_FAULT_IRQHandler                           /* PWM2 fault or reload error interrupt*/
    .long   PWM2_0_IRQHandler                               /* PWM2 capture 0, compare 0, or reload 0 interrupt*/
    .long   PWM2_1_IRQHandler                               /* PWM2 capture 1, compare 1, or reload 1 interrupt*/
    .long   PWM2_2_IRQHandler                               /* PWM2 capture 2, compare 2, or reload 2 interrupt*/
    .long   PWM2_3_IRQHandler                               /* PWM2 capture 3, compare 3, or reload 3 interrupt*/
    .long   PWM3_FAULT_IRQHandler                           /* PWM3 fault or reload error interrupt*/
    .long   PWM3_0_IRQHandler                               /* PWM3 capture 0, compare 0, or reload 0 interrupt*/
    .long   PWM3_1_IRQHandler                               /* PWM3 capture 1, compare 1, or reload 1 interrupt*/
    .long   PWM3_2_IRQHandler                               /* PWM3 capture 2, compare 2, or reload 2 interrupt*/
    .long   PWM3_3_IRQHandler                               /* PWM3 capture 3, compare 3, or reload 3 interrupt*/
    .long   PWM4_FAULT_IRQHandler                           /* PWM4 fault or reload error interrupt*/
    .long   PWM4_0_IRQHandler                               /* PWM4 capture 0, compare 0, or reload 0 interrupt*/
    .long   PWM4_1_IRQHandler                               /* PWM4 capture 1, compare 1, or reload 1 interrupt*/
    .long   PWM4_2_IRQHandler                               /* PWM4 capture 2, compare 2, or reload 2 interrupt*/
    .long   PWM4_3_IRQHandler                               /* PWM4 capture 3, compare 3, or reload 3 interrupt*/
    .long   EQDC1_IRQHandler                                /* EQDC1 interrupt*/
    .long   EQDC2_IRQHandler                                /* EQDC2 interrupt*/
    .long   EQDC3_IRQHandler                                /* EQDC3 interrupt*/
    .long   EQDC4_IRQHandler                                /* EQDC4 interrupt*/
    .long   ADC2_IRQHandler                                 /* ADC2 interrupt*/
    .long   DCDC_IRQHandler                                 /* DCDC interrupt*/
    .long   CAN3_IRQHandler                                 /* CAN3 interrupt*/
    .long   CAN3_ERROR_IRQHandler                           /* CAN3 error interrupt*/
    .long   DAC_IRQHandler                                  /* DAC interrupt*/
    .long   LPSPI5_IRQHandler                               /* LPSPI5 interrupt*/
    .long   LPSPI6_IRQHandler                               /* LPSPI6 interrupt*/
    .long   LPUART7_IRQHandler                              /* LPUART7 interrupt*/
    .long   LPUART8_IRQHandler                              /* LPUART8 interrupt*/
    .long   SAI2_IRQHandler                                 /* SAI2 interrupt*/
    .long   SAI3_IRQHandler                                 /* SAI3 interrupt*/
    .long   ACMP1_IRQHandler                                /* CMP1 interrupt*/
    .long   ACMP2_IRQHandler                                /* CMP2 interrupt*/
    .long   ACMP3_IRQHandler                                /* CMP3 interrupt*/
    .long   ACMP4_IRQHandler                                /* CMP4 interrupt*/
    .long   Reserved220_IRQHandler                          /* Reserved interrupt*/
    .long   Reserved221_IRQHandler                          /* Reserved interrupt*/
    .long   Reserved222_IRQHandler                          /* Reserved interrupt*/
    .long   Reserved223_IRQHandler                          /* Reserved interrupt*/
    .long   Reserved224_IRQHandler                          /* Reserved interrupt*/
    .long   GPT1_IRQHandler                                 /* GPT1 interrupt*/
    .long   GPT2_IRQHandler                                 /* GPT2 interrupt*/
    .long   KPP_IRQHandler                                  /* KPP interrupt*/
    .long   USBPHY1_IRQHandler                              /* USBPHY1 interrupt*/
    .long   USBPHY2_IRQHandler                              /* USBPHY2 interrupt*/
    .long   USB_OTG2_IRQHandler                             /* USBOTG2 interrupt*/
    .long   USB_OTG1_IRQHandler                             /* USBOTG1 interrupt*/
    .long   XSPI_SLV_IRQHandler                             /* XSPI_SLV interrupt*/
    .long   NETC_IRQHandler                                 /* NETC interrupt*/
    .long   MSGINTR1_IRQHandler                             /* MSGINTR1 interrupt*/
    .long   MSGINTR2_IRQHandler                             /* MSGINTR2 interrupt*/
    .long   MSGINTR3_IRQHandler                             /* MSGINTR3 interrupt*/
    .long   MSGINTR4_IRQHandler                             /* MSGINTR4 interrupt*/
    .long   MSGINTR5_IRQHandler                             /* MSGINTR5 interrupt*/
    .long   MSGINTR6_IRQHandler                             /* MSGINTR6 interrupt*/
    .long   SINC1_0_IRQHandler                              /* SINC Filter Glue 1 channel 0*/
    .long   SINC1_1_IRQHandler                              /* SINC Filter Glue 1 channel 1*/
    .long   SINC1_2_IRQHandler                              /* SINC Filter Glue 1 channel 2*/
    .long   SINC1_3_IRQHandler                              /* SINC Filter Glue 1 channel 3*/
    .long   SINC2_0_IRQHandler                              /* SINC Filter Glue 2 channel 0*/
    .long   SINC2_1_IRQHandler                              /* SINC Filter Glue 2 channel 1*/
    .long   SINC2_2_IRQHandler                              /* SINC Filter Glue 2 channel 2*/
    .long   SINC2_3_IRQHandler                              /* SINC Filter Glue 2 channel 3*/
    .long   GPIO4_0_IRQHandler                              /* GPIO4 interrupt 0*/
    .long   GPIO4_1_IRQHandler                              /* GPIO4 interrupt 1*/
    .long   GPIO5_0_IRQHandler                              /* GPIO5 interrupt 0*/
    .long   GPIO5_1_IRQHandler                              /* GPIO5 interrupt 1*/
    .long   GPIO6_0_IRQHandler                              /* GPIO6 interrupt 0*/
    .long   GPIO6_1_IRQHandler                              /* GPIO6 interrupt 1*/
    .long   ECAT_IRQHandler                                 /* ECAT interrupt*/
    .long   DefaultISR                                      /* 255*/

    .size    __isr_vector, . - __isr_vector

    .text
    .thumb

/* Reset Handler */

    .thumb_func
    .align 2
    .globl   Reset_Handler
    .weak    Reset_Handler
    .type    Reset_Handler, %function
Reset_Handler:
    cpsid   i               /* Mask interrupts */
    .equ    VTOR, 0xE000ED08
    ldr     r0, =VTOR
    ldr     r1, =__isr_vector
    str     r1, [r0]
    ldr     r2, [r1]
    msr     msp, r2
    ldr     r0, =__StackLimit
    msr     msplim, r0

#ifdef __STARTUP_INITIALIZE_TCM_ECC
/*  After POR, ECC for Code/System TCM memory is enabled by default but not initialized.
 *  Also, the ECC initialization done by boot rom is disabled by fuse configuration.
 *  Zero out all TCM memory to correctly initialize ECC.
 */
    ldr r1, =m_code_tcm_start
    ldr r2, =m_code_tcm_end

    movs    r0, 0
.LC0:
    cmp     r1, r2
    itt    lt
    strlt   r0, [r1], #4
    blt    .LC0

    ldr r1, =m_system_tcm_start
    ldr r2, =m_system_tcm_end

    movs    r0, 0
.LC1:
    cmp     r1, r2
    itt    lt
    strlt   r0, [r1], #4
    blt    .LC1
#endif /* __STARTUP_INITIALIZE_TCM_ECC */

#ifndef __NO_SYSTEM_INIT
    ldr   r0,=SystemInit
    blx   r0
#endif

#ifndef __RAM_BUILD
/*     Loop to copy data from read only memory to RAM. The ranges
 *      of copy from/to are specified by following symbols evaluated in
 *      linker script.
 *      __data_load__: begin of data sections to copy from.
 *      __data_start__/__data_end__: RAM address range that data should be
 *      __noncachedata_start__/__noncachedata_end__ : none cachable region
 *      copied to. Both must be aligned to 4 bytes boundary.  */

    ldr    r1, =__data_load__
    ldr    r2, =__data_start__
    ldr    r3, =__data_end__

#ifdef __PERFORMANCE_IMPLEMENTATION
/* Here are two copies of loop implementations. First one favors performance
 * and the second one favors code size. Default uses the second one.
 * Define macro "__PERFORMANCE_IMPLEMENTATION" in project to use the first one */
    subs    r3, r2
    ble    .LC3
.LC2:
    subs    r3, #4
    ldr    r0, [r1, r3]
    str    r0, [r2, r3]
    bgt    .LC2
.LC3:
#else  /* code size implemenation */
.LC2:
    cmp     r2, r3
    ittt    lt
    ldrlt   r0, [r1], #4
    strlt   r0, [r2], #4
    blt    .LC2
#endif
#ifdef __STARTUP_INITIALIZE_NONCACHEDATA
    ldr    r2, =__noncachedata_start__
    ldr    r3, =__noncachedata_init_end__
#ifdef __PERFORMANCE_IMPLEMENTATION
/* Here are two copies of loop implementations. First one favors performance
 * and the second one favors code size. Default uses the second one.
 * Define macro "__PERFORMANCE_IMPLEMENTATION" in project to use the first one */
    subs    r3, r2
    ble    .LC5
.LC4:
    subs    r3, #4
    ldr    r0, [r1, r3]
    str    r0, [r2, r3]
    bgt    .LC4
.LC5:
#else  /* code size implemenation */
.LC4:
    cmp     r2, r3
    ittt    lt
    ldrlt   r0, [r1], #4
    strlt   r0, [r2], #4
    blt    .LC4
#endif
#endif /* __STARTUP_INITIALIZE_NONCACHEDATA */

#ifdef __STARTUP_CLEAR_BSS
/*     This part of work usually is done in C library startup code. Otherwise,
 *     define this macro to enable it in this startup.
 *
 *     Loop to zero out BSS section, which uses following symbols
 *     in linker script:
 *      __START_BSS: start of BSS section. Must align to 4
 *      __END_BSS: end of BSS section. Must align to 4
 */
    ldr    r1, =__START_BSS
    ldr    r2, =__END_BSS

    movs    r0, 0
.LC6:
    cmp     r1, r2
    itt    lt
    strlt   r0, [r1], #4
    blt    .LC6
#endif /* __STARTUP_CLEAR_BSS */

/* zero inited ncache section initialization */
    ldr r2, =__noncachedata_init_end__
    ldr r3, =__noncachedata_end__
    movs    r0,0
.LC7:
    cmp    r2,r3
    itt    lt
    strlt   r0,[r2],#4
    blt    .LC7

/* Copy text from flash to memory */
    ldr    r1, =__text_load
    ldr    r2, =__text_start
    ldr    r3, =__text_end

.LC8:
    cmp     r2, r3
    ittt    lt
    ldrlt   r0, [r1], #4
    strlt   r0, [r2], #4
    blt    .LC8

/* Copy slow text from flash to memory */
    ldr    r1, =__slow_text_load
    ldr    r2, =__slow_text_start
    ldr    r3, =__slow_text_end

.LC9:
    cmp     r2, r3
    ittt    lt
    ldrlt   r0, [r1], #4
    strlt   r0, [r2], #4
    blt    .LC9

/* Clear slow BSS section */
    ldr r1, =__slow_bss_start
    ldr r2, =__slow_bss_end

    movs    r0, 0
.LC10:
    cmp     r1, r2
    itt    lt
    strlt   r0, [r1], #4
    blt    .LC10

/* Relocate vector table */
    ldr    r1, =__isr_vector
    ldr    r2, =__ram_isr_vector_start
    ldr    r3, =__ram_isr_vector_end

.LC11:
    cmp     r2, r3
    ittt    lt
    ldrlt   r0, [r1], #4
    strlt   r0, [r2], #4
    blt    .LC11
#endif /* __RAM_BUILD */

/* We assume that BOARD_Init() code is in code_tcm or ocram
 * If hyperram is enabled, FlexSPI2 is initialized in this function.
 */
    ldr    r0, =BOARD_Init
    blx    r0

#ifndef __RAM_BUILD
#ifdef __HYPERRAM_BUILD
/* Copy hyperram text from flash to memory */
    ldr    r1, =__hyperram_text_load
    ldr    r2, =__hyperram_text_start
    ldr    r3, =__hyperram_text_end

.LC12:
    cmp     r2, r3
    ittt    lt
    ldrlt   r0, [r1], #4
    strlt   r0, [r2], #4
    blt    .LC12

    /* Copy hyperram data from flash to memory */
    ldr    r1, =__hyperram_data_load
    ldr    r2, =__hyperram_data_start
    ldr    r3, =__hyperram_data_end

.LC13:
    cmp     r2, r3
    ittt    lt
    ldrlt   r0, [r1], #4
    strlt   r0, [r2], #4
    blt    .LC13

    /* Clear hyperram BSS section */
    ldr    r1, =__hyperram_bss_start
    ldr    r2, =__hyperram_bss_end

    movs    r0, 0
.LC14:
    cmp     r1, r2
    itt    lt
    strlt   r0, [r1], #4
    blt    .LC14
#endif

/* Update VTOR offset register */
    .equ    VTOR, 0xE000ED08
    ldr     r0, =VTOR
    ldr     r1, =__ram_isr_vector_start
    str     r1, [r0]
#endif /* __RAM_BUILD */

/* Add stack / heap initializaiton */
    movs    r0, 0
    ldr     r1, =__HeapBase
    ldr     r2, =__HeapLimit
.LC15:
    cmp     r1, r2
    itt     lt
    strlt   r0, [r1], #4
    blt     .LC15

    ldr     r1, =__StackLimit
    ldr     r2, =__StackTop
.LC16:
    cmp     r1, r2
    itt     lt
    strlt   r0, [r1], #4
    blt     .LC16

    cpsie   i               /* Unmask interrupts */
#ifndef __START
#define __START _start
#endif
#ifndef __ATOLLIC__
    ldr   r0,=__START
    blx   r0
#else
    ldr   r0,=__libc_init_array
    blx   r0
    ldr   r0,=main
    bx    r0
#endif
    .pool
    .size Reset_Handler, . - Reset_Handler

    .align  1
    .thumb_func
    .weak DefaultISR
    .type DefaultISR, %function
DefaultISR:
    b DefaultISR
    .size DefaultISR, . - DefaultISR

    .align 1
    .thumb_func
    .weak NMI_Handler
    .type NMI_Handler, %function
NMI_Handler:
    ldr   r0,=NMI_Handler
    bx    r0
    .size NMI_Handler, . - NMI_Handler

    .align 1
    .thumb_func
    .type Fault_Handler, %function
Fault_Handler:
# On exception entry, lr has control information and bit 3 tells if return will
# occur using main or process stack
# See Armv8-M RM section D1.2.95
    tst     lr, #4
    ite     eq
    mrseq   r0, msp
    mrsne   r0, psp
# encode exception number in r1, second argument
    mrs     r1, ipsr
    ldr     r2,=Fault_Dispatcher
    bx      r2

    .align 1
    .thumb_func
    .weak SVC_Handler
    .type SVC_Handler, %function
SVC_Handler:
    ldr   r0,=SVC_Handler
    bx    r0
    .size SVC_Handler, . - SVC_Handler

    .align 1
    .thumb_func
    .weak PendSV_Handler
    .type PendSV_Handler, %function
PendSV_Handler:
    ldr   r0,=PendSV_Handler
    bx    r0
    .size PendSV_Handler, . - PendSV_Handler

    .align 1
    .thumb_func
    .weak SysTick_Handler
    .type SysTick_Handler, %function
SysTick_Handler:
    ldr   r0,=SysTick_Handler
    bx    r0
    .size SysTick_Handler, . - SysTick_Handler

    .align 1
    .thumb_func
    .weak CAN1_IRQHandler
    .type CAN1_IRQHandler, %function
CAN1_IRQHandler:
    ldr   r0,=CAN1_DriverIRQHandler
    bx    r0
    .size CAN1_IRQHandler, . - CAN1_IRQHandler

    .align 1
    .thumb_func
    .weak CAN1_ERROR_IRQHandler
    .type CAN1_ERROR_IRQHandler, %function
CAN1_ERROR_IRQHandler:
    ldr   r0,=CAN1_ERROR_DriverIRQHandler
    bx    r0
    .size CAN1_ERROR_IRQHandler, . - CAN1_ERROR_IRQHandler

    .align 1
    .thumb_func
    .weak I3C1_IRQHandler
    .type I3C1_IRQHandler, %function
I3C1_IRQHandler:
    ldr   r0,=I3C1_DriverIRQHandler
    bx    r0
    .size I3C1_IRQHandler, . - I3C1_IRQHandler

    .align 1
    .thumb_func
    .weak LPI2C1_IRQHandler
    .type LPI2C1_IRQHandler, %function
LPI2C1_IRQHandler:
    ldr   r0,=LPI2C1_DriverIRQHandler
    bx    r0
    .size LPI2C1_IRQHandler, . - LPI2C1_IRQHandler

    .align 1
    .thumb_func
    .weak LPI2C2_IRQHandler
    .type LPI2C2_IRQHandler, %function
LPI2C2_IRQHandler:
    ldr   r0,=LPI2C2_DriverIRQHandler
    bx    r0
    .size LPI2C2_IRQHandler, . - LPI2C2_IRQHandler

    .align 1
    .thumb_func
    .weak LPSPI1_IRQHandler
    .type LPSPI1_IRQHandler, %function
LPSPI1_IRQHandler:
    ldr   r0,=LPSPI1_DriverIRQHandler
    bx    r0
    .size LPSPI1_IRQHandler, . - LPSPI1_IRQHandler

    .align 1
    .thumb_func
    .weak LPSPI2_IRQHandler
    .type LPSPI2_IRQHandler, %function
LPSPI2_IRQHandler:
    ldr   r0,=LPSPI2_DriverIRQHandler
    bx    r0
    .size LPSPI2_IRQHandler, . - LPSPI2_IRQHandler

    .align 1
    .thumb_func
    .weak LPUART1_IRQHandler
    .type LPUART1_IRQHandler, %function
LPUART1_IRQHandler:
    ldr   r0,=LPUART1_DriverIRQHandler
    bx    r0
    .size LPUART1_IRQHandler, . - LPUART1_IRQHandler

    .align 1
    .thumb_func
    .weak LPUART2_IRQHandler
    .type LPUART2_IRQHandler, %function
LPUART2_IRQHandler:
    ldr   r0,=LPUART2_DriverIRQHandler
    bx    r0
    .size LPUART2_IRQHandler, . - LPUART2_IRQHandler

    .align 1
    .thumb_func
    .weak PDM_HWVAD_EVENT_IRQHandler
    .type PDM_HWVAD_EVENT_IRQHandler, %function
PDM_HWVAD_EVENT_IRQHandler:
    ldr   r0,=PDM_HWVAD_EVENT_DriverIRQHandler
    bx    r0
    .size PDM_HWVAD_EVENT_IRQHandler, . - PDM_HWVAD_EVENT_IRQHandler

    .align 1
    .thumb_func
    .weak PDM_HWVAD_ERROR_IRQHandler
    .type PDM_HWVAD_ERROR_IRQHandler, %function
PDM_HWVAD_ERROR_IRQHandler:
    ldr   r0,=PDM_HWVAD_ERROR_DriverIRQHandler
    bx    r0
    .size PDM_HWVAD_ERROR_IRQHandler, . - PDM_HWVAD_ERROR_IRQHandler

    .align 1
    .thumb_func
    .weak PDM_EVENT_IRQHandler
    .type PDM_EVENT_IRQHandler, %function
PDM_EVENT_IRQHandler:
    ldr   r0,=PDM_EVENT_DriverIRQHandler
    bx    r0
    .size PDM_EVENT_IRQHandler, . - PDM_EVENT_IRQHandler

    .align 1
    .thumb_func
    .weak PDM_ERROR_IRQHandler
    .type PDM_ERROR_IRQHandler, %function
PDM_ERROR_IRQHandler:
    ldr   r0,=PDM_ERROR_DriverIRQHandler
    bx    r0
    .size PDM_ERROR_IRQHandler, . - PDM_ERROR_IRQHandler

    .align 1
    .thumb_func
    .weak SAI1_IRQHandler
    .type SAI1_IRQHandler, %function
SAI1_IRQHandler:
    ldr   r0,=SAI1_DriverIRQHandler
    bx    r0
    .size SAI1_IRQHandler, . - SAI1_IRQHandler

    .align 1
    .thumb_func
    .weak CAN2_IRQHandler
    .type CAN2_IRQHandler, %function
CAN2_IRQHandler:
    ldr   r0,=CAN2_DriverIRQHandler
    bx    r0
    .size CAN2_IRQHandler, . - CAN2_IRQHandler

    .align 1
    .thumb_func
    .weak CAN2_ERROR_IRQHandler
    .type CAN2_ERROR_IRQHandler, %function
CAN2_ERROR_IRQHandler:
    ldr   r0,=CAN2_ERROR_DriverIRQHandler
    bx    r0
    .size CAN2_ERROR_IRQHandler, . - CAN2_ERROR_IRQHandler

    .align 1
    .thumb_func
    .weak FLEXIO1_IRQHandler
    .type FLEXIO1_IRQHandler, %function
FLEXIO1_IRQHandler:
    ldr   r0,=FLEXIO1_DriverIRQHandler
    bx    r0
    .size FLEXIO1_IRQHandler, . - FLEXIO1_IRQHandler

    .align 1
    .thumb_func
    .weak FLEXIO2_IRQHandler
    .type FLEXIO2_IRQHandler, %function
FLEXIO2_IRQHandler:
    ldr   r0,=FLEXIO2_DriverIRQHandler
    bx    r0
    .size FLEXIO2_IRQHandler, . - FLEXIO2_IRQHandler

    .align 1
    .thumb_func
    .weak FLEXSPI1_IRQHandler
    .type FLEXSPI1_IRQHandler, %function
FLEXSPI1_IRQHandler:
    ldr   r0,=FLEXSPI1_DriverIRQHandler
    bx    r0
    .size FLEXSPI1_IRQHandler, . - FLEXSPI1_IRQHandler

    .align 1
    .thumb_func
    .weak FLEXSPI2_IRQHandler
    .type FLEXSPI2_IRQHandler, %function
FLEXSPI2_IRQHandler:
    ldr   r0,=FLEXSPI2_DriverIRQHandler
    bx    r0
    .size FLEXSPI2_IRQHandler, . - FLEXSPI2_IRQHandler

    .align 1
    .thumb_func
    .weak I3C2_IRQHandler
    .type I3C2_IRQHandler, %function
I3C2_IRQHandler:
    ldr   r0,=I3C2_DriverIRQHandler
    bx    r0
    .size I3C2_IRQHandler, . - I3C2_IRQHandler

    .align 1
    .thumb_func
    .weak LPI2C3_IRQHandler
    .type LPI2C3_IRQHandler, %function
LPI2C3_IRQHandler:
    ldr   r0,=LPI2C3_DriverIRQHandler
    bx    r0
    .size LPI2C3_IRQHandler, . - LPI2C3_IRQHandler

    .align 1
    .thumb_func
    .weak LPI2C4_IRQHandler
    .type LPI2C4_IRQHandler, %function
LPI2C4_IRQHandler:
    ldr   r0,=LPI2C4_DriverIRQHandler
    bx    r0
    .size LPI2C4_IRQHandler, . - LPI2C4_IRQHandler

    .align 1
    .thumb_func
    .weak LPSPI3_IRQHandler
    .type LPSPI3_IRQHandler, %function
LPSPI3_IRQHandler:
    ldr   r0,=LPSPI3_DriverIRQHandler
    bx    r0
    .size LPSPI3_IRQHandler, . - LPSPI3_IRQHandler

    .align 1
    .thumb_func
    .weak LPSPI4_IRQHandler
    .type LPSPI4_IRQHandler, %function
LPSPI4_IRQHandler:
    ldr   r0,=LPSPI4_DriverIRQHandler
    bx    r0
    .size LPSPI4_IRQHandler, . - LPSPI4_IRQHandler

    .align 1
    .thumb_func
    .weak LPUART3_IRQHandler
    .type LPUART3_IRQHandler, %function
LPUART3_IRQHandler:
    ldr   r0,=LPUART3_DriverIRQHandler
    bx    r0
    .size LPUART3_IRQHandler, . - LPUART3_IRQHandler

    .align 1
    .thumb_func
    .weak LPUART4_IRQHandler
    .type LPUART4_IRQHandler, %function
LPUART4_IRQHandler:
    ldr   r0,=LPUART4_DriverIRQHandler
    bx    r0
    .size LPUART4_IRQHandler, . - LPUART4_IRQHandler

    .align 1
    .thumb_func
    .weak LPUART5_IRQHandler
    .type LPUART5_IRQHandler, %function
LPUART5_IRQHandler:
    ldr   r0,=LPUART5_DriverIRQHandler
    bx    r0
    .size LPUART5_IRQHandler, . - LPUART5_IRQHandler

    .align 1
    .thumb_func
    .weak LPUART6_IRQHandler
    .type LPUART6_IRQHandler, %function
LPUART6_IRQHandler:
    ldr   r0,=LPUART6_DriverIRQHandler
    bx    r0
    .size LPUART6_IRQHandler, . - LPUART6_IRQHandler

    .align 1
    .thumb_func
    .weak ASRC_IRQHandler
    .type ASRC_IRQHandler, %function
ASRC_IRQHandler:
    ldr   r0,=ASRC_DriverIRQHandler
    bx    r0
    .size ASRC_IRQHandler, . - ASRC_IRQHandler

    .align 1
    .thumb_func
    .weak USDHC1_IRQHandler
    .type USDHC1_IRQHandler, %function
USDHC1_IRQHandler:
    ldr   r0,=USDHC1_DriverIRQHandler
    bx    r0
    .size USDHC1_IRQHandler, . - USDHC1_IRQHandler

    .align 1
    .thumb_func
    .weak USDHC2_IRQHandler
    .type USDHC2_IRQHandler, %function
USDHC2_IRQHandler:
    ldr   r0,=USDHC2_DriverIRQHandler
    bx    r0
    .size USDHC2_IRQHandler, . - USDHC2_IRQHandler

    .align 1
    .thumb_func
    .weak DMA_ERROR_IRQHandler
    .type DMA_ERROR_IRQHandler, %function
DMA_ERROR_IRQHandler:
    ldr   r0,=DMA_ERROR_DriverIRQHandler
    bx    r0
    .size DMA_ERROR_IRQHandler, . - DMA_ERROR_IRQHandler

    .align 1
    .thumb_func
    .weak DMA_0_IRQHandler
    .type DMA_0_IRQHandler, %function
DMA_0_IRQHandler:
    ldr   r0,=DMA_0_DriverIRQHandler
    bx    r0
    .size DMA_0_IRQHandler, . - DMA_0_IRQHandler

    .align 1
    .thumb_func
    .weak DMA_1_IRQHandler
    .type DMA_1_IRQHandler, %function
DMA_1_IRQHandler:
    ldr   r0,=DMA_1_DriverIRQHandler
    bx    r0
    .size DMA_1_IRQHandler, . - DMA_1_IRQHandler

    .align 1
    .thumb_func
    .weak DMA_2_IRQHandler
    .type DMA_2_IRQHandler, %function
DMA_2_IRQHandler:
    ldr   r0,=DMA_2_DriverIRQHandler
    bx    r0
    .size DMA_2_IRQHandler, . - DMA_2_IRQHandler

    .align 1
    .thumb_func
    .weak DMA_3_IRQHandler
    .type DMA_3_IRQHandler, %function
DMA_3_IRQHandler:
    ldr   r0,=DMA_3_DriverIRQHandler
    bx    r0
    .size DMA_3_IRQHandler, . - DMA_3_IRQHandler

    .align 1
    .thumb_func
    .weak DMA_4_IRQHandler
    .type DMA_4_IRQHandler, %function
DMA_4_IRQHandler:
    ldr   r0,=DMA_4_DriverIRQHandler
    bx    r0
    .size DMA_4_IRQHandler, . - DMA_4_IRQHandler

    .align 1
    .thumb_func
    .weak DMA_5_IRQHandler
    .type DMA_5_IRQHandler, %function
DMA_5_IRQHandler:
    ldr   r0,=DMA_5_DriverIRQHandler
    bx    r0
    .size DMA_5_IRQHandler, . - DMA_5_IRQHandler

    .align 1
    .thumb_func
    .weak DMA_6_IRQHandler
    .type DMA_6_IRQHandler, %function
DMA_6_IRQHandler:
    ldr   r0,=DMA_6_DriverIRQHandler
    bx    r0
    .size DMA_6_IRQHandler, . - DMA_6_IRQHandler

    .align 1
    .thumb_func
    .weak DMA_7_IRQHandler
    .type DMA_7_IRQHandler, %function
DMA_7_IRQHandler:
    ldr   r0,=DMA_7_DriverIRQHandler
    bx    r0
    .size DMA_7_IRQHandler, . - DMA_7_IRQHandler

    .align 1
    .thumb_func
    .weak DMA_8_IRQHandler
    .type DMA_8_IRQHandler, %function
DMA_8_IRQHandler:
    ldr   r0,=DMA_8_DriverIRQHandler
    bx    r0
    .size DMA_8_IRQHandler, . - DMA_8_IRQHandler

    .align 1
    .thumb_func
    .weak DMA_9_IRQHandler
    .type DMA_9_IRQHandler, %function
DMA_9_IRQHandler:
    ldr   r0,=DMA_9_DriverIRQHandler
    bx    r0
    .size DMA_9_IRQHandler, . - DMA_9_IRQHandler

    .align 1
    .thumb_func
    .weak DMA_10_IRQHandler
    .type DMA_10_IRQHandler, %function
DMA_10_IRQHandler:
    ldr   r0,=DMA_10_DriverIRQHandler
    bx    r0
    .size DMA_10_IRQHandler, . - DMA_10_IRQHandler

    .align 1
    .thumb_func
    .weak DMA_11_IRQHandler
    .type DMA_11_IRQHandler, %function
DMA_11_IRQHandler:
    ldr   r0,=DMA_11_DriverIRQHandler
    bx    r0
    .size DMA_11_IRQHandler, . - DMA_11_IRQHandler

    .align 1
    .thumb_func
    .weak DMA_12_IRQHandler
    .type DMA_12_IRQHandler, %function
DMA_12_IRQHandler:
    ldr   r0,=DMA_12_DriverIRQHandler
    bx    r0
    .size DMA_12_IRQHandler, . - DMA_12_IRQHandler

    .align 1
    .thumb_func
    .weak DMA_13_IRQHandler
    .type DMA_13_IRQHandler, %function
DMA_13_IRQHandler:
    ldr   r0,=DMA_13_DriverIRQHandler
    bx    r0
    .size DMA_13_IRQHandler, . - DMA_13_IRQHandler

    .align 1
    .thumb_func
    .weak DMA_14_IRQHandler
    .type DMA_14_IRQHandler, %function
DMA_14_IRQHandler:
    ldr   r0,=DMA_14_DriverIRQHandler
    bx    r0
    .size DMA_14_IRQHandler, . - DMA_14_IRQHandler

    .align 1
    .thumb_func
    .weak DMA_15_IRQHandler
    .type DMA_15_IRQHandler, %function
DMA_15_IRQHandler:
    ldr   r0,=DMA_15_DriverIRQHandler
    bx    r0
    .size DMA_15_IRQHandler, . - DMA_15_IRQHandler

    .align 1
    .thumb_func
    .weak DMA_16_IRQHandler
    .type DMA_16_IRQHandler, %function
DMA_16_IRQHandler:
    ldr   r0,=DMA_16_DriverIRQHandler
    bx    r0
    .size DMA_16_IRQHandler, . - DMA_16_IRQHandler

    .align 1
    .thumb_func
    .weak DMA_17_IRQHandler
    .type DMA_17_IRQHandler, %function
DMA_17_IRQHandler:
    ldr   r0,=DMA_17_DriverIRQHandler
    bx    r0
    .size DMA_17_IRQHandler, . - DMA_17_IRQHandler

    .align 1
    .thumb_func
    .weak DMA_18_IRQHandler
    .type DMA_18_IRQHandler, %function
DMA_18_IRQHandler:
    ldr   r0,=DMA_18_DriverIRQHandler
    bx    r0
    .size DMA_18_IRQHandler, . - DMA_18_IRQHandler

    .align 1
    .thumb_func
    .weak DMA_19_IRQHandler
    .type DMA_19_IRQHandler, %function
DMA_19_IRQHandler:
    ldr   r0,=DMA_19_DriverIRQHandler
    bx    r0
    .size DMA_19_IRQHandler, . - DMA_19_IRQHandler

    .align 1
    .thumb_func
    .weak DMA_20_IRQHandler
    .type DMA_20_IRQHandler, %function
DMA_20_IRQHandler:
    ldr   r0,=DMA_20_DriverIRQHandler
    bx    r0
    .size DMA_20_IRQHandler, . - DMA_20_IRQHandler

    .align 1
    .thumb_func
    .weak DMA_21_IRQHandler
    .type DMA_21_IRQHandler, %function
DMA_21_IRQHandler:
    ldr   r0,=DMA_21_DriverIRQHandler
    bx    r0
    .size DMA_21_IRQHandler, . - DMA_21_IRQHandler

    .align 1
    .thumb_func
    .weak DMA_22_IRQHandler
    .type DMA_22_IRQHandler, %function
DMA_22_IRQHandler:
    ldr   r0,=DMA_22_DriverIRQHandler
    bx    r0
    .size DMA_22_IRQHandler, . - DMA_22_IRQHandler

    .align 1
    .thumb_func
    .weak DMA_23_IRQHandler
    .type DMA_23_IRQHandler, %function
DMA_23_IRQHandler:
    ldr   r0,=DMA_23_DriverIRQHandler
    bx    r0
    .size DMA_23_IRQHandler, . - DMA_23_IRQHandler

    .align 1
    .thumb_func
    .weak DMA_24_IRQHandler
    .type DMA_24_IRQHandler, %function
DMA_24_IRQHandler:
    ldr   r0,=DMA_24_DriverIRQHandler
    bx    r0
    .size DMA_24_IRQHandler, . - DMA_24_IRQHandler

    .align 1
    .thumb_func
    .weak DMA_25_IRQHandler
    .type DMA_25_IRQHandler, %function
DMA_25_IRQHandler:
    ldr   r0,=DMA_25_DriverIRQHandler
    bx    r0
    .size DMA_25_IRQHandler, . - DMA_25_IRQHandler

    .align 1
    .thumb_func
    .weak DMA_26_IRQHandler
    .type DMA_26_IRQHandler, %function
DMA_26_IRQHandler:
    ldr   r0,=DMA_26_DriverIRQHandler
    bx    r0
    .size DMA_26_IRQHandler, . - DMA_26_IRQHandler

    .align 1
    .thumb_func
    .weak DMA_27_IRQHandler
    .type DMA_27_IRQHandler, %function
DMA_27_IRQHandler:
    ldr   r0,=DMA_27_DriverIRQHandler
    bx    r0
    .size DMA_27_IRQHandler, . - DMA_27_IRQHandler

    .align 1
    .thumb_func
    .weak DMA_28_IRQHandler
    .type DMA_28_IRQHandler, %function
DMA_28_IRQHandler:
    ldr   r0,=DMA_28_DriverIRQHandler
    bx    r0
    .size DMA_28_IRQHandler, . - DMA_28_IRQHandler

    .align 1
    .thumb_func
    .weak DMA_29_IRQHandler
    .type DMA_29_IRQHandler, %function
DMA_29_IRQHandler:
    ldr   r0,=DMA_29_DriverIRQHandler
    bx    r0
    .size DMA_29_IRQHandler, . - DMA_29_IRQHandler

    .align 1
    .thumb_func
    .weak DMA_30_IRQHandler
    .type DMA_30_IRQHandler, %function
DMA_30_IRQHandler:
    ldr   r0,=DMA_30_DriverIRQHandler
    bx    r0
    .size DMA_30_IRQHandler, . - DMA_30_IRQHandler

    .align 1
    .thumb_func
    .weak DMA_31_IRQHandler
    .type DMA_31_IRQHandler, %function
DMA_31_IRQHandler:
    ldr   r0,=DMA_31_DriverIRQHandler
    bx    r0
    .size DMA_31_IRQHandler, . - DMA_31_IRQHandler

    .align 1
    .thumb_func
    .weak DMA4_ERROR_IRQHandler
    .type DMA4_ERROR_IRQHandler, %function
DMA4_ERROR_IRQHandler:
    ldr   r0,=DMA4_ERROR_DriverIRQHandler
    bx    r0
    .size DMA4_ERROR_IRQHandler, . - DMA4_ERROR_IRQHandler

    .align 1
    .thumb_func
    .weak DMA4_0_1_IRQHandler
    .type DMA4_0_1_IRQHandler, %function
DMA4_0_1_IRQHandler:
    ldr   r0,=DMA4_0_1_DriverIRQHandler
    bx    r0
    .size DMA4_0_1_IRQHandler, . - DMA4_0_1_IRQHandler

    .align 1
    .thumb_func
    .weak DMA4_2_3_IRQHandler
    .type DMA4_2_3_IRQHandler, %function
DMA4_2_3_IRQHandler:
    ldr   r0,=DMA4_2_3_DriverIRQHandler
    bx    r0
    .size DMA4_2_3_IRQHandler, . - DMA4_2_3_IRQHandler

    .align 1
    .thumb_func
    .weak DMA4_4_5_IRQHandler
    .type DMA4_4_5_IRQHandler, %function
DMA4_4_5_IRQHandler:
    ldr   r0,=DMA4_4_5_DriverIRQHandler
    bx    r0
    .size DMA4_4_5_IRQHandler, . - DMA4_4_5_IRQHandler

    .align 1
    .thumb_func
    .weak DMA4_6_7_IRQHandler
    .type DMA4_6_7_IRQHandler, %function
DMA4_6_7_IRQHandler:
    ldr   r0,=DMA4_6_7_DriverIRQHandler
    bx    r0
    .size DMA4_6_7_IRQHandler, . - DMA4_6_7_IRQHandler

    .align 1
    .thumb_func
    .weak DMA4_8_9_IRQHandler
    .type DMA4_8_9_IRQHandler, %function
DMA4_8_9_IRQHandler:
    ldr   r0,=DMA4_8_9_DriverIRQHandler
    bx    r0
    .size DMA4_8_9_IRQHandler, . - DMA4_8_9_IRQHandler

    .align 1
    .thumb_func
    .weak DMA4_10_11_IRQHandler
    .type DMA4_10_11_IRQHandler, %function
DMA4_10_11_IRQHandler:
    ldr   r0,=DMA4_10_11_DriverIRQHandler
    bx    r0
    .size DMA4_10_11_IRQHandler, . - DMA4_10_11_IRQHandler

    .align 1
    .thumb_func
    .weak DMA4_12_13_IRQHandler
    .type DMA4_12_13_IRQHandler, %function
DMA4_12_13_IRQHandler:
    ldr   r0,=DMA4_12_13_DriverIRQHandler
    bx    r0
    .size DMA4_12_13_IRQHandler, . - DMA4_12_13_IRQHandler

    .align 1
    .thumb_func
    .weak DMA4_14_15_IRQHandler
    .type DMA4_14_15_IRQHandler, %function
DMA4_14_15_IRQHandler:
    ldr   r0,=DMA4_14_15_DriverIRQHandler
    bx    r0
    .size DMA4_14_15_IRQHandler, . - DMA4_14_15_IRQHandler

    .align 1
    .thumb_func
    .weak DMA4_16_17_IRQHandler
    .type DMA4_16_17_IRQHandler, %function
DMA4_16_17_IRQHandler:
    ldr   r0,=DMA4_16_17_DriverIRQHandler
    bx    r0
    .size DMA4_16_17_IRQHandler, . - DMA4_16_17_IRQHandler

    .align 1
    .thumb_func
    .weak DMA4_18_19_IRQHandler
    .type DMA4_18_19_IRQHandler, %function
DMA4_18_19_IRQHandler:
    ldr   r0,=DMA4_18_19_DriverIRQHandler
    bx    r0
    .size DMA4_18_19_IRQHandler, . - DMA4_18_19_IRQHandler

    .align 1
    .thumb_func
    .weak DMA4_20_21_IRQHandler
    .type DMA4_20_21_IRQHandler, %function
DMA4_20_21_IRQHandler:
    ldr   r0,=DMA4_20_21_DriverIRQHandler
    bx    r0
    .size DMA4_20_21_IRQHandler, . - DMA4_20_21_IRQHandler

    .align 1
    .thumb_func
    .weak DMA4_22_23_IRQHandler
    .type DMA4_22_23_IRQHandler, %function
DMA4_22_23_IRQHandler:
    ldr   r0,=DMA4_22_23_DriverIRQHandler
    bx    r0
    .size DMA4_22_23_IRQHandler, . - DMA4_22_23_IRQHandler

    .align 1
    .thumb_func
    .weak DMA4_24_25_IRQHandler
    .type DMA4_24_25_IRQHandler, %function
DMA4_24_25_IRQHandler:
    ldr   r0,=DMA4_24_25_DriverIRQHandler
    bx    r0
    .size DMA4_24_25_IRQHandler, . - DMA4_24_25_IRQHandler

    .align 1
    .thumb_func
    .weak DMA4_26_27_IRQHandler
    .type DMA4_26_27_IRQHandler, %function
DMA4_26_27_IRQHandler:
    ldr   r0,=DMA4_26_27_DriverIRQHandler
    bx    r0
    .size DMA4_26_27_IRQHandler, . - DMA4_26_27_IRQHandler

    .align 1
    .thumb_func
    .weak DMA4_28_29_IRQHandler
    .type DMA4_28_29_IRQHandler, %function
DMA4_28_29_IRQHandler:
    ldr   r0,=DMA4_28_29_DriverIRQHandler
    bx    r0
    .size DMA4_28_29_IRQHandler, . - DMA4_28_29_IRQHandler

    .align 1
    .thumb_func
    .weak DMA4_30_31_IRQHandler
    .type DMA4_30_31_IRQHandler, %function
DMA4_30_31_IRQHandler:
    ldr   r0,=DMA4_30_31_DriverIRQHandler
    bx    r0
    .size DMA4_30_31_IRQHandler, . - DMA4_30_31_IRQHandler

    .align 1
    .thumb_func
    .weak LPI2C5_IRQHandler
    .type LPI2C5_IRQHandler, %function
LPI2C5_IRQHandler:
    ldr   r0,=LPI2C5_DriverIRQHandler
    bx    r0
    .size LPI2C5_IRQHandler, . - LPI2C5_IRQHandler

    .align 1
    .thumb_func
    .weak LPI2C6_IRQHandler
    .type LPI2C6_IRQHandler, %function
LPI2C6_IRQHandler:
    ldr   r0,=LPI2C6_DriverIRQHandler
    bx    r0
    .size LPI2C6_IRQHandler, . - LPI2C6_IRQHandler

    .align 1
    .thumb_func
    .weak SAI4_IRQHandler
    .type SAI4_IRQHandler, %function
SAI4_IRQHandler:
    ldr   r0,=SAI4_DriverIRQHandler
    bx    r0
    .size SAI4_IRQHandler, . - SAI4_IRQHandler

    .align 1
    .thumb_func
    .weak SPDIF_IRQHandler
    .type SPDIF_IRQHandler, %function
SPDIF_IRQHandler:
    ldr   r0,=SPDIF_DriverIRQHandler
    bx    r0
    .size SPDIF_IRQHandler, . - SPDIF_IRQHandler

    .align 1
    .thumb_func
    .weak LPUART9_IRQHandler
    .type LPUART9_IRQHandler, %function
LPUART9_IRQHandler:
    ldr   r0,=LPUART9_DriverIRQHandler
    bx    r0
    .size LPUART9_IRQHandler, . - LPUART9_IRQHandler

    .align 1
    .thumb_func
    .weak LPUART10_IRQHandler
    .type LPUART10_IRQHandler, %function
LPUART10_IRQHandler:
    ldr   r0,=LPUART10_DriverIRQHandler
    bx    r0
    .size LPUART10_IRQHandler, . - LPUART10_IRQHandler

    .align 1
    .thumb_func
    .weak LPUART11_IRQHandler
    .type LPUART11_IRQHandler, %function
LPUART11_IRQHandler:
    ldr   r0,=LPUART11_DriverIRQHandler
    bx    r0
    .size LPUART11_IRQHandler, . - LPUART11_IRQHandler

    .align 1
    .thumb_func
    .weak LPUART12_IRQHandler
    .type LPUART12_IRQHandler, %function
LPUART12_IRQHandler:
    ldr   r0,=LPUART12_DriverIRQHandler
    bx    r0
    .size LPUART12_IRQHandler, . - LPUART12_IRQHandler

    .align 1
    .thumb_func
    .weak CAN3_IRQHandler
    .type CAN3_IRQHandler, %function
CAN3_IRQHandler:
    ldr   r0,=CAN3_DriverIRQHandler
    bx    r0
    .size CAN3_IRQHandler, . - CAN3_IRQHandler

    .align 1
    .thumb_func
    .weak CAN3_ERROR_IRQHandler
    .type CAN3_ERROR_IRQHandler, %function
CAN3_ERROR_IRQHandler:
    ldr   r0,=CAN3_ERROR_DriverIRQHandler
    bx    r0
    .size CAN3_ERROR_IRQHandler, . - CAN3_ERROR_IRQHandler

    .align 1
    .thumb_func
    .weak LPSPI5_IRQHandler
    .type LPSPI5_IRQHandler, %function
LPSPI5_IRQHandler:
    ldr   r0,=LPSPI5_DriverIRQHandler
    bx    r0
    .size LPSPI5_IRQHandler, . - LPSPI5_IRQHandler

    .align 1
    .thumb_func
    .weak LPSPI6_IRQHandler
    .type LPSPI6_IRQHandler, %function
LPSPI6_IRQHandler:
    ldr   r0,=LPSPI6_DriverIRQHandler
    bx    r0
    .size LPSPI6_IRQHandler, . - LPSPI6_IRQHandler

    .align 1
    .thumb_func
    .weak LPUART7_IRQHandler
    .type LPUART7_IRQHandler, %function
LPUART7_IRQHandler:
    ldr   r0,=LPUART7_DriverIRQHandler
    bx    r0
    .size LPUART7_IRQHandler, . - LPUART7_IRQHandler

    .align 1
    .thumb_func
    .weak LPUART8_IRQHandler
    .type LPUART8_IRQHandler, %function
LPUART8_IRQHandler:
    ldr   r0,=LPUART8_DriverIRQHandler
    bx    r0
    .size LPUART8_IRQHandler, . - LPUART8_IRQHandler

    .align 1
    .thumb_func
    .weak SAI2_IRQHandler
    .type SAI2_IRQHandler, %function
SAI2_IRQHandler:
    ldr   r0,=SAI2_DriverIRQHandler
    bx    r0
    .size SAI2_IRQHandler, . - SAI2_IRQHandler

    .align 1
    .thumb_func
    .weak SAI3_IRQHandler
    .type SAI3_IRQHandler, %function
SAI3_IRQHandler:
    ldr   r0,=SAI3_DriverIRQHandler
    bx    r0
    .size SAI3_IRQHandler, . - SAI3_IRQHandler

    .align 1
    .thumb_func
    .weak XSPI_SLV_IRQHandler
    .type XSPI_SLV_IRQHandler, %function
XSPI_SLV_IRQHandler:
    ldr   r0,=XSPI_SLV_DriverIRQHandler
    bx    r0
    .size XSPI_SLV_IRQHandler, . - XSPI_SLV_IRQHandler


/*    Macro to define default handlers. Default handler
 *    will be weak symbol and just dead loops. They can be
 *    overwritten by other handlers */
    .macro def_irq_handler  handler_name
    .weak \handler_name
    .set  \handler_name, DefaultISR
    .endm

/* Exception Handlers */
    def_irq_handler    DebugMon_Handler
    def_irq_handler    TMR1_IRQHandler
    def_irq_handler    DAP_IRQHandler
    def_irq_handler    M7_CTI_TRIGGER_OUTPUT_IRQHandler
    def_irq_handler    M33_CTI_TRIGGER_OUTPUT_IRQHandler
    def_irq_handler    TMR5_IRQHandler
    def_irq_handler    TMR6_IRQHandler
    def_irq_handler    TMR7_IRQHandler
    def_irq_handler    TMR8_IRQHandler
    def_irq_handler    CAN1_DriverIRQHandler
    def_irq_handler    CAN1_ERROR_DriverIRQHandler
    def_irq_handler    GPIO1_0_IRQHandler
    def_irq_handler    GPIO1_1_IRQHandler
    def_irq_handler    I3C1_DriverIRQHandler
    def_irq_handler    LPI2C1_DriverIRQHandler
    def_irq_handler    LPI2C2_DriverIRQHandler
    def_irq_handler    LPIT1_IRQHandler
    def_irq_handler    LPSPI1_DriverIRQHandler
    def_irq_handler    LPSPI2_DriverIRQHandler
    def_irq_handler    LPTMR1_IRQHandler
    def_irq_handler    LPUART1_DriverIRQHandler
    def_irq_handler    LPUART2_DriverIRQHandler
    def_irq_handler    MU1_IRQHandler
    def_irq_handler    MU2_IRQHandler
    def_irq_handler    PWM1_FAULT_IRQHandler
    def_irq_handler    PWM1_0_IRQHandler
    def_irq_handler    PWM1_1_IRQHandler
    def_irq_handler    PWM1_2_IRQHandler
    def_irq_handler    PWM1_3_IRQHandler
    def_irq_handler    SENTINEL_TRUST_MUA_RX_FULL_IRQHandler
    def_irq_handler    SENTINEL_TRUST_MUA_TX_EMPTY_IRQHandler
    def_irq_handler    SENTINEL_APPS_CORE_MUA_RX_FULL_IRQHandler
    def_irq_handler    SENTINEL_APPS_CORE_MUA_TX_EMPTY_IRQHandler
    def_irq_handler    SENTINEL_REALTIME_CORE_MUA_RX_FULL_IRQHandler
    def_irq_handler    SENTINEL_REALTIME_CORE_MUA_TX_EMPTY_IRQHandler
    def_irq_handler    SENTINEL_SECURE_IRQHandler
    def_irq_handler    SENTINEL_NONSECURE_IRQHandler
    def_irq_handler    TPM1_IRQHandler
    def_irq_handler    TPM2_IRQHandler
    def_irq_handler    RTWDOG1_IRQHandler
    def_irq_handler    RTWDOG2_IRQHandler
    def_irq_handler    Reserved56_IRQHandler
    def_irq_handler    PDM_HWVAD_EVENT_DriverIRQHandler
    def_irq_handler    PDM_HWVAD_ERROR_DriverIRQHandler
    def_irq_handler    PDM_EVENT_DriverIRQHandler
    def_irq_handler    PDM_ERROR_DriverIRQHandler
    def_irq_handler    SAI1_DriverIRQHandler
    def_irq_handler    M33_PS_IRQHandler
    def_irq_handler    M33_TCM_ECC_IRQHandler
    def_irq_handler    M33_TCM_ERROR_IRQHandler
    def_irq_handler    M7_TCM_ECC_IRQHandler
    def_irq_handler    M7_TCM_ERROR_IRQHandler
    def_irq_handler    CAN2_DriverIRQHandler
    def_irq_handler    CAN2_ERROR_DriverIRQHandler
    def_irq_handler    FLEXIO1_DriverIRQHandler
    def_irq_handler    FLEXIO2_DriverIRQHandler
    def_irq_handler    FLEXSPI1_DriverIRQHandler
    def_irq_handler    FLEXSPI2_DriverIRQHandler
    def_irq_handler    GPIO2_0_IRQHandler
    def_irq_handler    GPIO2_1_IRQHandler
    def_irq_handler    GPIO3_0_IRQHandler
    def_irq_handler    GPIO3_1_IRQHandler
    def_irq_handler    I3C2_DriverIRQHandler
    def_irq_handler    LPI2C3_DriverIRQHandler
    def_irq_handler    LPI2C4_DriverIRQHandler
    def_irq_handler    LPIT2_IRQHandler
    def_irq_handler    LPSPI3_DriverIRQHandler
    def_irq_handler    LPSPI4_DriverIRQHandler
    def_irq_handler    LPTMR2_IRQHandler
    def_irq_handler    LPUART3_DriverIRQHandler
    def_irq_handler    LPUART4_DriverIRQHandler
    def_irq_handler    LPUART5_DriverIRQHandler
    def_irq_handler    LPUART6_DriverIRQHandler
    def_irq_handler    ASRC_DriverIRQHandler
    def_irq_handler    BBNSM_IRQHandler
    def_irq_handler    SYS_CTRL1_IRQHandler
    def_irq_handler    TPM3_IRQHandler
    def_irq_handler    TPM4_IRQHandler
    def_irq_handler    TPM5_IRQHandler
    def_irq_handler    TPM6_IRQHandler
    def_irq_handler    RTWDOG3_IRQHandler
    def_irq_handler    RTWDOG4_IRQHandler
    def_irq_handler    RTWDOG5_IRQHandler
    def_irq_handler    Reserved98_IRQHandler
    def_irq_handler    TEMPSENSOR_IRQHandler
    def_irq_handler    Reserved100_IRQHandler
    def_irq_handler    LDO_1P8_IRQHandler
    def_irq_handler    USDHC1_DriverIRQHandler
    def_irq_handler    USDHC2_DriverIRQHandler
    def_irq_handler    Reserved104_IRQHandler
    def_irq_handler    TMR2_IRQHandler
    def_irq_handler    LDO_1P0_IRQHandler
    def_irq_handler    MECC1_IRQHandler
    def_irq_handler    MECC2_IRQHandler
    def_irq_handler    ADC1_IRQHandler
    def_irq_handler    DMA_ERROR_DriverIRQHandler
    def_irq_handler    DMA_0_DriverIRQHandler
    def_irq_handler    DMA_1_DriverIRQHandler
    def_irq_handler    DMA_2_DriverIRQHandler
    def_irq_handler    DMA_3_DriverIRQHandler
    def_irq_handler    DMA_4_DriverIRQHandler
    def_irq_handler    DMA_5_DriverIRQHandler
    def_irq_handler    DMA_6_DriverIRQHandler
    def_irq_handler    DMA_7_DriverIRQHandler
    def_irq_handler    DMA_8_DriverIRQHandler
    def_irq_handler    DMA_9_DriverIRQHandler
    def_irq_handler    DMA_10_DriverIRQHandler
    def_irq_handler    DMA_11_DriverIRQHandler
    def_irq_handler    DMA_12_DriverIRQHandler
    def_irq_handler    DMA_13_DriverIRQHandler
    def_irq_handler    DMA_14_DriverIRQHandler
    def_irq_handler    DMA_15_DriverIRQHandler
    def_irq_handler    DMA_16_DriverIRQHandler
    def_irq_handler    DMA_17_DriverIRQHandler
    def_irq_handler    DMA_18_DriverIRQHandler
    def_irq_handler    DMA_19_DriverIRQHandler
    def_irq_handler    DMA_20_DriverIRQHandler
    def_irq_handler    DMA_21_DriverIRQHandler
    def_irq_handler    DMA_22_DriverIRQHandler
    def_irq_handler    DMA_23_DriverIRQHandler
    def_irq_handler    DMA_24_DriverIRQHandler
    def_irq_handler    DMA_25_DriverIRQHandler
    def_irq_handler    DMA_26_DriverIRQHandler
    def_irq_handler    DMA_27_DriverIRQHandler
    def_irq_handler    DMA_28_DriverIRQHandler
    def_irq_handler    DMA_29_DriverIRQHandler
    def_irq_handler    DMA_30_DriverIRQHandler
    def_irq_handler    DMA_31_DriverIRQHandler
    def_irq_handler    DMA4_ERROR_DriverIRQHandler
    def_irq_handler    DMA4_0_1_DriverIRQHandler
    def_irq_handler    DMA4_2_3_DriverIRQHandler
    def_irq_handler    DMA4_4_5_DriverIRQHandler
    def_irq_handler    DMA4_6_7_DriverIRQHandler
    def_irq_handler    DMA4_8_9_DriverIRQHandler
    def_irq_handler    DMA4_10_11_DriverIRQHandler
    def_irq_handler    DMA4_12_13_DriverIRQHandler
    def_irq_handler    DMA4_14_15_DriverIRQHandler
    def_irq_handler    DMA4_16_17_DriverIRQHandler
    def_irq_handler    DMA4_18_19_DriverIRQHandler
    def_irq_handler    DMA4_20_21_DriverIRQHandler
    def_irq_handler    DMA4_22_23_DriverIRQHandler
    def_irq_handler    DMA4_24_25_DriverIRQHandler
    def_irq_handler    DMA4_26_27_DriverIRQHandler
    def_irq_handler    DMA4_28_29_DriverIRQHandler
    def_irq_handler    DMA4_30_31_DriverIRQHandler
    def_irq_handler    Reserved160_IRQHandler
    def_irq_handler    Reserved161_IRQHandler
    def_irq_handler    Reserved162_IRQHandler
    def_irq_handler    Reserved163_IRQHandler
    def_irq_handler    Reserved164_IRQHandler
    def_irq_handler    Reserved165_IRQHandler
    def_irq_handler    Reserved166_IRQHandler
    def_irq_handler    Reserved167_IRQHandler
    def_irq_handler    LPI2C5_DriverIRQHandler
    def_irq_handler    LPI2C6_DriverIRQHandler
    def_irq_handler    SAI4_DriverIRQHandler
    def_irq_handler    SPDIF_DriverIRQHandler
    def_irq_handler    LPUART9_DriverIRQHandler
    def_irq_handler    LPUART10_DriverIRQHandler
    def_irq_handler    LPUART11_DriverIRQHandler
    def_irq_handler    LPUART12_DriverIRQHandler
    def_irq_handler    Reserved176_IRQHandler
    def_irq_handler    Reserved177_IRQHandler
    def_irq_handler    Reserved178_IRQHandler
    def_irq_handler    Reserved179_IRQHandler
    def_irq_handler    TMR3_IRQHandler
    def_irq_handler    TMR4_IRQHandler
    def_irq_handler    M33_SYSRESET_REQ_IRQHandler
    def_irq_handler    M33_LOCKUP_IRQHandler
    def_irq_handler    M7_SYSRESET_REQ_IRQHandler
    def_irq_handler    M7_LOCKUP_IRQHandler
    def_irq_handler    PWM2_FAULT_IRQHandler
    def_irq_handler    PWM2_0_IRQHandler
    def_irq_handler    PWM2_1_IRQHandler
    def_irq_handler    PWM2_2_IRQHandler
    def_irq_handler    PWM2_3_IRQHandler
    def_irq_handler    PWM3_FAULT_IRQHandler
    def_irq_handler    PWM3_0_IRQHandler
    def_irq_handler    PWM3_1_IRQHandler
    def_irq_handler    PWM3_2_IRQHandler
    def_irq_handler    PWM3_3_IRQHandler
    def_irq_handler    PWM4_FAULT_IRQHandler
    def_irq_handler    PWM4_0_IRQHandler
    def_irq_handler    PWM4_1_IRQHandler
    def_irq_handler    PWM4_2_IRQHandler
    def_irq_handler    PWM4_3_IRQHandler
    def_irq_handler    EQDC1_IRQHandler
    def_irq_handler    EQDC2_IRQHandler
    def_irq_handler    EQDC3_IRQHandler
    def_irq_handler    EQDC4_IRQHandler
    def_irq_handler    ADC2_IRQHandler
    def_irq_handler    DCDC_IRQHandler
    def_irq_handler    CAN3_DriverIRQHandler
    def_irq_handler    CAN3_ERROR_DriverIRQHandler
    def_irq_handler    DAC_IRQHandler
    def_irq_handler    LPSPI5_DriverIRQHandler
    def_irq_handler    LPSPI6_DriverIRQHandler
    def_irq_handler    LPUART7_DriverIRQHandler
    def_irq_handler    LPUART8_DriverIRQHandler
    def_irq_handler    SAI2_DriverIRQHandler
    def_irq_handler    SAI3_DriverIRQHandler
    def_irq_handler    ACMP1_IRQHandler
    def_irq_handler    ACMP2_IRQHandler
    def_irq_handler    ACMP3_IRQHandler
    def_irq_handler    ACMP4_IRQHandler
    def_irq_handler    Reserved220_IRQHandler
    def_irq_handler    Reserved221_IRQHandler
    def_irq_handler    Reserved222_IRQHandler
    def_irq_handler    Reserved223_IRQHandler
    def_irq_handler    Reserved224_IRQHandler
    def_irq_handler    GPT1_IRQHandler
    def_irq_handler    GPT2_IRQHandler
    def_irq_handler    KPP_IRQHandler
    def_irq_handler    USBPHY1_IRQHandler
    def_irq_handler    USBPHY2_IRQHandler
    def_irq_handler    USB_OTG2_IRQHandler
    def_irq_handler    USB_OTG1_IRQHandler
    def_irq_handler    XSPI_SLV_DriverIRQHandler
    def_irq_handler    NETC_IRQHandler
    def_irq_handler    MSGINTR1_IRQHandler
    def_irq_handler    MSGINTR2_IRQHandler
    def_irq_handler    MSGINTR3_IRQHandler
    def_irq_handler    MSGINTR4_IRQHandler
    def_irq_handler    MSGINTR5_IRQHandler
    def_irq_handler    MSGINTR6_IRQHandler
    def_irq_handler    SINC1_0_IRQHandler
    def_irq_handler    SINC1_1_IRQHandler
    def_irq_handler    SINC1_2_IRQHandler
    def_irq_handler    SINC1_3_IRQHandler
    def_irq_handler    SINC2_0_IRQHandler
    def_irq_handler    SINC2_1_IRQHandler
    def_irq_handler    SINC2_2_IRQHandler
    def_irq_handler    SINC2_3_IRQHandler
    def_irq_handler    GPIO4_0_IRQHandler
    def_irq_handler    GPIO4_1_IRQHandler
    def_irq_handler    GPIO5_0_IRQHandler
    def_irq_handler    GPIO5_1_IRQHandler
    def_irq_handler    GPIO6_0_IRQHandler
    def_irq_handler    GPIO6_1_IRQHandler
    def_irq_handler    ECAT_IRQHandler

    .end
