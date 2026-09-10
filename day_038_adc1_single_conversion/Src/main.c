#include "stm32f401_registers.h"
#include "stm32f401_gpio.h"
#include "stm32f401_usart.h"
#include "stm32f401_systick.h"
#include "stm32f401_system.h"
#include "stm32f401_adc.h"
#include <stdio.h>

#define ADC_PIN 0 // PA0 mapped to ADC1_IN0

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

    // 1. Enable FPU hardware (Required for hard-float ABI execution)
    FPU_Enable();

    // 2. Initialize system core peripherals (SysTick 1 ms tick, GPIO clocks, USART2)
    SysTick_Init(16000);
    RCC_EnableGPIOClock();
    USART2_Init(115200);

    UART_Print("\r\n=== STM32F401 Bare-Metal ADC1 Single Conversion Started ===\r\n");

    // 3. Configure PA0 Pin as ANALOG Mode (MODER = 0b11) to disable digital Schmitt trigger
    GPIO_Init_t adc_pin_config = {
        .Pin  = ADC_PIN,
        .Mode = GPIO_MODE_ANALOG,
        .Pull = GPIO_PUPDR_NOPULLUPDOWN
    };
    GPIO_Init(GPIOA, &adc_pin_config);

    // 4. Initialize ADC1 Peripheral (Sequence SQ1 = Channel 0, ADON = 1)
    ADC1_Init(0);

    // 5. Main loop: Periodically trigger ADC conversion via SWSTART
    while (1) {
        // Read 12-bit raw value (0 - 4095) via polling EOC flag
        adc_raw = ADC1_Read();

        // Convert 12-bit raw value to dynamic voltage (0.0V - 3.3V)
        voltage = ((float)adc_raw / 4095.0f) * 3.3f;

        // Print telemetry over UART
        Format_ADC_ToString(uart_buf, sizeof(uart_buf), adc_raw, voltage);
        UART_Print(uart_buf);

        // Sampling rate: 200 ms interval
        SysTick_DelayMs(200);
    }
}
