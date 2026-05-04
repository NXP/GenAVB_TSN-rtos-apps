/*
** ###################################################################
**     Processors:          MIMXRT1176AVM8A_cm7
**                          MIMXRT1176CVM8A_cm7
**                          MIMXRT1176DVMAA_cm7
**
**     Compiler:            GNU C Compiler
**     Reference manual:    IMXRT1170RM, Rev 1, 02/2021
**     Version:             rev. 1.1, 2022-04-02
**     Build:               b221022
**
**     Abstract:
**         Linker file for the GNU C Compiler
**
**     Copyright 2016 Freescale Semiconductor, Inc.
**     Copyright 2016-2022, 2025 NXP
**     All rights reserved.
**
**     SPDX-License-Identifier: BSD-3-Clause
**
**     http:                 www.nxp.com
**     mail:                 support@nxp.com
**
** ###################################################################
*/

/* Entry Point */
ENTRY(Reset_Handler)

HEAP_SIZE  = DEFINED(__heap_size__)  ? __heap_size__  : 0x1000;
STACK_SIZE = DEFINED(__stack_size__) ? __stack_size__ : 0x1000;

/* Specify the memory areas */
MEMORY
{
  flash    (RX)  : ORIGIN = 0x30000000, LENGTH = 0x04000000
  itcm     (RX)  : ORIGIN = 0x00000000, LENGTH = 0x00038000
  dtcm     (RW)  : ORIGIN = 0x20000000, LENGTH = 0x00048000
  ocram_m4 (RW)  : ORIGIN = 0x20200000, LENGTH = 0x00040000
  ocram1   (RX)  : ORIGIN = 0x20240000, LENGTH = 0x00080000
  ocram2   (RW)  : ORIGIN = 0x202C0000, LENGTH = 0x0007E000
  ocram2_nc (RW)  : ORIGIN = 0x2033E000, LENGTH = 0x00002000
  sdram_text (RX)  : ORIGIN = 0x80000000, LENGTH = 0x02000000
  sdram_data (RW)  : ORIGIN = 0x82000000, LENGTH = 0x02000000
}

boot_hdr_flash_conf = ORIGIN(flash) + 0x400;
boot_hdr_ivt = boot_hdr_flash_conf + 0xc00;
program_image = boot_hdr_ivt  + 0x1000;

