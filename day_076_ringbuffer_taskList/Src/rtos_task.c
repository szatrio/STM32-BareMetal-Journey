#include "rtos_task.h"
#include "stm32f401_systick.h"
#include "cortex_m4_core.h"
#include <stddef.h>

// Ring Buffer structure for task ready queue management
typedef struct {
    volatile TCB_t *buffer[configMAX_TASKS];
    uint8_t head;
    uint8_t tail;
    uint8_t count;
} TaskRingBuffer_t;

static TaskRingBuffer_t xReadyRingBuffer = {
    .head = 0,
    .tail = 0,
    .count = 0
};

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

    // 1. Simulate hardware-saved registers (pushed automatically by Cortex-M4 on exception)
    // Order (bottom to top of stack): xPSR, PC, LR, R12, R3, R2, R1, R0
    *(--pStk) = (1UL << 24);                     // xPSR (Thumb state bit set)
    *(--pStk) = (uint32_t)task_code;            // PC (Task entry function address)
    *(--pStk) = (uint32_t)RTOS_Task_ExitError;  // LR (Fallback on return)

    // R12, R3, R2, R1, R0 initialized to 0
    for (uint32_t i = 0; i < 5U; i++) {
        *(--pStk) = 0x00000000UL;
    }

    // 2. Simulate software-saved registers (pushed manually by context switcher)
    // Order: R11, R10, R9, R8, R7, R6, R5, R4
    for (uint32_t i = 0; i < 8U; i++) {
        *(--pStk) = 0x00000000UL;
    }

    // Save finalized stack pointer position inside TCB
    pxTask->pStackPointer = pStk;
    pxTask->state = TASK_READY;
    pxTask->priority = 1U;

    return true;
}

/**
 * @brief Register a created task into the ring buffer queue (FIFO).
 */
bool RTOS_Task_AddToList(TCB_t *pxTask) {
    if (pxTask == NULL || xReadyRingBuffer.count >= configMAX_TASKS) {
        return false;
    }

    xReadyRingBuffer.buffer[xReadyRingBuffer.head] = pxTask;
    xReadyRingBuffer.head = (xReadyRingBuffer.head + 1U) % configMAX_TASKS;
    xReadyRingBuffer.count++;

    return true;
}

/**
 * @brief Helper function to switch active task pointers.
 */
void RTOS_SwitchTaskPointer(void) {
    pxCurrentTaskPtr = pxNextTaskPtr;
}

/**
 * @brief PendSV Exception Handler (Naked Function)
 *        Handles low-level context switching between tasks in Assembly.
 */
__attribute__((naked)) void PendSV_Handler(void) {
    // 1. SAVE CONTEXT OF CURRENT TASK
    __asm volatile ("MRS r0, psp");
    __asm volatile ("STMDB r0!, {r4-r11}");

    __asm volatile ("LDR r1, =pxCurrentTaskPtr");
    __asm volatile ("LDR r1, [r1]");
    __asm volatile ("STR r0, [r1]"); // TCB->pStackPointer = r0

    // 2. SWITCH TO NEXT TASK
    __asm volatile ("PUSH {r14}");
    __asm volatile ("BL RTOS_SwitchTaskPointer");
    __asm volatile ("POP {r14}");

    // 3. RESTORE CONTEXT OF NEW TASK
    __asm volatile ("LDR r1, =pxCurrentTaskPtr");
    __asm volatile ("LDR r1, [r1]");
    __asm volatile ("LDR r0, [r1]"); // Load new task stack pointer

    __asm volatile ("LDMIA r0!, {r4-r11}");
    __asm volatile ("MSR psp, r0");

    __asm volatile ("BX r14");
}

/**
 * @brief Round-Robin Scheduler using Ring Buffer circular rotation.
 */
void RTOS_Scheduler(void) {
    if (xReadyRingBuffer.count == 0) {
        return;
    }

    // 1. Extract the current running task from tail and push it back to the head (Circular rotation)
    volatile TCB_t *pxCurrentTask = xReadyRingBuffer.buffer[xReadyRingBuffer.tail];

    xReadyRingBuffer.tail = (xReadyRingBuffer.tail + 1U) % configMAX_TASKS;
    xReadyRingBuffer.buffer[xReadyRingBuffer.head] = pxCurrentTask;
    xReadyRingBuffer.head = (xReadyRingBuffer.head + 1U) % configMAX_TASKS;

    // 2. Assign the next task to run
    pxNextTaskPtr = xReadyRingBuffer.buffer[xReadyRingBuffer.tail];

    // 3. Trigger PendSV exception to initiate context switch
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}

/**
 * @brief Starts the RTOS scheduler, initializes SysTick, and launches the first task via SVC.
 */
void RTOS_Start(void) {
    if (xReadyRingBuffer.count == 0) {
        return;
    }

    // 1. Set initial pointers explicitly from the ring buffer queue
    pxCurrentTaskPtr = xReadyRingBuffer.buffer[xReadyRingBuffer.tail];

    // Simulate a preliminary rotation for next task setup
    uint8_t nextIndex = (xReadyRingBuffer.tail + 1U) % configMAX_TASKS;
    pxNextTaskPtr    = xReadyRingBuffer.buffer[nextIndex];

    // 2. Initialize SysTick for 1ms heartbeats (16MHz clock)
    SysTick_Init(16000000 / 1000);

    // 3. Enable global interrupts
    __asm volatile ("CPSIE I");

    // 4. Trigger Supervisor Call (SVC) to execute the first task switch in Handler mode
    __asm volatile ("svc 0");

    while (1) {
        // Should never be reached
    }
}

/**
 * @brief SVC Exception Handler (Naked Function)
 *        Used to launch the very first task safely from Handler mode.
 */
__attribute__((naked)) void SVC_Handler(void) {
    __asm volatile (
        // 1. Get the current task's stack pointer
        "ldr r3, =pxCurrentTaskPtr\n\t"
        "ldr r3, [r3]\n\t"             // r3 = &pxCurrentTaskPtr
        "ldr r0, [r3]\n\t"             // r0 = current task stack pointer

        // 2. Restore software-saved registers (R4-R11)
        "ldmia r0!, {r4-r11}\n\t"

        // 3. Update Process Stack Pointer (PSP) to point to the hardware frame
        "msr psp, r0\n\t"

        // 4. Switch Thread Mode stack from MSP to PSP using CONTROL register
        "movs r0, #2\n\t"
        "msr control, r0\n\t"
        "isb\n\t"

        // 5. Load EXC_RETURN and exit exception to enter the first task
        "ldr r14, =0xFFFFFFFD\n\t"     // Return to Thread mode using PSP
        "bx r14\n\t"
    );
}
