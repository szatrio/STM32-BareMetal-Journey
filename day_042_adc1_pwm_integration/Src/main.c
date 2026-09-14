#include "stm32f401_registers.h"
#include "stm32f401_gpio.h"
#include "stm32f401_usart.h"
#include "stm32f401_systick.h"
#include "stm32f401_system.h"
#include "stm32f401_adc.h"
#include "stm32f401_tim.h"
#include <stdio.h>

#define ADC_PIN 0 // PA0 mapped to ADC1_IN0
#define PWM_PIN 1 // PA1 mapped to TIM2_CH2 (PWM Output)

// Timeout guard limit for EOC polling
#define ADC_TIMEOUT_LOOPS      500000U

/**
 * @brief Helper function to format telemetry data for UART logging
 */
static void Format_ADC_PWM_ToString(char *buf, size_t buf_size, uint16_t adc_12bit, float voltage, float duty_cycle) {
    uint32_t int_v = (uint32_t)voltage;
    uint32_t dec_v = (uint32_t)((voltage - (float)int_v) * 100.0f);

    uint32_t int_d = (uint32_t)duty_cycle;
    uint32_t dec_d = (uint32_t)((duty_cycle - (float)int_d) * 100.0f);

    snprintf(buf, buf_size, "[ADC: %4u | %u.%02uV] ---> [PWM Duty: %u.%02u%%]\r\n",
                 adc_12bit, (unsigned int)int_v, (unsigned int)dec_v,
                 (unsigned int)int_d, (unsigned int)dec_d);
}

int main(void) {
    char uart_buf[128];

    uint16_t dr_val = 0;
	uint16_t adc_12bit = 0;
	float voltage = 0.0f;
	float duty_cycle = 0.0f;

    // 1. Enable FPU hardware (Required for hard-float ABI execution)
    FPU_Enable();

    // 2. Initialize system core peripherals (SysTick 1ms tick, GPIO clocks, USART2)
    SysTick_Init(16000);
    RCC_EnableGPIOClock();
    USART2_Init(115200);

    UART_Print("\r\n=== STM32F4 ADC TO PWM INTEGRATION ===\r\n");

    // 3. Configure PA0 Pin as ANALOG Mode to disable digital Schmitt trigger
    GPIO_Init_t adc_pin_config = {
        .Pin  = ADC_PIN,
        .Mode = GPIO_MODE_ANALOG,
        .Pull = GPIO_PUPDR_NOPULLUPDOWN
    };
    GPIO_Init(GPIOA, &adc_pin_config);

    // 4. Configure PA1 Pin as Alternate Function for PWM Output (TIM2_CH2)
	GPIO_Init_t pwm_pin_config = {
		.Pin  = PWM_PIN,
		.Mode = GPIO_MODE_ALT,
		.Pull = GPIO_PUPDR_NOPULLUPDOWN
	};
	GPIO_Init(GPIOA, &pwm_pin_config);
	GPIO_SetAltFunction(GPIOA, PWM_PIN, 1);

    // 5. Initialize ADC1 Peripheral (Sequence SQ1 = Channel 0, ADON = 1, LEFT Alignment)
    ADC1_InitWithAlignment(0, ADC_ALIGN_LEFT);
    ADC1_StartConversion();

    // 6. Initialize TIM2 PWM Channel 2
	// Frequency formula: 16 MHz / ((PSC + 1) * (ARR + 1)) -> 16MHz / (16 * 1000) = 1 kHz
    TIM2_PWM_Init(2, 16, 1000);

	// 7. Main loop: Harvest ADC, map to PWM, and stream telemetry
	while (1) {
		// Clear Overrun condition if hardware stalled during delay
		if (ADC1->SR & ADC_SR_OVR) {
			ADC1_ClearOVR();
			ADC1_StartConversion();
		}

		// Poll EOC flag
		if (ADC1->SR & ADC_SR_EOC) {
			dr_val = ADC1->DR;

			// Realign and mask for pure 12-bit value (0 - 4095)
			adc_12bit = (dr_val >> 4) & 0x0FFF;

			// Calculate voltage for telemetry
			voltage = ((float)adc_12bit / 4095.0f) * 3.3f;

			// Map 12-bit ADC range to PWM Duty Cycle percentage (0.0% - 100.0%)
			duty_cycle = ((float)adc_12bit / 4095.0f) * 100.0f;

			// Apply duty cycle to hardware Timer 2 Channel 2 register
			TIM2_PWM_SetDutyCycle(2, duty_cycle);

			// Stream telemetry data over UART
			Format_ADC_PWM_ToString(uart_buf, sizeof(uart_buf), adc_12bit, voltage, duty_cycle);
			UART_Print(uart_buf);
		}

		SysTick_DelayMs(100);
	}
}

