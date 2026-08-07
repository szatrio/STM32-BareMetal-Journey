#ifndef STM32F401_EXTI_H
#define STM32F401_EXTI_H

#include "stm32f401_registers.h"

void EXTI_PC13_Init(void);
void EXTI15_10_IRQHandler(void); //IRQ NVIC = 40

#endif
