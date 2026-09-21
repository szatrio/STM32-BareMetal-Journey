#ifndef STM32F401_I2C_H
#define STM32F401_I2C_H

#include "stm32f401_registers.h"

// Initialize I2C1 Standard Mode (100 kHz) at PB6 (SCL) & PB7 (SDA)
void I2C1_Init(void);
void I2C1_Start(void);
void I2C1_Stop(void);

#endif
