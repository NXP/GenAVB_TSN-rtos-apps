/*
** ###################################################################
**     Processors:          MIMXRT1052CVL5A
**                          MIMXRT1052DVL6A
**
**     Compiler:            GNU C Compiler
**     Reference manual:    IMXRT1050RM Rev.C, 08/2017
**     Version:             rev. 0.1, 2017-01-10
**     Build:               b171208
**
**     Abstract:
**         Linker file for the GNU C Compiler
**
**     The Clear BSD License
**     Copyright 2016 Freescale Semiconductor, Inc.
**     Copyright 2016-2018, 2025 NXP
**     All rights reserved.
**
**     Redistribution and use in source and binary forms, with or without
**     modification, are permitted (subject to the limitations in the
**     disclaimer below) provided that the following conditions are met:
**
**     * Redistributions of source code must retain the above copyright
**       notice, this list of conditions and the following disclaimer.
**
**     * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in the
**       documentation and/or other materials provided with the distribution.
**
**     * Neither the name of the copyright holder nor the names of its
**       contributors may be used to endorse or promote products derived from
**       this software without specific prior written permission.
**
**     NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
**     GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
**     HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
**     WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
**     MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**     DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
**     LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
**     CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
**     SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
**     BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
**     WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
**     OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
**     IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
  flash    (RX)  : ORIGIN = 0x60000000, LENGTH = 0x04000000
  itcm     (RX)  : ORIGIN = 0x00000000, LENGTH = 0x00030000
  dtcm     (RW)  : ORIGIN = 0x20000000, LENGTH = 0x00050000
  sdram_text (RX)  : ORIGIN = 0x80000000, LENGTH = 0x01000000
  sdram_data (RW)  : ORIGIN = 0x81000000, LENGTH = 0x01000000
}

boot_hdr_flash_conf = ORIGIN(flash);
boot_hdr_ivt = boot_hdr_flash_conf + 0x1000;
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
    __Vectors = .;
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
    *startup_MIMXRT1052.S.obj(.text)
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
    *gen_avb/management/*(*.text* *.rodata*)
    *gen_avb/gptp/*(*.text* *.rodata*)
    *gen_avb/srp/*(*.text* *.rodata*)
    *gen_avb/avdecc/*(*.text* *.rodata*)
    *gen_avb/maap/*(*.text* *.rodata*)
    *gen_avb/common*(*.text* *.rodata*)
    *gen_avb/apps/*/aem-manager/*.c.obj(*.text*)

    /* MCUX SDK */
    *components/codec/*.c.obj(*.text*)
    *components/gpio/*.c.obj(*.text*)
    *components/i2c/*.c.obj(*.text*)
    *components/phy/*.c.obj(*.text*)
    *components/uart/*.c.obj(*.text*)
    *drivers/fsl_flexspi.c.obj(*.text*)
    *drivers/fsl_lpi2c.c.obj(*.text*)
    *drivers/fsl_sai.c.obj(*.text*)
    *middleware/littlefs/*.c.obj(*.text*)
    *middleware/lwip/*.c.obj(*.text*)
    *utilities/debug_console/*.c.obj(*.text*)
    *utilities/shell/*.c.obj(*.text*)

    /* MCUX SDK 3.0 */
    *components/debug_console/*.c.obj(*.text*)
    *components/phy*(*.text*)
    *components/shell/*.c.obj(*.text*)
    *drivers/flexspi/fsl_flexspi.c.obj(*.text*)
    *drivers/igpio*(*.text*)
    *drivers/lpi2c/fsl_lpi2c.c.obj(*.text*)
    *drivers/sai/fsl_sai.c.obj(*.text*)

    /* RTOS APPS */
    *audio/audio_avb.c.obj(*.text*)
    *audio/*.c.obj(*.text*_init *.text*_exit *.text*_dump *.text*_check_config)

    /* Application Common */
    *common/flexspi_hyper_flash_ops.c.obj(*.text*)
    *common/genavb.c.obj(*.text*_init*)
    *common/lfs_hyperflash_nor.c.obj(*.text*)
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
    *media_stack/fsl_wm8960.c.obj(*.text*)
    *media_stack/i2c.c.obj(*.text*)
    *media_stack/sai.c.obj(*.text*)

    . = ALIGN(4);
    __slow_text_end = .;
  } > sdram_text AT> flash

  .slow_data :
  {
    __slow_data_load = LOADADDR(.slow_data);
    . = ALIGN(4);
    __slow_data_start = .;
    *(__slow_data)
    *(.rodata.str*)              /* .rodata string sections */

    /* MCUX SDK 3.0 */
    *gen_avb/management/*(*.data* *.rodata*)
    *gen_avb/gptp/*(*.data* *.rodata*)
    *gen_avb/srp/*(*.data* *.rodata*)
    *gen_avb/avdecc/*(*.data* *.rodata*)
    *gen_avb/maap/*(*.data* *.rodata*)
    *gen_avb/common*(*.data* *.rodata*)

    /* MCUX SDK */
    *components/codec/*.c.obj(*.data* *.rodata*)
    *components/gpio/*.c.obj(*.data* *.rodata*)
    *components/i2c/*.c.obj(*.data* *.rodata*)
    *components/phy/*.c.obj(*.data* *.rodata*)
    *components/uart/*.c.obj(*.data* *.rodata*)
    *drivers/fsl_flexspi.c.obj(*.data* *.rodata*)
    *drivers/fsl_lpi2c.c.obj(*.data* *.rodata*)
    *drivers/fsl_sai.c.obj(*.data* *.rodata*)
    *middleware/littlefs/*.c.obj(*.data* *.rodata*)
    *middleware/lwip/*.c.obj(*.data* *.rodata*)
    *utilities/debug_console/*.c.obj(*.data* *.rodata*)
    *utilities/shell/*.c.obj(*.data* *.rodata*)

    /* MCUX SDK 3.0 */
    *components/debug_console/*.c.obj(*.data* *.rodata*)
    *components/shell/*.c.obj(*.data* *.rodata*)
    *components/phy*(*.data* *.rodata*)
    *drivers/flexspi/fsl_flexspi.c.obj(*.data* *.rodata*)
    *drivers/lpi2c/fsl_lpi2c.c.obj(*.data* *.rodata*)
    *drivers/sai/fsl_sai.c.obj(*.data* *.rodata*)
    *drivers/igpio*(*.data* *.rodata*)

    /* Application Common */
    *common/flexspi_hyper_flash_ops.c.obj(*.data* *.rodata*)
    *common/lfs_hyperflash_nor.c.obj(*.data* *.rodata*)
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
    *avb_audio_app/configs.c.obj(*.data* *.rodata*)
    *avb_audio_app/task_config.c.obj(*.data* *.rodata*)

    . = ALIGN(4);
    __slow_data_end = .;
  } > sdram_data AT> flash

  .slow_bss (NOLOAD) :
  {
    . = ALIGN(4);
    __slow_bss_start = .;
    *(__slow_bss)

    /* MCUX SDK */
    *components/codec/*.c.obj(*.bss*)
    *components/gpio/*.c.obj(*.bss*)
    *components/i2c/*.c.obj(*.bss*)
    *components/phy/*.c.obj(*.bss*)
    *components/uart/*.c.obj(*.bss*)
    *drivers/fsl_flexspi.c.obj(*.bss*)
    *drivers/fsl_lpi2c.c.obj(*.bss*)
    *drivers/fsl_sai.c.obj(*.bss*)
    *middleware/littlefs/*.c.obj(*.bss*)
    *middleware/lwip/*.c.obj(*.bss*)
    *utilities/debug_console/*.c.obj(*.bss*)
    *utilities/shell/*.c.obj(*.bss*)

    /* MCUX SDK 3.0 */
    *components/debug_console/*.c.obj(*.bss*)
    *components/shell/*.c.obj(*.bss*)
    *drivers/flexspi/fsl_flexspi.c.obj(*.bss*)
    *drivers/lpi2c/fsl_lpi2c.c.obj(*.bss*)
    *drivers/sai/fsl_sai.c.obj(*.bss*)

    /* Application Common */
    *common/flexspi_hyper_flash_ops.c.obj(*.bss*)
    *common/lfs_hyperflash_nor.c.obj(*.bss*)
    *common/shell.c.obj(*.bss*)
    *common/storage.c.obj(*.bss*)

    /* Application Audio */
    */audio_app/*.c.obj(*.bss*)

    . = ALIGN(4);
    __slow_bss_end = .;
  } > sdram_data

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
  } > dtcm AT> flash

  . = __noncachedata_init_end__;
  .ncache :
  {
    *(NonCacheable)
    . = ALIGN(4);
    __noncachedata_end__ = .;     /* define a global symbol at ncache data end */
  } > dtcm AT> flash

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

