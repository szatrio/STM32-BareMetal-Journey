/**
 * @file fault_handler.h
 * @brief Hard Fault Handler Header for ARM Cortex-M4
 */

#ifndef FAULT_HANDLER_H
#define FAULT_HANDLER_H

#include <stdint.h>

/* CPU stacked registers during a fault */
typedef struct {
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r12;
    uint32_t lr;   /* Link Register */
    uint32_t pc;   /* Program Counter (Fault address) */
    uint32_t psr;  /* Program Status Register */
} StackFrame_t;

void fault_print_diagnostics(StackFrame_t *stack_frame);

#endif /* FAULT_HANDLER_H */
