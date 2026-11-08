/**
 * =================================================================
 * STM32F401RE Custom Linker Script - Day 81 Segment Mapping Test
 * =================================================================
 */

#include <stdint.h>

/* 1. Initialized global variable (Stored in Flash, copied to .data in SRAM at startup) */
uint32_t global_initialized_var = 0xDEADBEEF;

/* 2. Uninitialized global variable (Allocated in .bss in SRAM, zero-initialized at startup) */
uint32_t global_uninitialized_var;

/* 3. Read-only constant (Stored in .rodata section in Flash) */
const char *app_banner = "STM32F401RE Day 81: Segment Mapping Verification";

int main(void) {
    /* Local variable stored on the Stack (anchored by _estack in SRAM) */
    volatile uint32_t counter = 0;

    while (1) {
        counter++;

        /* Dummy read to prevent variables from being optimized out by the compiler */
        if (global_initialized_var == 0xDEADBEEF) {
            global_uninitialized_var = counter;
        }
    }
}
