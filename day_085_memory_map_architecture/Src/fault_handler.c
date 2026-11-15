/**
 * @file fault_handler.c
 * @brief Low-level Hard Fault capture and diagnostics
 */

#include "fault_handler.h"
#include <stdint.h>

/* Extract active stack pointer and jump to C diagnostics */
__attribute__((naked)) void HardFault_Handler(void)
{
    __asm__ volatile (
        "tst lr, #4\n\t"             /* Check if MSP or PSP was used */
        "ite eq\n\t"
        "mrseq r0, msp\n\t"          /* If 0, stack pointer is MSP */
        "mrsne r0, psp\n\t"          /* If !0, stack pointer is PSP */
        "b fault_print_diagnostics\n\t" /* Branch to C function with pointer in R0 */
    );
}

/* Analyze fault status registers and trap execution */
void fault_print_diagnostics(StackFrame_t *stack_frame)
{
    /* Read SCB fault status registers */
    volatile uint32_t cfsr  = (*((volatile uint32_t *)(0xE000ED28)));
    volatile uint32_t mmar = (*((volatile uint32_t *)(0xE000ED34)));
    volatile uint32_t bfar = (*((volatile uint32_t *)(0xE000ED38)));

    (void)stack_frame;
    (void)cfsr;
    (void)mmar;
    (void)bfar;

    /* Infinite loop to freeze MCU for debugger inspection */
    while (1)
    {
        /* Inspect 'stack_frame->pc' to find the exact crashing instruction */
    }
}
