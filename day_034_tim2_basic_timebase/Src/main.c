#include "stm32f401_gpio.h"
#include "stm32f401_usart.h"
#include "stm32f401_tim.h"
#include <stdio.h>

#define LED_PIN 5

int main(void) {
    // 1. Initialize core system hardware
    RCC_EnableGPIOClock();

    GPIO_Init_t led_cfg = {
        .Pin   = LED_PIN,
        .Mode  = GPIO_MODE_OUTPUT,
        .OType = GPIO_OTYPE_PUSHPULL,
        .Pull  = GPIO_PUPDR_NOPULLUPDOWN
    };
    GPIO_Init(GPIOA, &led_cfg);

    USART2_Init(115200);
    UART_Print("\r\n=== TIM2 Basic Timebase Started ===\r\n");

    // 2. Initialize TIM2
    // HSI Clock = 16 MHz.
    // PSC = 15999 -> TIM2 Clock = 16 MHz / (15999 + 1) = 1000 Hz (1 ms tick duration)
    // ARR = 499   -> Counter overflows every (499 + 1) = 500 ticks (500 ms interval)
    TIM2_Timebase_Init(15999, 499);

    uint32_t toggle_count = 0;

    while (1) {
        // Poll TIM2 hardware status flag without blocking CPU execution
        if (TIM2_Has_Update_Occurred()) {
            GPIOA->ODR ^= (1U << LED_PIN); // Toggle Heartbeat LED
            toggle_count++;

            char msg[64];
            snprintf(msg, sizeof(msg), "[TIM2] Overflow Event #%lu | LED Toggled\r\n", toggle_count);
            UART_Print(msg);
        }
    }
}
