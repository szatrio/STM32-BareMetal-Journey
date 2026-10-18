#ifndef STM32F401_CRC_H
#define STM32F401_CRC_H

#include <stdint.h>
#include "stm32f401_registers.h"

// Function prototypes for Day 61
void CRC_Init(void);
void CRC_Reset(void);
uint32_t CRC_Calculate(uint32_t data);
uint32_t CRC_CalculateBuffer(uint32_t *pBuffer, uint32_t length);

#endif // STM32F401_CRC_H
