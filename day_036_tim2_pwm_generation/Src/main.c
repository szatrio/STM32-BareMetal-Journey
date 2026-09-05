#include "stm32f401_registers.h"
#include "stm32f401_gpio.h"
#include "stm32f401_usart.h"
#include "stm32f401_tim.h"
#include "stm32f401_systick.h"
#include <stdio.h>

#define LED_PIN 5

int main(void) {
    // 1. Initialize core system hardware (16 MHz HSI / 16000 = 1000 Hz (1 ms tick interrupt))
	SysTick_Init(16000);
    RCC_EnableGPIOClock();
    USART2_Init(115200);

    UART_Print("\r\n=== TIM2 Hardware PWM Generation Started ===\r\n");

    // 2. Configure PA5 (LED) as Alternate Function Mode (AF01 = TIM2_CH1)
    GPIOA->MODER &= ~(3UL << (LED_PIN * 2));
    GPIOA->MODER |=  (GPIO_MODE_ALT << (LED_PIN * 2));

    GPIOA->AFR[0] &= ~(0xFUL << (LED_PIN * 4));
    GPIOA->AFR[0] |=  (1UL << (LED_PIN * 4)); // AF01 mapping for TIM2_CH1

    // 3. Initialize TIM2 PWM: 16 MHz / 16 = 1 MHz Tick Clock (1 us)
    // ARR = 1000 -> PWM Period = 1000 us (1 ms) -> PWM Frequency = 1 kHz
    TIM2_PWM_CH1_Init(16, 1000);

    // 4. Foreground loop: Dynamic duty cycle adjustment (Breathing LED)
    while (1) {
        UART_Print("[Main Loop] Modulating PWM Duty Cycle...\r\n");

        // Fade In: 0% to 100% Duty Cycle
        for (uint32_t duty = 0; duty <= 1000; duty += 10) {
            TIM2_PWM_SetDutyCycle(duty);
            SysTick_DelayMs(5);
        }

        // Fade Out: 100% to 0% Duty Cycle
        for (int32_t duty = 1000; duty >= 0; duty -= 10) {
            TIM2_PWM_SetDutyCycle((uint32_t)duty);
            SysTick_DelayMs(5);
        }
    }
}
