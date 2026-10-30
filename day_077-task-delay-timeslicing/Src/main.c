#include <stdint.h>
#include <stdio.h>
#include "stm32f401_usart.h"
#include "stm32f401_systick.h"
#include "stm32f401_gpio.h"
#include "cortex_m4_core.h"
#include "rtos_task.h"

// TCB instances for Task 1 and Task 2
TCB_t task1;
TCB_t task2;

// Task 1 entry function
static void task1_entry(void) {
    while (1) {
        _disable_irq();
        UART_Println("[Task 1] Running...");
        _enable_irq();
        RTOS_Task_Delay(500); // Replaced busy-wait with RTOS blocking delay
    }
}

// Task 2 entry function
static void task2_entry(void) {
    while (1) {
        _disable_irq();
        UART_Println("[Task 2] Running...");
        _enable_irq();
        RTOS_Task_Delay(500); // Replaced busy-wait with RTOS blocking delay
    }
}

int main(void) {
    // 1. Initialize USART2 for logging
    USART2_Init(115200);
    UART_Println("==================================================");
    UART_Println("STM32F401 DAY 77: TASK DELAY & BLOCKED STATE");
    UART_Println("==================================================");

    // 2. Create tasks
    RTOS_Task_Create(&task1, task1_entry);
    RTOS_Task_Create(&task2, task2_entry);

    // 3. Register tasks into the scheduler's ring buffer queue
    RTOS_Task_AddToList(&task1);
    RTOS_Task_AddToList(&task2);

    // 4. Set PendSV priority to lowest level
    RTOS_SetPendSV_Priority();

    UART_Println("[SUCCESS] Starting RTOS with Blocked State...");

    // 5. Launch RTOS scheduler
    RTOS_Start();

    while (1) {
        // Should never be reached
    }
}
