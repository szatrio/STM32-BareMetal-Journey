#include <stdint.h>
#include <stdio.h>
#include "stm32f401_usart.h"
#include "stm32f401_systick.h"
#include "stm32f401_gpio.h"
#include "cortex_m4_core.h"
#include "rtos_task.h"

// TCB instances for 4 concurrent tasks (stress testing max capacity)
TCB_t task1;
TCB_t task2;
TCB_t task3;
TCB_t task4;

// Task 1 entry function (Priority 0 - Highest)
static void task1_entry(void) {
    while (1) {
        _disable_irq();
        UART_Println("[Task 1] Running...");
        _enable_irq();
        RTOS_Task_Delay(500);
    }
}

// Task 2 entry function (Priority 1)
static void task2_entry(void) {
    while (1) {
        _disable_irq();
        UART_Println("[Task 2] Running...");
        _enable_irq();
        RTOS_Task_Delay(1000);
    }
}

// Task 3 entry function (Priority 2)
static void task3_entry(void) {
    while (1) {
        _disable_irq();
        UART_Println("[Task 3] Running...");
        _enable_irq();
        RTOS_Task_Delay(1500);
    }
}

// Task 4 entry function (Priority 3 - Lowest)
static void task4_entry(void) {
    while (1) {
        _disable_irq();
        UART_Println("[Task 4] Running...");
        _enable_irq();
        RTOS_Task_Delay(2000);
    }
}

int main(void) {
    // 1. Initialize USART2 for logging
    USART2_Init(115200);
    UART_Println("==================================================");
    UART_Println("STM32F401 DAY 79: ROUND-ROBIN & PREEMPTION STABILITY TEST");
    UART_Println("==================================================");

    // 2. Create 4 tasks with assigned priorities
    RTOS_Task_Create(&task1, task1_entry, 0);
    RTOS_Task_Create(&task2, task2_entry, 1);
    RTOS_Task_Create(&task3, task3_entry, 2);
    RTOS_Task_Create(&task4, task4_entry, 3);

    // 3. Register all 4 tasks into the scheduler's ring buffer queue
    RTOS_Task_AddToList(&task1);
    RTOS_Task_AddToList(&task2);
    RTOS_Task_AddToList(&task3);
    RTOS_Task_AddToList(&task4);

    // 4. Set PendSV priority to lowest level
    RTOS_SetPendSV_Priority();

    UART_Println("[SUCCESS] Starting 4-Task Stability Stress Test...");

    // 5. Launch RTOS scheduler
    RTOS_Start();

    while (1) {
        // Should never be reached
    }
}
