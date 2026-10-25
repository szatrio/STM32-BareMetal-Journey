#include "rtos_task.h"
#include "cortex_m4_core.h"
#include <stddef.h>

// Global pointers to track current and next running tasks
volatile TCB_t *pxCurrentTaskPtr = NULL;
volatile TCB_t *pxNextTaskPtr    = NULL;

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

/**
 * @brief Temporary helper function to shift task pointers (will be replaced by Scheduler).
 */
void RTOS_SwitchTaskPointer(void) {
    pxCurrentTaskPtr = pxNextTaskPtr;
}

/**
 * @brief PendSV Exception Handler (Naked Function)
 *        Handles the low-level context switching between RTOS tasks in Assembly.
 */
__attribute__((naked)) void PendSV_Handler(void) {
    /*
     * 1. SAVE THE CONTEXT OF THE CURRENT TASK
     */
    // Get Process Stack Pointer (PSP) of the currently running task
    __asm volatile ("MRS r0, psp");

    // Save software-saved registers (R4-R11) onto the current task's stack
    // Using STMDB (Store Multiple Decrement Before)
    __asm volatile ("STMDB r0!, {r4-r11}");

    // Save the updated PSP value into the first element (`pStackPointer`) of the current TCB
    __asm volatile ("LDR r1, =pxCurrentTaskPtr");
    __asm volatile ("LDR r1, [r1]");
    __asm volatile ("STR r0, [r1]"); // TCB->pStackPointer = r0

    /*
     * 2. SWITCH TO THE NEXT TASK
     */
    // Push Link Register (LR / EXC_RETURN) to temporary stack before calling C function
    __asm volatile ("PUSH {r14}");

    // Call helper function to switch task pointers
    __asm volatile ("BL RTOS_SwitchTaskPointer");

    // Pop Link Register back
    __asm volatile ("POP {r14}");

    /*
     * 3. RESTORE THE CONTEXT OF THE NEW TASK
     */
    // Load the new task's TCB pointer
    __asm volatile ("LDR r1, =pxCurrentTaskPtr");
    __asm volatile ("LDR r1, [r1]");

    // Load the `pStackPointer` (new PSP) from the new TCB
    __asm volatile ("LDR r0, [r1]");

    // Restore software-saved registers (R4-R11) from the new task's stack
    // Using LDMIA (Load Multiple Increment After)
    __asm volatile ("LDMIA r0!, {r4-r11}");

    // Update Process Stack Pointer (PSP) to point to the new task's stack
    __asm volatile ("MSR psp, r0");

    // Exit exception using EXC_RETURN (returns to Thread mode using PSP)
    __asm volatile ("BX r14");
}
