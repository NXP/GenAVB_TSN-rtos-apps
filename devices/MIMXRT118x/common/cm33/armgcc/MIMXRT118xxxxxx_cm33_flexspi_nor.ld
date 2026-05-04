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

app_image_offset   = 0x0000B000;
m_code_tcm_start   = 0x0FFE0000;
m_code_tcm_size    = 0x00020000;
m_code_tcm_end     = m_code_tcm_start + m_code_tcm_size;
m_system_tcm_start = 0x20000000;
m_system_tcm_size  = 0x00020000;
m_system_tcm_end   = m_system_tcm_start + m_system_tcm_size;
m_ocram1_ro        = 0xC000;
m_ocram1_nc_start  = 0x20480000 + m_ocram1_ro;
m_ocram1_nc_size   = 0x00008000;
m_ocram1_start     = m_ocram1_nc_start + m_ocram1_nc_size;
m_ocram1_size      = 0x00080000 - m_ocram1_nc_size - m_ocram1_ro;
m_flash_start      = DEFINED(BOARD_FLASH_START_ADDR) ? BOARD_FLASH_START_ADDR : 0x28000000;
m_flash_size       = DEFINED(BOARD_FLASH_SIZE) ? BOARD_FLASH_SIZE : 0x01000000;

m33_max_image_size = 1M;

lfs_size           = (m_flash_size / 2);
lfs_start          = m_flash_start + lfs_size;

HEAP_SIZE          = DEFINED(__heap_size__)  ? __heap_size__  : 0x0400;
STACK_SIZE         = DEFINED(__stack_size__) ? __stack_size__ : 0x1000;

/* Boot header offsets and sizes */
m_fcb_offset       = 0x400;
m_fcb_size         = 0x200;
m_xmcd_offset      = 0x800;
m_xmcd_size        = 0x400;
m_container_offset = 0x1000;
m_container_size   = 0x2000;

/* Container image symbols */
__CONTAINER_IMG_OFFSET = app_image_offset - m_container_offset;

/* Specify the memory areas */
MEMORY
{
  m_fcb            (R)  : ORIGIN = m_flash_start + m_fcb_offset, LENGTH = m_fcb_size
  m_xmcd           (R)  : ORIGIN = m_flash_start + m_xmcd_offset, LENGTH = m_xmcd_size
  m_container      (R)  : ORIGIN = m_flash_start + m_container_offset, LENGTH = m_container_size
  m_flash          (RX)  : ORIGIN = m_flash_start + app_image_offset, LENGTH = m33_max_image_size - app_image_offset
  m_code_tcm       (RX)  : ORIGIN = m_code_tcm_start, LENGTH = m_code_tcm_size
  m_system_tcm     (RW)  : ORIGIN = m_system_tcm_start, LENGTH = m_system_tcm_size
  m_ocram1_nc      (RW)  : ORIGIN = m_ocram1_nc_start, LENGTH = m_ocram1_nc_size
  m_ocram1         (RX)  : ORIGIN = m_ocram1_start, LENGTH = m_ocram1_size
}

/* Define output sections */
SECTIONS
{
  .fcb :
  {
    KEEP(*(.boot_hdr.conf))
  } > m_fcb

  .xmcd :
  {
    KEEP(*(.boot_hdr.xmcd_data))
  } > m_xmcd

  .container :
  {
    KEEP(*(.boot_hdr.container))
  } > m_container

  /* The startup code goes first into internal RAM */
  .interrupts :
  {
    __VECTOR_TABLE = .;
    __Vectors = .;
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
  } > m_code_tcm

  /* Reset Handler and System init in flash */
  .startup :
  {
    *startup_MIMXRT118x_cm33.S.obj(.text)
    *(.text.SystemInit*)
    *(.text.BOARD_InitHook)
    . = ALIGN(4);
  } > m_flash

  /* .text goes into internal RAM */
  .text :
  {
    __text_load = LOADADDR(.text);
    . = ALIGN(4);
    __text_start = .;
    *(CodeQuickAccess)
    */freertos-kernel/*(*.text* *.rodata*)
    */memcpy.c.obj(*.text* *.rodata*)
    */fsl_netc_endpoint*.c.obj(*.text* *.rodata*)
    . = ALIGN(4);
    __text_end = .;
  } > m_code_tcm AT> m_flash

  /* .slow_text goes into OCRAM */
  .slow_text :
  {
    __slow_text_load = LOADADDR(.slow_text);
    . = ALIGN(4);
    __slow_text_start = .;
    *(.text)                 /* .text sections (code) */
    *(.text*)                /* .text* sections (code) */
    *(.rodata)               /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)              /* .rodata* sections (constants, strings, etc.) */
    *(.glue_7)               /* glue arm to thumb code */
    *(.glue_7t)              /* glue thumb to arm code */
    *(.eh_frame)
    KEEP (*(.init))
    KEEP (*(.fini))
  } > m_ocram1 AT> m_flash

  .ARM.extab :
  {
    *(.ARM.extab* .gnu.linkonce.armextab.*)
  } > m_ocram1 AT> m_flash

  .ARM :
  {
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
  } > m_ocram1 AT> m_flash

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
  } > m_ocram1 AT> m_flash

  .dtors :
  {
    __DTOR_LIST__ = .;
    KEEP (*crtbegin.o(.dtors))
    KEEP (*crtbegin?.o(.dtors))
    KEEP (*(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))
    __DTOR_END__ = .;
  } > m_ocram1 AT> m_flash

  .preinit_array :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } > m_ocram1 AT> m_flash

  .init_array :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
  } > m_ocram1 AT> m_flash

  .fini_array :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
    . = ALIGN(4);
    __slow_text_end = .;
  } > m_ocram1 AT> m_flash

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
  } > m_code_tcm AT> m_flash

  .ncache.init :
  {
    __noncachedata_start__ = .;   /* create a global symbol at ncache data start */
    *(NonCacheable.init)
    . = ALIGN(4);
    __noncachedata_init_end__ = .;   /* create a global symbol at initialized ncache data end */
  } > m_ocram1_nc AT> m_flash

    __flash_image_end__ = LOADADDR(.ncache.init) + SIZEOF(.ncache.init);
  __CONTAINER_IMG_SIZE = __flash_image_end__ - (m_flash_start + app_image_offset);

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
    __StackStart = .;
    . += STACK_SIZE;
  } > m_system_tcm

  /* Initializes stack on the end of block */
  __StackTop   = ORIGIN(m_system_tcm) + LENGTH(m_system_tcm);
  __StackLimit = __StackTop - STACK_SIZE;
  PROVIDE(__stack = __StackTop);

  .ARM.attributes 0 : { *(.ARM.attributes) }

  ASSERT(__StackLimit >= __bss_end__, "region m_system_tcm overflowed with stack and heap")
}
