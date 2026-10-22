#include <stdint.h>
#include <stdio.h>
#include "stm32f401_usart.h"
#include "cortex_m4_core.h"
#include "rtos_task.h"

// Sample task function
static void my_sample_task(void) {
    while (1) {
        // Task body
    }
}

// Static TCB instance for our task
static TCB_t sample_task;

int main(void) {
    // Initialize USART2 for diagnostic logs (115200 baud)
    USART2_Init(115200);
    UART_Println("==================================================");
    UART_Println("STM32F401 DAY 71: TCB & STACK ALLOCATION");
    UART_Println("==================================================");

    // 1. Create task and initialize artificial stack frame
    if (RTOS_Task_Create(&sample_task, my_sample_task)) {
        UART_Println("[SUCCESS] Task TCB and stack frame initialized.");
    } else {
        UART_Println("[ERROR] Task creation failed!");
    }

    // 2. Print initial TCB stack pointer address
    char buf[128];
    snprintf(buf, sizeof(buf), "[INFO] TCB pStackPointer: 0x%08lX\r\n",
             (uint32_t)sample_task.pStackPointer);
    UART_Print(buf);

    // 3. Load task's stack pointer into PSP and switch Thread Mode to PSP
    __set_PSP((uint32_t)sample_task.pStackPointer);

    uint32_t control = __get_CONTROL();
    control |= (1U << 1);  // Set SPSEL bit
    __set_CONTROL(control);

    // 4. Validate state after switching to task stack
    snprintf(buf, sizeof(buf), "[INFO] Active PSP: 0x%08lX | CONTROL: 0x%08lX\r\n",
             __get_PSP(), __get_CONTROL());
    UART_Print(buf);

    // Infinite loop running safely on the task's private stack
    while (1) {
        // Main loop
    }
}
