/*
** ###################################################################
**     Processor:           MIMXRT1189CVM8A_cm33
**     Compiler:            GNU C Compiler
**     Reference manual:    IMXRT1180RM, Rev 0, 03/2021
**     Version:             rev. 0.1, 2021-03-09
**     Build:               b220221
**
**     Abstract:
**         Linker file for the GNU C Compiler
**
**     Copyright 2016 Freescale Semiconductor, Inc.
**     Copyright 2016-2022 NXP
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

m_code_tcm_start   = 0x0FFE0000;
m_code_tcm_size    = 0x00020000;
m_system_tcm_start = 0x20000000;
m_system_tcm_size  = 0x00020000;
m_ocram1_ro        = 0xC000;
m_ocram1_nc_start  = 0x20480000 + m_ocram1_ro;
m_ocram1_nc_size   = 0x00008000;
m_ocram1_start     = m_ocram1_nc_start + m_ocram1_nc_size;
m_ocram1_size      = 0x00080000 - m_ocram1_nc_size - m_ocram1_ro;
m_flash_start      = DEFINED(BOARD_FLASH_START_ADDR) ? BOARD_FLASH_START_ADDR : 0x28000000;
m_flash_size       = DEFINED(BOARD_FLASH_SIZE) ? BOARD_FLASH_SIZE : 0x01000000;

HEAP_SIZE          = DEFINED(__heap_size__)  ? __heap_size__  : 0x0400;
STACK_SIZE         = DEFINED(__stack_size__) ? __stack_size__ : 0x1000;

/* Specify the memory areas */
MEMORY
{
  m_code_tcm       (RWX) : ORIGIN = m_code_tcm_start, LENGTH = m_code_tcm_size
  m_system_tcm     (RW)  : ORIGIN = m_system_tcm_start, LENGTH = m_system_tcm_size
  m_ocram1_nc      (RW)  : ORIGIN = m_ocram1_nc_start, LENGTH = m_ocram1_nc_size
  m_ocram1         (RX)  : ORIGIN = m_ocram1_start, LENGTH = m_ocram1_size
}

/* Define output sections */
SECTIONS
{
  /* The startup code goes first into internal RAM */
  .interrupts :
  {
    __VECTOR_TABLE = .;
    . = ALIGN(4);
    KEEP(*(.isr_vector))     /* Startup code */
    . = ALIGN(4);
  } > m_code_tcm

  /* The program code and other data goes into internal RAM */
  .text :
  {
    . = ALIGN(4);
    *(CodeQuickAccess)       /* quick access code section */
    */freertos-kernel/*(*.text* *.rodata*)
    */memcpy.c.obj(*.text* *.rodata*)
    */fsl_netc_endpoint*.c.obj(*.text* *.rodata*)
    . = ALIGN(4);
  } > m_code_tcm

  .slow_text :
  {
    . = ALIGN(4);
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
  } > m_ocram1

  .ARM.extab :
  {
    *(.ARM.extab* .gnu.linkonce.armextab.*)
  } > m_ocram1

  .ARM :
  {
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
  } > m_ocram1

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
  } > m_ocram1

  .dtors :
  {
    __DTOR_LIST__ = .;
    KEEP (*crtbegin.o(.dtors))
    KEEP (*crtbegin?.o(.dtors))
    KEEP (*(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))
    __DTOR_END__ = .;
  } > m_ocram1

  .preinit_array :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } > m_ocram1

  .init_array :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
  } > m_ocram1

  .fini_array :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } > m_ocram1

  .data :
  {
    . = ALIGN(4);
    __data_start__ = .;      /* create a global symbol at data start */
    *(m_usb_dma_init_data)
    *(.data)                 /* .data sections */
    *(.data*)                /* .data* sections */
    *(DataQuickAccess)       /* quick access data section */
    KEEP(*(.jcr*))
    . = ALIGN(4);
    __data_end__ = .;        /* define a global symbol at data end */
  } > m_code_tcm

  .ncache.init :
  {
    __noncachedata_start__ = .;   /* create a global symbol at ncache data start */
    *(NonCacheable.init)
    . = ALIGN(4);
    __noncachedata_init_end__ = .;   /* create a global symbol at initialized ncache data end */
  } > m_ocram1_nc
  . = __noncachedata_init_end__;
  .ncache :
  {
    *(.bss.ep_rx_buff)
    *(NonCacheable)
    . = ALIGN(4);
    __noncachedata_end__ = .;     /* define a global symbol at ncache data end */
  } > m_ocram1_nc

  __NCACHE_REGION_START = ORIGIN(m_ocram1_nc);
  __NCACHE_REGION_SIZE = LENGTH(m_ocram1_nc);

  .slow_bss (NOLOAD) :
  {
    . = ALIGN(4);
    __slow_bss_start = .;
    *middleware/lwip*.c.obj(*.bss*)
    . = ALIGN(4);
    __slow_bss_end = .;
  } > m_ocram1

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
  } > m_code_tcm

  .heap :
  {
    . = ALIGN(8);
    __end__ = .;
    PROVIDE(end = .);
    __HeapBase = .;
    . += HEAP_SIZE;
    __HeapLimit = .;
    __heap_limit = .; /* Add for _sbrk */
  } > m_system_tcm

  .ucHeap :
  {
    . = ALIGN(8);
    __ucHeapBase__ = .;
    PROVIDE(ucHeap = .);
    . = ORIGIN(m_system_tcm) + LENGTH(m_system_tcm) - STACK_SIZE;
  } > m_system_tcm

  __ucHeapSize = . - __ucHeapBase__;

  .stack :
  {
    . = ALIGN(8);
    . += STACK_SIZE;
    __StackEnd = .;
  } > m_system_tcm

  /* Initializes stack on the end of block */
  __StackTop   = ORIGIN(m_system_tcm) + LENGTH(m_system_tcm);
  __StackLimit = __StackTop - STACK_SIZE;
  PROVIDE(__stack = __StackTop);

  .ARM.attributes 0 : { *(.ARM.attributes) }

  ASSERT(__StackLimit >= __bss_end__, "region m_system_tcm overflowed with stack and heap")
}
