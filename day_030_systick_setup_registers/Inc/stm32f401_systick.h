#ifndef STM32F401_SYSTICK_H
#define STM32F401_SYSTICK_H

#include "stm32f401_registers.h"

/**
 * @brief  Menginisialisasi SysTick timer untuk interupsi periodik.
 * @param  ticks: Total cycle clocks for countdown (example: 16000 for 1ms on 16MHz)
 */
void SysTick_Init(uint32_t ticks);

#endif
