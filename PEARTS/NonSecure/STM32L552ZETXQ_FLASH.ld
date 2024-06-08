/*
******************************************************************************
**
**  File        : LinkerScript.ld
**
**  Author      : STM32CubeIDE
**
**  Abstract    : Linker script for STM32L552xE Device from STM32L5 series
**                      512Kbytes FLASH
**                      256Kbytes RAM
**
**                Set heap size, stack size and stack location according
**                to application requirements.
**
**                Set memory bank area and size if external memory is used.
**
**  Target      : STMicroelectronics STM32
**
**  Distribution: The file is distributed as is without any warranty
**                of any kind.
**
*****************************************************************************
** @attention
**
** Copyright (c) 2023 STMicroelectronics.
** All rights reserved.
**
** This software is licensed under terms that can be found in the LICENSE file
** in the root directory of this software component.
** If no LICENSE file comes with this software, it is provided AS-IS.
**
*****************************************************************************
*/

/* Entry Point */
ENTRY(Reset_Handler)

/* Highest address of the user mode stack */
_estack = ORIGIN(RAM) + LENGTH(RAM);	/* end of "RAM" Ram type memory */

_Min_Heap_Size = 0x200;	/* required amount of heap  */
_Min_Stack_Size = 0x400;	/* required amount of stack */

_elastic_stack_size = 0x400; /* Corrected size comment to 1KB */

/* Memories definition */
MEMORY
{
  RAM	(xrw)		: ORIGIN = 0x20018000,	LENGTH = 92K     /* Memory is divided. Actual start is 0x20000000 and actual length is 192K */
  RAM_RESERVED (rw): ORIGIN = 0x2002F000, LENGTH = 4k
  RAM2	(xrw)		: ORIGIN = 0x20030000,	LENGTH = 56K
  RAM_ELASTIC (xrw) : ORIGIN = 0x2003E000,  LENGTH = 8k
  
  FLASH	(rx)		: ORIGIN = 0x08040000,	LENGTH = 188K    /* Memory is divided. Actual start is 0x8000000 and actual length is 512K */
  ELASTIC_NS (rx)	: ORIGIN = 0x0806E000,	LENGTH = 8K
  FLASH2 (rx)		: ORIGIN = 0x08070000,	LENGTH = 64k     /* 64k = 0x00010000, 8k = 0x2000 */
	
}



/* Sections */
SECTIONS
{
  /* The startup code into "FLASH" Rom type memory */
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector)) /* Startup code */
    . = ALIGN(4);
  } >FLASH

  /* The program code and other data into "FLASH" Rom type memory */
  .text :
  {
    . = ALIGN(4);
    *(.text)           /* .text sections (code) */
    *(.text*)          /* .text* sections (code) */
    *(.glue_7)         /* glue arm to thumb code */
    *(.glue_7t)        /* glue thumb to arm code */
    *(.eh_frame)

    KEEP (*(.init))
    KEEP (*(.fini))

    . = ALIGN(4);
    _etext = .;        /* define a global symbols at end of code */
  } >FLASH
  
  
    .elastic_ns_section :
  {
    . = ALIGN(4);
    __elastic_ns_start = .;
    *(.elastic_ns_section)
    __elastic_ns_end = .;
    . = ALIGN(4);
  } >ELASTIC_NS

  .elastic_section :
  {
    . = ALIGN(4);
    __elastic_start = .;
    *(.elastic_section)
    __elastic_end = .;
    . = ALIGN(4);
  } >FLASH2


    .elastic_data :
  {
    . = ALIGN(4);
    _elastic_global_start = .;
	*(.elastic_data) 
    _elastic_global_end = .;
    . = ALIGN(4);
  } >FLASH2 AT> RAM_ELASTIC
  
  
  .elastic_stack :
  {
    . = ALIGN(4);
    _elastic_stack_start = .;
    . = . + _elastic_stack_size;
    _elastic_stack_end = .;
    . = ALIGN(4);
  } >RAM2 
	

  /* The functions that are going to be attested */
  /* .att_funcs :
  {
    . = ALIGN(32);
    _beg_att_funcs = .;
    *(.att_funcs)
    . = ALIGN(32);
    _end_att_funcs = .;
  } >FLASH */




  /* Constant data into "FLASH" Rom type memory */
  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
    . = ALIGN(4);
  } >FLASH

  .ARM.extab   : {
    . = ALIGN(4);
    *(.ARM.extab* .gnu.linkonce.armextab.*)
    . = ALIGN(4);
  } >FLASH

  .ARM : {
    . = ALIGN(4);
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
    . = ALIGN(4);
  } >FLASH

  .preinit_array     :
  {
    . = ALIGN(4);
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
    . = ALIGN(4);
  } >FLASH

  .init_array :
  {
    . = ALIGN(4);
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
    . = ALIGN(4);
  } >FLASH

  .fini_array :
  {
    . = ALIGN(4);
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
    . = ALIGN(4);
  } >FLASH

  /* Used by the startup to initialize data */
  _sidata = LOADADDR(.data);

  /* Initialized data sections into "RAM" Ram type memory */
  .data :
  {
    . = ALIGN(4);
    _sdata = .;        /* create a global symbol at data start */
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */
    *(.RamFunc)        /* .RamFunc sections */
    *(.RamFunc*)       /* .RamFunc* sections */

    . = ALIGN(4);
    _edata = .;        /* define a global symbol at data end */

  } >RAM AT> FLASH

  /* Uninitialized data section into "RAM" Ram type memory */
  . = ALIGN(4);
  .bss :
  {
    /* This is used by the startup in order to initialize the .bss section */
    _sbss = .;         /* define a global symbol at bss start */
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)

    . = ALIGN(4);
    _ebss = .;         /* define a global symbol at bss end */
    __bss_end__ = _ebss;
  } >RAM

  /* User_heap_stack section, used to check that there is enough "RAM" Ram  type memory left */
  ._user_heap_stack :
  {
    . = ALIGN(8);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;
    . = ALIGN(8);
  } >RAM


  /* Remove information from the compiler libraries */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }
}
