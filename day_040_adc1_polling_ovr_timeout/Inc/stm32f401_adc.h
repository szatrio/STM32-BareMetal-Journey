#ifndef STM32F401_ADC_H_
#define STM32F401_ADC_H_

#include "stm32f401_registers.h"

typedef enum {
    ADC_OK = 0,
    ADC_ERROR_TIMEOUT,
    ADC_ERROR_OVERRUN
} ADC_Status_t;

// Initialize ADC1 for single conversion on a specified channel
void ADC1_Init(uint8_t channel);

// triggering first and last conversion
void ADC1_StartConversion(void);

// Start conversion, wait until completed, and return 12-bit result (0 - 4095)
uint16_t ADC1_Read(void);

// getting raw data via pointer and this function return the status
ADC_Status_t ADC1_ReadPollForConversion(uint16_t *data_out, uint32_t timeout_loops);

// clear overrun flag
void ADC1_ClearOVR(void);

#endif /* STM32F401_ADC_H_ */
