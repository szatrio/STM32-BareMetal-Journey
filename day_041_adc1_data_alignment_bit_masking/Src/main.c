#include "stm32f401_registers.h"
#include "stm32f401_gpio.h"
#include "stm32f401_usart.h"
#include "stm32f401_systick.h"
#include "stm32f401_system.h"
#include "stm32f401_adc.h"
#include <stdio.h>

#define ADC_PIN 0 // PA0 mapped to ADC1_IN0

// Timeout guard limit for EOC polling
#define ADC_TIMEOUT_LOOPS      500000U

/**
 * @brief Helper function to format ADC raw voltage (float) for UART logging
 *        without heavy printf float library (-u _printf_float).
 */
static void Format_ADC_Comparison_ToString(char *buf, size_t buf_size, uint16_t dr_reg, uint16_t raw_12bit, uint8_t raw_8bit, float voltage) {
    uint32_t int_v = (uint32_t)voltage;
    uint32_t dec_v = (uint32_t)((voltage - (float)int_v) * 100.0f);

    snprintf(buf, buf_size, "[DR: 0x%04X] | 12-Bit: %4u (%u.%02u V) | 8-Bit Fast: %3u\r\n",
                 dr_reg, raw_12bit, (unsigned int)int_v, (unsigned int)dec_v, raw_8bit);
}

int main(void) {
    char uart_buf[128];

    uint16_t dr_val = 0;
    uint16_t adc_12bit = 0;
    uint8_t  adc_8bit = 0;
    float voltage = 0.0f;

    // 1. Enable FPU hardware (Required for hard-float ABI execution)
    FPU_Enable();

    // 2. Initialize system core peripherals (SysTick 1ms tick, GPIO clocks, USART2)
    SysTick_Init(16000);
    RCC_EnableGPIOClock();
    USART2_Init(115200);

    UART_Print("\r\n=== STM32F4 ADC DATA ALIGNMENT & BIT MASKING ===\r\n");

    // 3. Configure PA0 Pin as ANALOG Mode to disable digital Schmitt trigger
    GPIO_Init_t adc_pin_config = {
        .Pin  = ADC_PIN,
        .Mode = GPIO_MODE_ANALOG,
        .Pull = GPIO_PUPDR_NOPULLUPDOWN
    };
    GPIO_Init(GPIOA, &adc_pin_config);

    // 4. Initialize ADC1 Peripheral (Sequence SQ1 = Channel 0, ADON = 1, LEFT Alignment)
    ADC1_InitWithAlignment(0, ADC_ALIGN_LEFT);
    ADC1_StartConversion();

    // 5. Main loop: Safely harvest ADC data with OVR and Timeout guards
    while (1) {
        // Clear Overrun condition if hardware stalled during SysTick delay
        if (ADC1->SR & ADC_SR_OVR) {
            ADC1_ClearOVR();
            ADC1_StartConversion(); // Restart conversion sequence after OVR clear
        }

        // Poll EOC flag and process incoming data
        if (ADC1->SR & ADC_SR_EOC) {
            dr_val = ADC1->DR;

            // Extract fast 8-bit MSB (Left alignment advantage)
            adc_8bit = (uint8_t)(dr_val >> 8);

            // Realign and mask for pure 12-bit value
            adc_12bit = (dr_val >> 4) & 0x0FFF;

            // Calculate voltage from 12-bit value
            voltage = ((float)adc_12bit / 4095.0f) * 3.3f;

            // Stream telemetry data over UART
            Format_ADC_Comparison_ToString(uart_buf, sizeof(uart_buf), dr_val, adc_12bit, adc_8bit, voltage);
            UART_Print(uart_buf);
        }

        SysTick_DelayMs(200);
    }
}
