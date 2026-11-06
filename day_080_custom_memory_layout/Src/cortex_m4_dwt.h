#ifndef CORTEX_M4_DWT_H
#define CORTEX_M4_DWT_H

#include <stdint.h>
#include <stdbool.h>
#include "cortex_m4_core.h"

bool DWT_Delay_Init(uint32_t cpu_freq_hz);
uint32_t DWT_GetCycleCount(void);
void DWT_Delay_us(uint32_t us);
void DWT_Delay_ms(uint32_t ms);

#endif
