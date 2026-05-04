/*
** ###################################################################
**     Processors:          MCXE31BMPB
**
**     Compiler:            GNU C Compiler
**     Reference manual:    MCXE31RM, Rev. 1, Jan. 2025
**     Version:             rev. 1.0, 2023-03-01
**     Build:               b241119
**
**     Abstract:
**         Linker file for the GNU C Compiler
**
**     Copyright 2016 Freescale Semiconductor, Inc.
**     Copyright 2016-2025 NXP
**     SPDX-License-Identifier: BSD-3-Clause
**
**     http:                 www.nxp.com
**     mail:                 support@nxp.com
**
** ###################################################################
*/

/* Entry Point */
ENTRY(Reset_Handler)

HEAP_SIZE  = DEFINED(__heap_size__)  ? __heap_size__  : 0x0400;
STACK_SIZE = DEFINED(__stack_size__) ? __stack_size__ : 0x0400;

m_itcm_start             = 0x00000000;
m_itcm_end               = 0x00007FFF;
m_itcm_size              = m_itcm_end - m_itcm_start;

m_dtcm_start             = 0x20000000;
m_dtcm_end               = 0x2000FFFF;
m_dtcm_size              = m_dtcm_end - m_dtcm_start;

m_flash_start            = 0x00400000;
m_flash_end              = 0x007D3FFF; /* The flash memory region [0x007D4000-0x7FFFFF] is reserved for HSE_B. */
m_flash_size             = m_flash_end - m_flash_start;

m_sram_stdby_mode_offset = 0x00008000; /* ~32 KB */
m_sram_start             = 0x20400000 + m_sram_stdby_mode_offset;
m_sram_end               = 0x2044FFFF;
m_sram_size              = (m_sram_end + 1) - m_sram_start;


/* Specify the memory areas */
MEMORY
{
  flash                 (RX)  : ORIGIN = m_flash_start,       LENGTH = m_flash_size
  itcm                  (RX)  : ORIGIN = m_itcm_start,        LENGTH = m_itcm_size
  dtcm                  (RW)  : ORIGIN = m_dtcm_start,        LENGTH = m_dtcm_size
  sram                  (RW)  : ORIGIN = m_sram_start,        LENGTH = m_sram_size 
 }

boot_hdr_flash_conf = ORIGIN(flash);
program_image = boot_hdr_flash_conf + 0x1000;

