#include <stdint.h>
#include <stdio.h>
#include "stm32f401_usart.h"
#include "stm32f401_systick.h"
#include "cortex_m4_core.h"
#include "rtos_task.h"

// Sample task function
static void my_sample_task(void) {
    while (1) {
        // Task body
    }
}

// Static TCB instance
static TCB_t sample_task;

int main(void) {
    // 1. Initialize USART2 for logs
    USART2_Init(115200);
    UART_Println("==================================================");
    UART_Println("STM32F401 DAY 73: ASSEMBLY CONTEXT SWITCHER");
    UART_Println("==================================================");

    // 2. Initialize Task
    RTOS_Task_Create(&sample_task, my_sample_task);

    // 3. Set initial task pointer (Task A is now the active running task)
    pxCurrentTaskPtr = &sample_task;

    // 4. Set PendSV priority to lowest level (from cortex_m4_core.h)
    RTOS_SetPendSV_Priority();

    // 5. Initialize SysTick for 1ms heartbeat (Assuming SystemCoreClock = 16MHz)
    SysTick_Init(16000000 / 1000);

    UART_Println("[SUCCESS] Assembly PendSV context switcher ready.");

    while (1) {
        // Main loop
    }
}
