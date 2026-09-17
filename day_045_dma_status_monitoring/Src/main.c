#include <stdio.h>
#include "stm32f401_registers.h"
#include "stm32f401_gpio.h"
#include "stm32f401_usart.h"
#include "stm32f401_systick.h"
#include "stm32f401_system.h"
#include "stm32f401_adc.h"
#include "stm32f401_dma.h"

#define ADC_PIN      0 // PA0 (ADC1_IN0)
#define DMA_BUF_SIZE 1

// Target RAM buffer where DMA automatically stores ADC conversion results in the background
static uint16_t adc_dma_buffer[DMA_BUF_SIZE] = {0};

int main(void) {
    // 1. Initialize System Core & Peripherals
    FPU_Enable();
    SysTick_Init(16000);
    RCC_EnableGPIOClock();
    USART2_Init(115200);

    UART_Print("\r\n=== DMA STATUS MONITORING AND TC FLAG CLEARING ===\r\n");

    // 2. Configure PA0 Pin in Analog Mode
    GPIO_Init_t adc_pin_config = {
        .Pin  = ADC_PIN,
        .Mode = GPIO_MODE_ANALOG,
        .Pull = GPIO_PUPDR_NOPULLUPDOWN
    };
    GPIO_Init(GPIOA, &adc_pin_config);

    // 3. Initialize ADC1 (Right-aligned 12-bit + Continuous Mode)
    ADC1_InitWithAlignment(0, ADC_ALIGN_RIGHT);
    ADC1->CR2 |= ADC_CR2_CONT; // Enable continuous conversion mode

    // 4. Initialize DMA2 Stream 0 Channel 0 (Peripheral ADC1->DR to Memory Buffer)
    DMA_Stream_Init(&(DMA2->Stream[0]),
                    (uint32_t)&(ADC1->DR),
                    (uint32_t)adc_dma_buffer,
                    DMA_BUF_SIZE,
                    DMA_SxCR_CHSEL_0);

    // 5. [DAY 44] Enable Hardware DMA Trigger Request on ADC1
    ADC1_EnableDMA();

    // 6. Trigger the Initial ADC Conversion
    ADC1_StartConversion();

    // 7. Main Loop: CPU directly reads RAM buffer without polling ADC1->SR (EOC flag)
    while (1) {
    	// checking whether DMA hardware has completed transferring data to RAM
    	if (DMA_GetTCFlag(DMA2, 0)) {

    		// Getting dara from EAM buffer safely
    		uint16_t raw_adc = adc_dma_buffer[0];
			float voltage = ((float)raw_adc / 4095.0f) * 3.3f;

			char log_buf[80];
			snprintf(log_buf, sizeof(log_buf),
					 "[VALIDATED DMA TRANSFER] RAW: %4u | Voltage: %.2fV\r\n",
					 raw_adc, voltage);
			UART_Print(log_buf);

			// Clear flag for detecting next transfer cycle
			DMA_ClearTCFlag(DMA2, 0);
    	}
    }
}
