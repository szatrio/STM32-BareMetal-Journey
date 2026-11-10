/**
 * =================================================================
 * STM32F401RE Custom Linker Script - Day 82 Custom Section Verification
 * =================================================================
 */

#include <stdint.h>

/* Force this global variable into our newly created custom SRAM section */
__attribute__((section(".custom_data"))) uint32_t my_custom_variable = 0xCAFEBABE;

int main(void)
{
    /* Infinite loop to keep the system running and allow memory inspection via debugger/map file */
    while (1)
    {
        /* Prevent optimization of the custom variable */
        volatile uint32_t val = my_custom_variable;
        (void)val;
    }
}
