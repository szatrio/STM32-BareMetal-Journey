/*
 * =================================================================
 * STM32F401RE Clean Default Linker Script for Hard Fault Testing
 * Target: STM32F401RE (Flash: 512KB, SRAM: 96KB)
 * =================================================================
 */

/* Entry point */
ENTRY(Reset_Handler)

/* Highest address of the user mode stack */
_estack = ORIGIN(SRAM) + LENGTH(SRAM);

/* Required amount of heap and stack */
_Min_Heap_Size = 0x200;    /* 512 bytes */
_Min_Stack_Size = 0x400;   /* 1 KB */

/* Physical memory boundaries */
MEMORY
{
    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 512K
    SRAM  (rwx) : ORIGIN = 0x20000000, LENGTH = 96K
}

/* Sections mapping */
SECTIONS
{
    /* The startup code and interrupt vector table go into Flash */
    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        . = ALIGN(4);
    } >FLASH

    /* Program executable code goes into Flash */
    .text :
    {
        . = ALIGN(4);
        *(.text)           /* .text sections (code) */
        *(.text*)          /* .text* sections (code) */
        . = ALIGN(4);
        _etext = .;        /* Define a global symbol at end of code */
    } >FLASH

    /* Read-only data (constants, strings, lookup tables) goes into Flash */
    .rodata :
    {
        . = ALIGN(4);
        *(.rodata)         /* .rodata sections */
        *(.rodata*)        /* .rodata* sections */
        . = ALIGN(4);
    } >FLASH

    /* C-Runtime constructor and destructor initialization tables */
    .init_array :
    {
        . = ALIGN(4);
        __init_array_start = .;
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        __init_array_end = .;
        . = ALIGN(4);
    } >FLASH

    .fini_array :
    {
        . = ALIGN(4);
        __fini_array_start = .;
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        __fini_array_end = .;
        . = ALIGN(4);
    } >FLASH

    /* Initialized data sections go into RAM, but loaded from Flash */
    _sidata = LOADADDR(.data);

    .data : 
    {
        . = ALIGN(4);
        _sdata = .;        /* Create a global symbol for data start */
        *(.data)           /* .data sections */
        *(.data*)          /* .data* sections */
        . = ALIGN(4);
        _edata = .;        /* Define a global symbol at data end */
    } >SRAM AT> FLASH

    /* Uninitialized data section goes into RAM (Zero-filled) */
    .bss :
    {
        . = ALIGN(4);
        _sbss = .;         /* Define a global symbol for bss start */
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;         /* Define a global symbol for bss end */
    } >SRAM

    /* User heap section placed after bss. Stack grows downwards from _estack. */
    ._user_heap_stack :
    {
        . = ALIGN(8);
        PROVIDE ( end = . );
        PROVIDE ( _end = . );
        . = . + _Min_Heap_Size;
        . = ALIGN(8);
    } >SRAM
}