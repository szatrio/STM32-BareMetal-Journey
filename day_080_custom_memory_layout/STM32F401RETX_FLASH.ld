/* 
 * =================================================================
 * STM32F401RE Custom Linker Script - Memory Layout (Day 80)
 * Target: STM32F401RE (Flash: 512KB, SRAM: 96KB)
 * =================================================================
 */

/* Entry point */
ENTRY(Reset_Handler)

/* Physical memory boundaries definition */
MEMORY
{
    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 512K
    SRAM  (rwx) : ORIGIN = 0x20000000, LENGTH = 96K
}

/* 
 * Note: SECTIONS mapping (.text, .data, .bss) and stack/heap symbols 
 * will be implemented progressively in the upcoming days.
 */