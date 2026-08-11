#ifndef STM32F401_SYSTICK_H
#define STM32F401_SYSTICK_H

#include "stm32f401_registers.h"

// Global tick counter variable accessible across application
extern volatile uint32_t g_ticks;

/**
 * @brief  init SysTick timer for periodic interruption.
 * @param  ticks: Total cycle clocks for countdown (example: 16000 for 1ms on 16MHz)
 */
void SysTick_Init(uint32_t ticks);

/**
 * @brief  Returns the current system tick count in milliseconds.
 * @return Current tick value
 */
uint32_t SysTick_GetTick(void);

#endif
