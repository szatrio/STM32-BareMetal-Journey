#ifndef STM32F401_I2C_H
#define STM32F401_I2C_H

#include "stm32f401_registers.h"

// Initialize I2C1 Standard Mode (100 kHz) at PB6 (SCL) & PB7 (SDA)
void I2C1_Init(void);
void I2C_Start(I2C_TypeDef *I2Cx);
void I2C_Stop(I2C_TypeDef *I2Cx);

void I2C_WriteByte(I2C_TypeDef *I2Cx, uint8_t slave_addr, uint8_t data);
void I2C_WriteData(I2C_TypeDef *I2Cx, uint8_t slave_addr, uint8_t *pData, uint32_t len);

#endif
