#ifndef STM32F401_ADC_H_
#define STM32F401_ADC_H_

#include "stm32f401_registers.h"

// Initialize ADC1 for single conversion on a specified channel
void ADC1_Init(uint8_t channel);

// Start conversion, wait until completed, and return 12-bit result (0 - 4095)
uint16_t ADC1_Read(void);

#endif /* STM32F401_ADC_H_ */
