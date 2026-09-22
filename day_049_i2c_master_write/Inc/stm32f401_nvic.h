#ifndef STM32F401_NVIC_H
#define STM32F401_NVIC_H

#include "stm32f401_registers.h"

// Enable specific IRQ via ISER register
void NVIC_EnableIRQ_Custom(uint8_t irq_num);

// Disable specific IRQ via ICER register
void NVIC_DisableIRQ_Custom(uint8_t irq_num);

// Check if specific IRQ is enabled
uint8_t NVIC_GetEnableIRQ_Custom(uint8_t irq_num);

#endif
