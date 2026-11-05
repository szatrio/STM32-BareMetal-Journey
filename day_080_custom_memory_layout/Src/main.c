/**
 * =================================================================
 * STM32F401RE Custom Linker Script Integration - Day 80
 * =================================================================
 */

#include <stdint.h>
#include <stdio.h>
#include "stm32f401_usart.h"
#include "stm32f401_gpio.h"

int main(void) {
    /* Initialize system peripherals and logging interface */
    USART2_Init(115200);

    UART_Println("==================================================");
    UART_Println("STM32F401 DAY 80: CUSTOM LINKER SCRIPT MEMORY LAYOUT");
    UART_Println("==================================================");

    /* Main application loop */
    while (1) {
        /* Running on custom memory boundaries */
    }
}
