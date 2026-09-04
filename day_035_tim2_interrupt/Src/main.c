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
    UART_Print("\r\n=== TIM2 Hardware Interrupt Started ===\r\n");

    // 2. Initialize TIM2 with Interrupt Enabled (NVIC)
    // HSI Clock = 16 MHz.
    // PSC = 15999 -> TIM2 Clock = 1000 Hz (1 ms tick duration)
    // ARR = 499   -> Interrupt triggers every 500 ms
    TIM2_Timebase_IT_Init(15999, 499);

    while (1) {
		UART_Print("[Main Loop] CPU is busy on other task...\r\n");

		// Blocking delay for heavy task simulation
		Delay_Simple(200000000);
	}
}
