#include "stm32f401_usart.h"

void USART2_Init(uint32_t baud_rate)
{
    // 1. Enable Clock USART2
    RCC_APB1ENR |= RCC_APB1ENR_USART2EN;

    // 2. Set Baud Rate
    if (baud_rate == 115200) {
        USART2->BRR = 0x008A;
    }

    // 3. Enable TE & UE via Struct Pointer USART2
    USART2->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
}

void UART_SendChar(char ch)
{
    // Wait until Transmit Data Register (TDR) is empty
    while (!(USART2->SR & USART_SR_TXE)) {
        // Wait / Poll TXE flag
    }

    // Write character byte to Data Register
    USART2->DR = ((uint32_t)ch & 0xFFU);
}

char UART_ReceiveChar(void)
{
    // 1. Polling RXNE: Waiting input from PC (RXNE == 1)
    while (!(USART2->SR & USART_SR_RXNE));

    // 2. Reading DR (reset bit RXNE to 0 automatically)
    return (char)(USART2->DR & 0xFF);
}