/* Define output sections */
SECTIONS
{

  .flash_config boot_hdr_flash_conf :
  {
    . = ALIGN(4);
    __FLASH_BASE = .;
    KEEP(* (.boot_hdr.conf))     /* flash config section */
    . = ALIGN(4);
  } > flash

  .ivt boot_hdr_ivt :
  {
    . = ALIGN(4);
	KEEP(* (.boot_hdr.ivt))           /* ivt section */
	KEEP(* (.boot_hdr.boot_data))     /* boot section */
	KEEP(* (.boot_hdr.dcd_data))      /* dcd section */
    . = ALIGN(4);
  } > flash

  /* Interrupt table */
  .interrupts program_image :
  {
    __VECTOR_TABLE = .;
    . = ALIGN(4);
    KEEP(*(.isr_vector))
    . = ALIGN(4);
  } > flash

  .flash_text :
  {
    *pipeline_config.c.obj(*.rodata*)
  } > flash

  /* NULL pointer protection */
  .guard_area :
  {
    __guard_area_start = .;
    . += 0x400;
    __guard_area_end = .;
  } > itcm

  /* Interrupt table */
  .interrupts_ram :
  {
    . = ALIGN(4);
    __ram_isr_vector_start = .;
    . += SIZEOF(.interrupts);
    __ram_isr_vector_end = .;
    . = ALIGN(4);
  } > itcm

  /* The startup code xip from flash */
  .startup :
  {
    *startup_MIMXRT1176_cm7.S.obj(.text)
    *(.text.SystemInit*)
  } > flash

  .slow_text :
  {
    __slow_text_load = LOADADDR(.slow_text);
    . = ALIGN(4);
    __slow_text_start = .;

    /* GenAVB/TSN */
    *(.rodata.management)
    *(.rodata.gptp)
    *(.rodata.srp)
    *(.rodata.avdecc)
    *(.rodata.maap)
    *(.text.init)
    *(.text.exit)
    *(.text.management)
    *(.text.gptp)
    *(.text.srp)
    *(.text.avdecc)
    *(.text.maap)

    /* MCUX SDK 3.0 */
    *gen_avb/avdecc/*(*.text*)
    *gen_avb/gptp/*(*.text*)
    *gen_avb/maap/*(*.text*)
    *gen_avb/management/*(*.text*)
    *gen_avb/srp/*(*.text*)
    *gen_avb/apps/*/aem-manager/*.c.obj(*.text*)

    /* MCUX SDK */
    *components/codec/*.c.obj(*.text*)
    *components/gpio/*.c.obj(*.text*)
    *components/i2c/*.c.obj(*.text*)
    *components/phy/*.c.obj(*.text*)
    *drivers/fsl_flexspi.c.obj(*.text*)
    *drivers/fsl_lpi2c.c.obj(*.text*)
    *drivers/fsl_sai.c.obj(*.text*)
    *middleware/littlefs/*.c.obj(*.text*)
    *middleware/lwip/*.c.obj(*.text*)
    *utilities/shell/*.c.obj(*.text*)

    /* MCUX SDK 3.0 */
    *components/shell/*.c.obj(*.text*)
    *drivers/flexspi/fsl_flexspi.c.obj(*.text*)
    *drivers/lpi2c/fsl_lpi2c.c.obj(*.text*)
    *drivers/sai/fsl_sai.c.obj(*.text*)

    /* RTOS APPS */
    *audio/audio_avb.c.obj(*.text*)

    /* Application Common */
    *common/flexspi_nor_flash_ops.c.obj(*.text*)
    *common/lfs_qspi_nor.c.obj(*.text*)
    *common/log.c.obj(*.text*)
    *common/lwip/lwip*.c.obj(*.text*)
    *common/shell.c.obj(*.text*)
    *common/storage.c.obj(*.text*)

    /* Application Audio */
    */audio_app/*.c.obj(*.text*)

    /* MCUX SDK 3.0 */
    /* Application Audio */
    */audio_app.c.obj(*.text*)
    */audio_shell.c.obj(*.text*)
    */configs.c.obj(*.text*)
    */pipeline_config.c.obj(*.text*)

    /* Application Audio AVB */
    *avb_audio_app/avb_stream.c.obj(*.text*)
    *avb_audio_app/clock_domain.c.obj(*.text*)
    *avb_audio_app/crf_stream.c.obj(*.text*)
    *avb_audio_app/msrp.c.obj(*.text*)
    *avb_audio_app/shell.c.obj(*.text*)
    *media_stack/codec.c.obj(*.text*)
    *media_stack/fsl_wm8962.c.obj(*.text*)
    *media_stack/i2c.c.obj(*.text*)
    *media_stack/sai.c.obj(*.text*)

    /* Application TSN */
    *tsn_app/shell.c.obj(*.text*)

    . = ALIGN(4);
    __slow_text_end = .;
  } > ocram1 AT> flash

  .slow_data :
  {
    __slow_data_load = LOADADDR(.slow_data);
    . = ALIGN(4);
    __slow_data_start = .;
    *(__slow_data)
    *(.rodata.str*)              /* .rodata string sections */

    /* MCUX SDK 3.0 */
    *gen_avb/avdecc/*(*.data* .rodata*)
    *gen_avb/gptp/*(*.data* .rodata*)
    *gen_avb/maap/*(*.data* .rodata*)
    *gen_avb/management/*(*.data* .rodata*)
    *gen_avb/srp/*(*.data* .rodata*)

    /* MCUX SDK */
    *drivers/fsl_flexspi.c.obj(*.data* *.rodata*)
    *middleware/littlefs/*.c.obj(*.data* *.rodata*)
    *middleware/lwip/*.c.obj(*.data* *.rodata*)
    *utilities/shell/*.c.obj(*.data* *.rodata*)

    /* MCUX SDK 3.0 */
    *components/shell/fsl_shell.c.obj(*.data* *.rodata*)
    *drivers/flexspi/fsl_flexspi.c.obj(*.data* *.rodata*)

    /* RTOS APPS */
    */tsn/tsn_task_config.c.obj(*.data* *.rodata*)

    /* Application Common */
    *common/flexspi_nor_flash_ops.c.obj(*.data* *.rodata*)
    *common/lfs_qspi_nor.c.obj(*.data* *.rodata*)
    *common/shell.c.obj(*.data* *.rodata*)
    *common/storage.c.obj(*.data* *.rodata*)
    *common/system_config.c.obj(*.data* *.rodata*)

    /* Application Audio */
    */audio_app/*.c.obj(*.data* *.rodata*)

    /* MCUX SDK 3.0 */
    /* Application Audio */
    */audio_app.c.obj(*.data* .rodata*)
    */audio_shell.c.obj(*.data* .rodata*)
    */configs.c.obj(*.data* .rodata*)
    */pipeline_config.c.obj(*.data* .rodata*)

    /* Application Audio AVB */
    *avb_audio_app/audio_config.c.obj(*.data* *.rodata*)
    *avb_audio_app/avb_stream_config.c.obj(*.data* *.rodata*)
    *avb_audio_app/avb_stream.c.obj(*.data* *.rodata*)
    *avb_audio_app/configs.c.obj(*.data* *.rodata*)
    *avb_audio_app/task_config.c.obj(*.data* *.rodata*)

    /* Application TSN */
    *tsn_app/configs.c.obj(*.data* *.rodata*)

    . = ALIGN(4);
    __slow_data_end = .;
  } > ocram2 AT> flash

  .slow_bss (NOLOAD) :
  {
    . = ALIGN(4);
    __slow_bss_start = .;
    *(__slow_bss)

    /* MCUX SDK */
    *drivers/fsl_flexspi.c.obj(*.bss*)
    *middleware/littlefs/*.c.obj(*.bss*)
    *middleware/lwip/*.c.obj(*.bss*)
    *utilities/debug_console/*.c.obj(*.bss*)
    *utilities/shell/*.c.obj(*.bss*)

    /* MCUX SDK 3.0 */
    *drivers/flexspi/fsl_flexspi.c.obj(*.bss*)
    *components/debug_console/*.c.obj(*.bss*)
    *components/shell/*.c.obj(*.bss*)

    /* Application Common */
    *common/flexspi_nor_flash_ops.c.obj(*.bss*)
    *common/lfs_qspi_nor.c.obj(*.bss*)
    *common/shell.c.obj(*.bss*)
    *common/storage.c.obj(*.bss*)

    . = ALIGN(4);
    __slow_bss_end = .;
  } > ocram2

  /* Code and other data go into internal RAM */
  .text :
  {
    __text_load = LOADADDR(.text);
    . = ALIGN(4);
    __text_start = .;
    *(.isr_handlers)
    *(.text)                 /* .text sections (code) */
    *(.text*)                /* .text* sections (code) */
    *(.rodata)               /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)              /* .rodata* sections (constants, strings, etc.) */
    *(.glue_7)               /* glue arm to thumb code */
    *(.glue_7t)              /* glue thumb to arm code */
    *(.eh_frame)
    KEEP (*(.init))
    KEEP (*(.fini))
    . = ALIGN(4);
  } > itcm AT> flash

  .preinit_array :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } > itcm AT> flash

  .init_array :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
  } > itcm AT> flash

  .fini_array :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } > itcm AT> flash

  .ARM.extab :
  {
    *(.ARM.extab* .gnu.linkonce.armextab.*)
  } > itcm AT> flash

  .ARM :
  {
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
  __text_end = .;
  } > itcm AT> flash

  .data :
  {
    __data_load__ = LOADADDR(.data);
    . = ALIGN(4);
    __data_start__ = .;      /* create a global symbol at data start */
    *(m_usb_dma_init_data)
    *(.data)                 /* .data sections */
    *(.data*)                /* .data* sections */
    KEEP(*(.jcr*))
    . = ALIGN(4);
    __data_end__ = .;        /* define a global symbol at data end */
  } > dtcm AT> flash

  .ncache.init :
  {
    __noncachedata_start__ = .;   /* create a global symbol at ncache data start */
    *(NonCacheable.init)
    . = ALIGN(4);
    __noncachedata_init_end__ = .;   /* create a global symbol at initialized ncache data end */
  } > ocram2_nc AT> flash

  . = __noncachedata_init_end__;
  .ncache :
  {
    *(NonCacheable)
    . = ALIGN(4);
    __noncachedata_end__ = .;     /* define a global symbol at ncache data end */
  } > ocram2_nc AT> flash

  __NCACHE_REGION_START = ORIGIN(ocram2_nc);
  __NCACHE_REGION_SIZE = LENGTH(ocram2_nc);

  /* Uninitialized data section */
  .bss :
  {
    /* This is used by the startup in order to initialize the .bss section */
    . = ALIGN(4);
    __START_BSS = .;
    *(m_usb_dma_noninit_data)
    *(.bss)
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    __END_BSS = .;

    /* Prevent bss zeroing in libc init (crt0.S) */
    __bss_start__ = 0;
    __bss_end__ = 0;
  } > dtcm

  .heap :
  {
    . = ALIGN(8);
    __end__ = .;
    PROVIDE(end = .);
    __HeapBase = .;
    . += HEAP_SIZE;
    __HeapLimit = .;
    __heap_limit = .; /* Add for _sbrk */
  } > dtcm

  .ucHeap :
  {
    . = ALIGN(8);
    __ucHeapBase__ = .;
    PROVIDE(ucHeap = .);
    . = ORIGIN(dtcm) + LENGTH(dtcm) - STACK_SIZE;
  } > dtcm

  __ucHeapSize = . - __ucHeapBase__;

  .stack :
  {
    . = ALIGN(8);
    . += STACK_SIZE;
  } > dtcm

  /* Initializes stack on the end of block */
  __StackTop   = ORIGIN(dtcm) + LENGTH(dtcm);
  __StackLimit = __StackTop - STACK_SIZE;
  PROVIDE(__stack = __StackTop);

  .ARM.attributes 0 : { *(.ARM.attributes) }

  ASSERT(__StackLimit >= __HeapLimit, "region dtcm overflowed with stack and heap")
}

