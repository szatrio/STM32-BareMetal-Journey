#ifndef STM32F401_TIM_H_
#define STM32F401_TIM_H_

#include <stdint.h>
#include "stm32f401_registers.h"

/**
 * @brief  Initializes TIM2 as a basic timebase generator.
 * @param  psc: Prescaler value to divide the input timer clock.
 * @param  arr: Auto-Reload Register value establishing the overflow threshold.
 */
void TIM2_Timebase_Init(uint16_t psc, uint32_t arr);

/**
 * @brief  Initializes TIM2 with Update Interrupt enabled (NVIC).
 * @param  psc: Prescaler value to divide the input timer clock.
 * @param  arr: Auto-Reload Register value establishing the overflow threshold.
 */
void TIM2_Timebase_IT_Init(uint16_t psc, uint32_t arr);

/**
 * @brief  Polls and clears the Update Interrupt Flag (UIF) of TIM2.
 * @return 1 if an overflow event occurred, 0 otherwise.
 */
uint8_t TIM2_Has_Update_Occurred(void);

#endif
