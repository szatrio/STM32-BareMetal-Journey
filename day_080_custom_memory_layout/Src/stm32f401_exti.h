#ifndef STM32F401_EXTI_H
#define STM32F401_EXTI_H

#include "stm32f401_registers.h"

extern volatile uint8_t g_btn_event_flag;

void EXTI_PC13_Init(void);
void EXTI15_10_IRQHandler(void); //IRQ Vector Position = 40

#endif
