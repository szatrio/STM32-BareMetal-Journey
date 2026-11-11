/**
 * =================================================================
 * STM32F401RE Bare-Metal - Day 83 Memory Headroom & Stack Monitor
 * MISRA-Compliant: No dynamic memory allocation (malloc prohibited)
 * =================================================================
 */

#include <stdint.h>

/* Import external linker symbols defined in STM32F401RETX_FLASH.ld */
extern uint32_t _end;      /* End of BSS / start of heap boundary */
extern uint32_t _estack;   /* Top of stack (highest SRAM address) */

/* Force a test variable into SRAM to keep track of memory usage */
__attribute__((section(".custom_data"))) uint32_t my_custom_variable = 0xCAFEBABE;

/**
 * @brief Read Main Stack Pointer (MSP) using pure inline assembly
 * @return Current MSP register value
 */
static inline uint32_t get_msp(void)
{
    uint32_t result;
    __asm__ volatile ("MRS %0, msp" : "=r" (result) );
    return result;
}

/**
 * @brief Calculate the remaining safe RAM space (Stack-Heap headroom)
 * @return Number of free bytes between current stack pointer and heap end
 */
static uint32_t get_sram_headroom(void)
{
    uint32_t current_msp = get_msp();
    uint32_t heap_end   = (uint32_t)&_end;

    if (current_msp > heap_end) {
        return current_msp - heap_end;
    }
    return 0; /* Stack collision / overflow detected! */
}

int main(void)
{
    volatile uint32_t free_headroom_bytes = 0;
    volatile uint32_t test_var = my_custom_variable;
    (void)test_var;

    while (1)
    {
        /* Continuously monitor available stack/heap headroom at runtime */
        free_headroom_bytes = get_sram_headroom();

        /* Prevent compiler optimization */
        (void)free_headroom_bytes;
    }
}
