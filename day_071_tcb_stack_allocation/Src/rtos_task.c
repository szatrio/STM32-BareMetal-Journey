#include "rtos_task.h"
#include <stddef.h>

/**
 * @brief Default exit handler if a task function accidentally returns.
 */
static void RTOS_Task_ExitError(void) {
    while (1) {
        // Trap task execution on exit
    }
}

bool RTOS_Task_Create(TCB_t *pxTask, void (*task_code)(void)) {
    if (pxTask == NULL || task_code == NULL) {
        return false;
    }

    // Point stack pointer to the top of the allocated stack array (grows downward)
    uint32_t *pStk = &(pxTask->stack[configSTACK_SIZE]);

    /*
     * 1. Simulate hardware-saved registers (automatically pushed by Cortex-M4 on exception):
     * Order (bottom to top of stack): xPSR, PC, LR, R12, R3, R2, R1, R0
     */

    // xPSR: Set Thumb execution bit (bit 24 = 1)
    *(--pStk) = (1UL << 24);

    // PC: Program Counter (Task entry function address)
    *(--pStk) = (uint32_t)task_code;

    // LR: Link Register (Fallback handler if task returns)
    *(--pStk) = (uint32_t)RTOS_Task_ExitError;

    // R12, R3, R2, R1, R0 (Initialized to 0)
    *(--pStk) = 0x00000000UL; // R12
    *(--pStk) = 0x00000000UL; // R3
    *(--pStk) = 0x00000000UL; // R2
    *(--pStk) = 0x00000000UL; // R1
    *(--pStk) = 0x00000000UL; // R0

    /*
     * 2. Simulate software-saved registers (manually pushed by context switcher):
     * Order: R11, R10, R9, R8, R7, R6, R5, R4
     */
    for (uint32_t i = 0; i < 8U; i++) {
        *(--pStk) = 0x00000000UL;
    }

    // Save the finalized stack pointer position inside the TCB
    pxTask->pStackPointer = pStk;
    pxTask->state = TASK_READY;
    pxTask->priority = 1U;

    return true;
}
