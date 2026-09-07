#include "stm32f401_registers.h"
#include "stm32f401_gpio.h"
#include "stm32f401_usart.h"
#include "stm32f401_tim.h"
#include "stm32f401_systick.h"
#include "stm32f401_system.h"
#include <stdio.h>

#define LED_PIN 5

/**
 * @brief Helper function to format a float value into integer parts for UART logging
 *        without enabling the heavy printf float linker library (-u _printf_float).
 * @param buf: Target character buffer.
 * @param buf_size: Maximum size of the buffer.
 * @param val: Float value to format.
 */
static void Format_FloatToString(char *buf, size_t buf_size, float val) {
    uint32_t int_part = (uint32_t)val;
    uint32_t dec_part = (uint32_t)((val - (float)int_part) * 10.0f);
    snprintf(buf, buf_size, "[PWM] Duty Cycle: %u.%u%%\r\n", (unsigned int)int_part, (unsigned int)dec_part);
}

int main(void) {
	char uart_buf[64];

	// 1. Enable FPU hardware first before executing any floating-point math
	FPU_Enable();

    // 2. Initialize core system hardware (16 MHz HSI / 16000 = 1000 Hz (1 ms tick interrupt))
	SysTick_Init(16000);
    RCC_EnableGPIOClock();
    USART2_Init(115200);

    UART_Print("\r\n=== TIM2 Hardware PWM Generation Started ===\r\n");

    // 3. Configure PA5 (LED) as Alternate Function Mode (AF01 = TIM2_CH1)
    GPIO_Init_t led_config = {
		.Pin  = LED_PIN,
		.Mode = GPIO_MODE_ALT,
		.Pull = GPIO_PUPDR_NOPULLUPDOWN
	};
	GPIO_Init(GPIOA, &led_config);

	// Set AF01 (0x01) for TIM2_CH1 mapping on PA5
	GPIO_SetAltFunction(GPIOA, LED_PIN, 1);

    // 4. Initialize TIM2 PWM: 16 MHz / 16 = 1 MHz Tick Clock (1 us)
    // ARR = 1000 -> PWM Period = 1000 us (1 ms) -> PWM Frequency = 1 kHz
    TIM2_PWM_CH1_Init(16, 1000);

    // 5. Foreground loop: Dynamic duty cycle adjustment (Breathing LED)
    while (1) {
    	UART_Print("[Main Loop] Modulating PWM Duty Cycle (0.0% - 100.0%)...\r\n");

    	// Fade In: 0.0% to 100.0% Duty Cycle
		for (float duty = 0.0f; duty <= 100.0f; duty += 1.0f) {
			TIM2_PWM_SetDutyCycle(duty);

			Format_FloatToString(uart_buf, sizeof(uart_buf), duty);
			UART_Print(uart_buf);

			SysTick_DelayMs(10);
		}

		// Fade Out: 100.0% to 0.0% Duty Cycle
		for (float duty = 100.0f; duty >= 0.0f; duty -= 1.0f) {
			TIM2_PWM_SetDutyCycle(duty);

			Format_FloatToString(uart_buf, sizeof(uart_buf), duty);
			UART_Print(uart_buf);

			SysTick_DelayMs(10);
		}
    }
}