/* Define output sections */
SECTIONS
{
  .boot_header boot_hdr_flash_conf :
  {
    . = ALIGN(4);
    KEEP(*(.boot_header))    /* IVT */
    . = ALIGN(4);
  } > flash

  /* The startup code goes first into internal flash */
  .interrupts program_image :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector))     /* Startup code */
    . = ALIGN(4);
  } > flash
  
  /* NULL pointer protection */
  .guard_area :
  {
    __guard_area_start = .;
    . += 0x400;
    __guard_area_end = .;
  } > itcm
  
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

    /* Application Common */
    *common/flexspi_nor_flash_ops.c.obj(*.text*)
    *common/lfs_qspi_nor.c.obj(*.text*)
    *common/log.c.obj(*.text*)
    *common/lwip/lwip*.c.obj(*.text*)
    *common/shell.c.obj(*.text*)
    *common/storage.c.obj(*.text*)

    /* MCUX SDK 3.0 */
    */configs.c.obj(*.text*)
    */pipeline_config.c.obj(*.text*)

    /* Application TSN */
    *tsn_app/shell.c.obj(*.text*)

    . = ALIGN(4);
    __slow_text_end = .;
  } > flash

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

    /* Application TSN */
    *tsn_app/configs.c.obj(*.data* *.rodata*)

    . = ALIGN(4);
    __slow_data_end = .;
  } > sram AT> flash
  
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
  } > sram

  /* The program code and other data goes into internal flash */
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
  } > flash

  .ARM.extab :
  {
    *(.ARM.extab* .gnu.linkonce.armextab.*)
  } > flash

  .ARM :
  {
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
  __text_end = .;
  } > flash

 .ctors :
  {
    __CTOR_LIST__ = .;
    /* gcc uses crtbegin.o to find the start of
       the constructors, so we make sure it is
       first.  Because this is a wildcard, it
       doesn't matter if the user does not
       actually link against crtbegin.o; the
       linker won't look for a file to match a
       wildcard.  The wildcard also means that it
       doesn't matter which directory crtbegin.o
       is in.  */
    KEEP (*crtbegin.o(.ctors))
    KEEP (*crtbegin?.o(.ctors))
    /* We don't want to include the .ctor section from
       from the crtend.o file until after the sorted ctors.
       The .ctor section from the crtend file contains the
       end of ctors marker and it must be last */
    KEEP (*(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))
    __CTOR_END__ = .;
  } > flash

  .dtors :
  {
    __DTOR_LIST__ = .;
    KEEP (*crtbegin.o(.dtors))
    KEEP (*crtbegin?.o(.dtors))
    KEEP (*(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))
    __DTOR_END__ = .;
  } > flash

  .preinit_array :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } > flash

  .init_array :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
  } > flash

  .fini_array :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } > flash

  __etext = .;    /* define a global symbol at end of code */
  __DATA_ROM = .; /* Symbol is used by startup for data initialization */

  /* reserve MTB memory at the beginning of sram */
  .mtb : /* MTB buffer address as defined by the hardware */
  {
    . = ALIGN(8);
    _mtb_start = .;
    KEEP(*(.mtb_buf)) /* need to KEEP Micro Trace Buffer as not referenced by application */
    . = ALIGN(8);
    _mtb_end = .;
  } > sram

  .data : AT(__DATA_ROM)
  {
    . = ALIGN(4);
    __DATA_RAM = .;
    __data_start__ = .;      /* create a global symbol at data start */
    *(.data)                 /* .data sections */
    *(.data*)                /* .data* sections */
    KEEP(*(.jcr*))
    . = ALIGN(4);
    __data_end__ = .;        /* define a global symbol at data end */
  } > dtcm

  __ram_function_flash_start = __DATA_ROM + (__data_end__ - __data_start__); /* Symbol is used by startup for initialization */
  .ram_function : AT(__ram_function_flash_start)
  {
    . = ALIGN(8);
    __ram_function_start__ = .;
    *(CodeQuickAccess)
    . = ALIGN(8);
    __ram_function_end__ = .;
  } > itcm

  __qadata_flash_start = __ram_function_flash_start + (__ram_function_end__ - __ram_function_start__);
  .qadata : AT(__qadata_flash_start)
  {
    . = ALIGN(4);
    __qadata_start__ = .;
    . = ALIGN(4);
    __qadata_end__ = .;
  } > dtcm

  __noncache_data_flash_start = __qadata_flash_start + (__qadata_end__ - __qadata_start__);
  .ncache.init : AT(__noncache_data_flash_start)
  {
    . = ALIGN(8);
    __noncache_data_start__ = .;      /* create a global symbol at ncache data start */
    *(NonCacheable.init)
    . = ALIGN(8);
    __noncache_data_end__ = .;   /* create a global symbol at ncache data end */
  } > dtcm

  .ncache :
  {
    . = ALIGN(8);
    __noncache_bss_start__ = .;  /* define a global symbol at ncache bss start */
    *(NonCacheable)
    . = ALIGN(8);
    __noncache_bss_end__ = .;    /* define a global symbol at ncache bss end */
  } > dtcm

  __DATA_END = __noncache_data_flash_start + (__noncache_data_end__ - __noncache_data_start__);
  text_end = ORIGIN(flash) + LENGTH(flash);
  ASSERT(__DATA_END <= text_end, "region flash overflowed with text and data")

  /* Uninitialized data section */
  .bss :
  {
    /* This is used by the startup in order to initialize the .bss section */
    . = ALIGN(4);
    __START_BSS = .;
    __bss_start__ = .;
    *(.bss)
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    __bss_end__ = .;
    __END_BSS = .;
  } > sram

  .heap :
  {
    . = ALIGN(8);
    __end__ = .;
    PROVIDE(end = .);
    __HeapBase = .;
    . += HEAP_SIZE;
    __HeapLimit = .;
    __heap_limit = .; /* Add for _sbrk */
  } > sram

  .ucHeap :
  {
    . = ALIGN(8);
    __ucHeapBase__ = .;
    PROVIDE(ucHeap = .);
    . = ORIGIN(sram) + LENGTH(sram) - STACK_SIZE;
  } > sram

  __ucHeapSize = . - __ucHeapBase__;

  .stack :
  {
    . = ALIGN(8);
    . += STACK_SIZE;
  } > dtcm

  /* Initializes stack on the end of block */
  __StackTop   = ORIGIN(sram) + LENGTH(sram);
  __StackLimit = __StackTop - STACK_SIZE;
  PROVIDE(__stack = __StackTop);

  .ARM.attributes 0 : { *(.ARM.attributes) }

  ASSERT(__StackLimit >= __HeapLimit, "region sram overflowed with stack and heap")
}

