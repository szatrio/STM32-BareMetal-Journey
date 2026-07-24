#ifndef STM32F401_USART_H_
#define STM32F401_USART_H_

#include "stm32f401_registers.h"

void USART2_Init(uint32_t baud_rate);
void UART_SendChar(char ch);
char UART_ReceiveChar(void);

#endif
