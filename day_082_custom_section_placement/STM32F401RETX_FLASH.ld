/* 
 * =================================================================
 * STM32F401RE Custom Linker Script - Day 82 Custom Section Placement
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

    /* Day 82: Custom user-defined RAM section for isolated variables */
    .custom_ram_data :
    {
        . = ALIGN(4);
        _scustom = .;      /* Start address of custom section */
        *(.custom_data)    /* Custom tagged data sections */
        *(.custom_data*)
        . = ALIGN(4);
        _ecustom = .;      /* End address of custom section */
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

    /* User heap and stack section, placed after bss */
    ._user_heap_stack :
    {
        . = ALIGN(8);
        PROVIDE ( end = . );
        PROVIDE ( _end = . );
        . = . + _Min_Heap_Size;
        . = . + _Min_Stack_Size;
        . = ALIGN(8);
    } >SRAM
}