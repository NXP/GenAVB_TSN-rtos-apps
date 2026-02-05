/*
** ###################################################################
**     Processor:           MIMXRT1189CVM8A_cm7
**     Compiler:            GNU C Compiler
**     Reference manual:    IMXRT1180RM, Rev 1, 06/2022
**     Version:             rev. 0.1, 2021-03-09
**     Build:               b220810
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

m_itcm_start       = 0x00000000;
m_itcm_size        = 0x00040000;
m_dtcm_start       = 0x20000000;
m_dtcm_size        = 0x00040000;
m_ocram2_nc_start  = 0x20500000;
m_ocram2_nc_size   = 0x00002000;
m_ocram2_start     = m_ocram2_nc_start + m_ocram2_nc_size;
m_ocram2_size      = 0x00040000 - m_ocram2_nc_size;
m_flash_start      = DEFINED(BOARD_FLASH_START_ADDR) ? BOARD_FLASH_START_ADDR : 0x28000000;
m_flash_size       = DEFINED(BOARD_FLASH_SIZE) ? BOARD_FLASH_SIZE : 0x01000000;

m7_image_offset    = 1M;
m7_image_start     = m_flash_start + m7_image_offset;

lfs_size           = (m_flash_size / 2);
lfs_start          = m_flash_start + lfs_size;

HEAP_SIZE          = DEFINED(__heap_size__)  ? __heap_size__  : 0x0400;
STACK_SIZE         = DEFINED(__stack_size__) ? __stack_size__ : 0x0400;

/* Specify the memory areas */
MEMORY
{
  m_flash          (RX)  : ORIGIN = m7_image_start, LENGTH = m_flash_size - m7_image_offset - lfs_size
  m_itcm           (RX)  : ORIGIN = m_itcm_start, LENGTH = m_itcm_size
  m_dtcm           (RW)  : ORIGIN = m_dtcm_start, LENGTH = m_dtcm_size
  m_ocram2_nc      (RW)  : ORIGIN = m_ocram2_nc_start, LENGTH = m_ocram2_nc_size
  m_ocram2         (RW)  : ORIGIN = m_ocram2_start, LENGTH = m_ocram2_size
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
  } > m_flash

  /* Place for interrupt table */
  .interrupts_ram :
  {
    . = ALIGN(4);
    __ram_isr_vector_start = .;
    . += SIZEOF(.interrupts);
    __ram_isr_vector_end = .;
    . = ALIGN(4);
  } > m_itcm

  /* Reset Handler and System init in flash */
  .startup :
  {
    *startup_MIMXRT118x_cm7.S.obj(.text)
    *(.text.SystemInit*)
    *(.text.BOARD_InitHook)
    . = ALIGN(4);
  } > m_flash

  .slow_text :
  {
    __slow_text_load = LOADADDR(.slow_text);
    . = ALIGN(4);
    __slow_text_start = .;
    /* code goes here */
    . = ALIGN(4);
    __slow_text_end = .;
  } > m_ocram2 AT> m_flash

  /* The program code and other data goes into internal RAM */
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
    *(CodeQuickAccess)
    KEEP (*(.init))
    KEEP (*(.fini))
    . = ALIGN(4);
  } > m_itcm AT> m_flash

  .ARM.extab :
  {
    *(.ARM.extab* .gnu.linkonce.armextab.*)
  } > m_itcm AT> m_flash

  .ARM :
  {
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
  } > m_itcm AT> m_flash

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
  } > m_itcm AT> m_flash

  .dtors :
  {
    __DTOR_LIST__ = .;
    KEEP (*crtbegin.o(.dtors))
    KEEP (*crtbegin?.o(.dtors))
    KEEP (*(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))
    __DTOR_END__ = .;
  } > m_itcm AT> m_flash

  .preinit_array :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } > m_itcm AT> m_flash

  .init_array :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
  } > m_itcm AT> m_flash

  .fini_array :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
    __text_end = .;
  } > m_itcm AT> m_flash

  .data :
  {
    __data_load__ = LOADADDR(.data);
    . = ALIGN(4);
    __data_start__ = .;      /* create a global symbol at data start */
    *(m_usb_dma_init_data)
    *(.data)                 /* .data sections */
    *(.data*)                /* .data* sections */
    *(DataQuickAccess)       /* quick access data section */
    KEEP(*(.jcr*))
    . = ALIGN(4);
    __data_end__ = .;        /* define a global symbol at data end */
  } > m_dtcm AT> m_flash

  .ncache.init :
  {
    __noncachedata_start__ = .;   /* create a global symbol at ncache data start */
    *(NonCacheable.init)
    . = ALIGN(4);
    __noncachedata_init_end__ = .;   /* create a global symbol at initialized ncache data end */
  } > m_ocram2_nc AT> m_flash
  . = __noncachedata_init_end__;
  .ncache :
  {
    *(NonCacheable)
    . = ALIGN(4);
    __noncachedata_end__ = .;     /* define a global symbol at ncache data end */
  } > m_ocram2_nc

  __NCACHE_REGION_START = ORIGIN(m_ocram2_nc);
  __NCACHE_REGION_SIZE = LENGTH(m_ocram2_nc);

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
  } > m_dtcm

  .heap :
  {
    . = ALIGN(8);
    __end__ = .;
    PROVIDE(end = .);
    __HeapBase = .;
    . += HEAP_SIZE;
    __HeapLimit = .;
    __heap_limit = .; /* Add for _sbrk */
  } > m_dtcm

  .ucHeap :
  {
    . = ALIGN(8);
    __ucHeapBase__ = .;
    PROVIDE(ucHeap = .);
    . = ORIGIN(m_dtcm) + LENGTH(m_dtcm) - STACK_SIZE;
  } > m_dtcm

  __ucHeapSize = . - __ucHeapBase__;

  .stack :
  {
    . = ALIGN(8);
    __StackStart = .;
    . += STACK_SIZE;
  } > m_dtcm

  /* Initializes stack on the end of block */
  __StackTop   = ORIGIN(m_dtcm) + LENGTH(m_dtcm);
  __StackLimit = __StackTop - STACK_SIZE;
  PROVIDE(__stack = __StackTop);

  .ARM.attributes 0 : { *(.ARM.attributes) }

  ASSERT(__StackLimit >= __StackStart, "region m_dtcm overflowed with stack and heap")
}
