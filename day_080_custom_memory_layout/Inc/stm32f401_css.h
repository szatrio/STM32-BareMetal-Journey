#ifndef STM32F401_CSS_H
#define STM32F401_CSS_H

#include "stm32f401_registers.h"

bool CSS_Init(void);

//Non-Maskable Interrupt (NMI) Handler for CSS failure detection. Automatically invoked by hardware if the HSE clock fails.
void NMI_Handler(void);

#endif
