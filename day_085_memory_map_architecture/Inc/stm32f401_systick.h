#ifndef STM32F401_SYSTICK_H
#define STM32F401_SYSTICK_H

#include "stm32f401_registers.h"

// Global tick counter variable accessible across application
extern volatile uint32_t g_ticks;

void SysTick_Init(uint32_t ticks);
uint32_t SysTick_GetTick(void);
void SysTick_DelayMs(uint32_t ms);

#endif
