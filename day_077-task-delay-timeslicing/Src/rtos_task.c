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

// Master registry to track all created tasks in the system
static volatile TCB_t *pxTaskRegistry[configMAX_TASKS];
static uint8_t uxTotalTasks = 0;

// Global pointers to track current and next running tasks
volatile TCB_t *pxCurrentTaskPtr = NULL;
volatile TCB_t *pxNextTaskPtr    = NULL;

static void RTOS_Task_ExitError(void) {
    while (1) {}
}

bool RTOS_Task_Create(TCB_t *pxTask, void (*task_code)(void)) {
    if (pxTask == NULL || task_code == NULL) return false;

    uint32_t *pStk = &(pxTask->stack[configSTACK_SIZE]);

    *(--pStk) = (1UL << 24);                     // xPSR
    *(--pStk) = (uint32_t)task_code;            // PC
    *(--pStk) = (uint32_t)RTOS_Task_ExitError;  // LR

    for (uint32_t i = 0; i < 5U; i++) *(--pStk) = 0;
    for (uint32_t i = 0; i < 8U; i++) *(--pStk) = 0;

    pxTask->pStackPointer = pStk;
    pxTask->state = TASK_READY;
    pxTask->priority = 1U;
    pxTask->xTicksToDelay = 0U;

    // Register to master task list
    if (uxTotalTasks < configMAX_TASKS) {
        pxTaskRegistry[uxTotalTasks++] = pxTask;
    }

    return true;
}

bool RTOS_Task_AddToList(TCB_t *pxTask) {
    if (pxTask == NULL || xReadyRingBuffer.count >= configMAX_TASKS) return false;

    xReadyRingBuffer.buffer[xReadyRingBuffer.head] = pxTask;
    xReadyRingBuffer.head = (xReadyRingBuffer.head + 1U) % configMAX_TASKS;
    xReadyRingBuffer.count++;

    return true;
}

void RTOS_Task_Delay(uint32_t xTicksToDelay) {
    if (xTicksToDelay == 0U) return;

    _disable_irq();

    pxCurrentTaskPtr->xTicksToDelay = xTicksToDelay;
    pxCurrentTaskPtr->state = TASK_BLOCKED;

    // Remove current task from ready ring buffer tail
    xReadyRingBuffer.tail = (xReadyRingBuffer.tail + 1U) % configMAX_TASKS;
    xReadyRingBuffer.count--;

    if (xReadyRingBuffer.count > 0) {
        pxNextTaskPtr = xReadyRingBuffer.buffer[xReadyRingBuffer.tail];
    } else {
        pxNextTaskPtr = pxCurrentTaskPtr;
    }

    _enable_irq();

    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}

void RTOS_IncTick(void) {
    // Iterate through ALL created tasks in the system registry, not just the ready ring buffer
    for (uint8_t i = 0; i < uxTotalTasks; i++) {
        volatile TCB_t *pxTask = pxTaskRegistry[i];
        if (pxTask != NULL && pxTask->state == TASK_BLOCKED) {
            if (pxTask->xTicksToDelay > 0U) {
                pxTask->xTicksToDelay--;
                if (pxTask->xTicksToDelay == 0U) {
                    pxTask->state = TASK_READY;
                    // Safely re-add woken up task back to the ready ring buffer
                    if (xReadyRingBuffer.count < configMAX_TASKS) {
                        xReadyRingBuffer.buffer[xReadyRingBuffer.head] = pxTask;
                        xReadyRingBuffer.head = (xReadyRingBuffer.head + 1U) % configMAX_TASKS;
                        xReadyRingBuffer.count++;
                    }
                }
            }
        }
    }
}

void RTOS_SwitchTaskPointer(void) {
    pxCurrentTaskPtr = pxNextTaskPtr;
}

__attribute__((naked)) void PendSV_Handler(void) {
    __asm volatile ("MRS r0, psp");
    __asm volatile ("STMDB r0!, {r4-r11}");

    __asm volatile ("LDR r1, =pxCurrentTaskPtr");
    __asm volatile ("LDR r1, [r1]");
    __asm volatile ("STR r0, [r1]");

    __asm volatile ("PUSH {r14}");
    __asm volatile ("BL RTOS_SwitchTaskPointer");
    __asm volatile ("POP {r14}");

    __asm volatile ("LDR r1, =pxCurrentTaskPtr");
    __asm volatile ("LDR r1, [r1]");
    __asm volatile ("LDR r0, [r1]");

    __asm volatile ("LDMIA r0!, {r4-r11}");
    __asm volatile ("MSR psp, r0");

    __asm volatile ("BX r14");
}

void RTOS_Scheduler(void) {
    if (xReadyRingBuffer.count == 0) return;

    volatile TCB_t *pxCurrentTask = xReadyRingBuffer.buffer[xReadyRingBuffer.tail];

    xReadyRingBuffer.tail = (xReadyRingBuffer.tail + 1U) % configMAX_TASKS;
    xReadyRingBuffer.buffer[xReadyRingBuffer.head] = pxCurrentTask;
    xReadyRingBuffer.head = (xReadyRingBuffer.head + 1U) % configMAX_TASKS;

    pxNextTaskPtr = xReadyRingBuffer.buffer[xReadyRingBuffer.tail];

    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}

void RTOS_Start(void) {
    if (xReadyRingBuffer.count == 0) return;

    pxCurrentTaskPtr = xReadyRingBuffer.buffer[xReadyRingBuffer.tail];
    uint8_t nextIndex = (xReadyRingBuffer.tail + 1U) % configMAX_TASKS;
    pxNextTaskPtr = xReadyRingBuffer.buffer[nextIndex];

    SysTick_Init(16000000 / 1000);
    __asm volatile ("CPSIE I");
    __asm volatile ("svc 0");

    while (1) {}
}

__attribute__((naked)) void SVC_Handler(void) {
    __asm volatile (
        "ldr r3, =pxCurrentTaskPtr\n\t"
        "ldr r3, [r3]\n\t"
        "ldr r0, [r3]\n\t"
        "ldmia r0!, {r4-r11}\n\t"
        "msr psp, r0\n\t"
        "movs r0, #2\n\t"
        "msr control, r0\n\t"
        "isb\n\t"
        "ldr r14, =0xFFFFFFFD\n\t"
        "bx r14\n\t"
    );
}
