#include "stm32f401_registers.h"
#include "stm32f401_gpio.h"
#include "stm32f401_usart.h"
#include "stm32f401_systick.h"
#include "stm32f401_system.h"
#include "stm32f401_adc.h"
#include <stdio.h>

#define ADC_PIN 0 // PA0 mapped to ADC1_IN0

// add timeout guard limit for eoc polling
#define ADC_TIMEOUT_LOOPS      500000U

/**
 * @brief Helper function to format ADC raw voltage (float) for UART logging
 *        without heavy printf float library (-u _printf_float).
 */
static void Format_ADC_ToString(char *buf, size_t buf_size, uint16_t raw_val, float voltage) {
    uint32_t int_v = (uint32_t)voltage;
    uint32_t dec_v = (uint32_t)((voltage - (float)int_v) * 100.0f);

    snprintf(buf, buf_size, "[ADC1 CH0] Raw: %4u | Voltage: %u.%02u V\r\n",
             raw_val, (unsigned int)int_v, (unsigned int)dec_v);
}

int main(void) {
    char uart_buf[64];
    uint16_t adc_raw = 0;
    float voltage = 0.0f;

    // add status variable for adc error handling
	ADC_Status_t adc_status = ADC_OK;

    // 1. Enable FPU hardware (Required for hard-float ABI execution)
    FPU_Enable();

    // 2. Initialize system core peripherals (SysTick 1 ms tick, GPIO clocks, USART2)
    SysTick_Init(16000);
    RCC_EnableGPIOClock();
    USART2_Init(115200);

    UART_Print("\r\n=== STM32F401 Bare-Metal ADC1 Polling, OVR & Timeout Handling ===\r\n");

    // 3. Configure PA0 Pin as ANALOG Mode (MODER = 0b11) to disable digital Schmitt trigger
    GPIO_Init_t adc_pin_config = {
        .Pin  = ADC_PIN,
        .Mode = GPIO_MODE_ANALOG,
        .Pull = GPIO_PUPDR_NOPULLUPDOWN
    };
    GPIO_Init(GPIOA, &adc_pin_config);

    // 4. Initialize ADC1 Peripheral (Sequence SQ1 = Channel 0, ADON = 1)
    ADC1_Init(0);
    ADC1_StartConversion();

    // 5. Main loop: Safely harvest ADC data with OVR and Timeout guards
	while (1) {

		/* ==================== SIMULATION HOOKS ==================== */
		// UNCOMMENT THE LINE BELOW TO SIMULATE OVERRUN (OVR):
//		 Delay_Simple(10000);

		// UNCOMMENT THE LINE BELOW TO SIMULATE TIMEOUT (Limit = 1 loop):
//		 adc_status = ADC1_ReadPollForConversion(&adc_raw, 1);
		/* ========================================================== */

		adc_status = ADC1_ReadPollForConversion(&adc_raw, ADC_TIMEOUT_LOOPS);

		switch (adc_status) {
			case ADC_OK:
				// Convert 12-bit raw value to dynamic voltage (0.0V - 3.3V)
				voltage = ((float)adc_raw / 4095.0f) * 3.3f;
				Format_ADC_ToString(uart_buf, sizeof(uart_buf), adc_raw, voltage);
				UART_Print(uart_buf);
				break;

			case ADC_ERROR_OVERRUN:
				UART_Print("[ADC WARN] Hardware Overrun (OVR) detected and recovered!\r\n");
				break;

			case ADC_ERROR_TIMEOUT:
				UART_Print("[ADC ERROR] Conversion timed out! Hardware stalled.\r\n");
				break;

			default:
				break;
		}

		// Sampling interval delay
		SysTick_DelayMs(200);
	}
}
