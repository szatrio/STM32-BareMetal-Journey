#ifndef STM32F401_USART_H_
#define STM32F401_USART_H_

#include "stm32f401_registers.h"
#include "stm32f401_ring_buffer.h"

// Initialization & Blocking methods
void USART2_Init(uint32_t baud_rate);
void UART_SendChar(char ch);
char UART_ReceiveChar(void);
void UART_Print(const char *str);
void UART_Println(const char *str);
bool UART_IsDataAvailable(void);

// non-blocking buffered tx
bool USART2_SendChar_Buffered(RingBuffer_t *tx_buffer, char ch);
bool USART2_SendString_Buffered(RingBuffer_t *tx_buffer, const char *str);
void USART2_Flush_TX(RingBuffer_t *tx_buffer);

#endif
