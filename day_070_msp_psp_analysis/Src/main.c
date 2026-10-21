#include <stdint.h>
#include <stdio.h>
#include "stm32f401_usart.h"
#include "cortex_m4_core.h"

// Independent stack allocation for PSP (512 bytes)
#define TASK_STACK_SIZE   128U
static uint32_t s_task_stack[TASK_STACK_SIZE];

/**
 * @brief Print current stack pointers and CONTROL register values via UART.
 */
static void print_stack_info(void) {
    uint32_t msp_val = __get_MSP();
    uint32_t psp_val = __get_PSP();
    uint32_t control_val = __get_CONTROL();

    char buf[128];
    snprintf(buf, sizeof(buf), "[INFO] MSP: 0x%08lX | PSP: 0x%08lX | CONTROL: 0x%08lX\r\n",
             msp_val, psp_val, control_val);
    UART_Print(buf);
}

int main(void) {
    // Initialize USART2 for diagnostic logs (115200 baud)
	USART2_Init(115200);
    UART_Println("==================================================");
    UART_Println("STM32F401 DAY 70: MSP & PSP STACK ANALYSIS");
    UART_Println("==================================================");

    // Initial state: Thread mode runs on MSP by default
    UART_Println("[STATE] Initial state (Default Thread Mode -> MSP):");
    print_stack_info();

    // Set top of stack for PSP (grows downward from high address)
    uint32_t psp_top = (uint32_t)(&s_task_stack[TASK_STACK_SIZE]);
    __set_PSP(psp_top);

    // Switch Thread mode stack pointer to PSP by setting CONTROL[1]
    uint32_t control = __get_CONTROL();
    control |= (1U << 1);  // Set SPSEL bit
    __set_CONTROL(control);

    // Validate state after switching to PSP
    UART_Println("[STATE] Switched to PSP via CONTROL register:");
    print_stack_info();

    // Infinite loop running safely on PSP stack
    while (1) {
        // Main application loop
    }
}
