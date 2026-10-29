#ifndef RTOS_TASK_H
#define RTOS_TASK_H

#include <stdint.h>
#include <stdbool.h>

// Configuration for task stack size and maximum tasks limit in the ring buffer
#define configSTACK_SIZE   128U
#define configMAX_TASKS    4U

// Task execution states
typedef enum {
    TASK_READY = 0,
    TASK_RUNNING,
    TASK_BLOCKED,
    TASK_SUSPENDED
} TaskState_t;

// Task Control Block (TCB)
typedef struct TCB {
    volatile uint32_t *pStackPointer;        // Current stack pointer address
    uint32_t stack[configSTACK_SIZE];        // Private task stack memory
    TaskState_t state;                       // Current task state
    uint32_t priority;                       // Task priority
} TCB_t;

// Global task tracking pointers used by the assembly context switcher
extern volatile TCB_t *pxCurrentTaskPtr;
extern volatile TCB_t *pxNextTaskPtr;

/**
 * @brief Initialize a task TCB and its initial stack frame.
 */
bool RTOS_Task_Create(TCB_t *pxTask, void (*task_code)(void));

/**
 * @brief Register a created task into the scheduler's ring buffer ready queue.
 */
bool RTOS_Task_AddToList(TCB_t *pxTask);

void RTOS_Scheduler(void);
void RTOS_Start(void);
void SVC_Handler(void);

#endif // RTOS_TASK_H
