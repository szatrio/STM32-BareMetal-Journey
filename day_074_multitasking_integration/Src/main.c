#include <stdint.h>
#include <stdio.h>
#include "stm32f401_usart.h"
#include "stm32f401_systick.h"
#include "stm32f401_gpio.h"
#include "cortex_m4_core.h"
#include "rtos_task.h"

// Task 1 entry function
static void task1_entry(void) {
    while (1) {
        UART_Println("[Task 1] Running...");
        // Use Delay_Simple from gpio driver to simulate work
        Delay_Simple(500000UL);
    }
}

// Task 2 entry function
static void task2_entry(void) {
    while (1) {
        UART_Println("[Task 2] Running...");
        // Use Delay_Simple from gpio driver to simulate work
        Delay_Simple(500000UL);
    }
}

// Static TCB instances for Task 1 and Task 2
static TCB_t task1;
static TCB_t task2;

// SysTick Handler: Trigger PendSV to force context switch periodically
void SysTick_Handler(void) {
    // Simple round-robin pointer switcher between Task 1 and Task 2
    if (pxCurrentTaskPtr == &task1) {
        pxNextTaskPtr = &task2;
    } else {
        pxNextTaskPtr = &task1;
    }

    // Trigger PendSV exception to initiate context switch
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}

int main(void) {
    // 1. Initialize USART2 for logs
    USART2_Init(115200);
    UART_Println("==================================================");
    UART_Println("STM32F401 DAY 74: MULTITASKING INTEGRATION");
    UART_Println("==================================================");

    // 2. Initialize Tasks
    RTOS_Task_Create(&task1, task1_entry);
    RTOS_Task_Create(&task2, task2_entry);

    // 3. Set initial active task pointer to Task 1 and next to Task 2
    pxCurrentTaskPtr = &task1;
    pxNextTaskPtr    = &task2;

    // 4. Set PendSV priority to lowest level
    RTOS_SetPendSV_Priority();

    // 5. Initialize SysTick for 1ms heartbeat (assuming SystemCoreClock = 16MHz)
    SysTick_Init(16000000 / 1000);

    UART_Println("[SUCCESS] Multitasking scheduler started.");

    while (1) {
        // Main loop remains empty as tasks handle execution
    }
}
