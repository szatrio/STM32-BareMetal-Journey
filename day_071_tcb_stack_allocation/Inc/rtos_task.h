#ifndef RTOS_TASK_H
#define RTOS_TASK_H

#include <stdint.h>
#include <stdbool.h>

// Configuration for task stack size (128 words = 512 bytes)
#define configSTACK_SIZE   128U

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

/**
 * @brief Initialize a task TCB and its initial stack frame.
 */
bool RTOS_Task_Create(TCB_t *pxTask, void (*task_code)(void));

#endif // RTOS_TASK_H
