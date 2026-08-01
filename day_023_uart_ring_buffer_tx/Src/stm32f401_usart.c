#include "stm32f401_usart.h"
#include "stm32f401_ring_buffer.h"

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

void UART_Print(const char *str)
{
    // Loop until last char is '\0' (ASCII 0)
    while (*str != '\0')
    {
        UART_SendChar(*str);
        str++;
    }
}

void UART_Println(const char *str)
{
    UART_Print(str);
    UART_SendChar('\r');
    UART_SendChar('\n');
}

bool UART_IsDataAvailable(void){
    // bit RXNE (Bit 5) in register USART2
    // if bit 1 -> return true
    return (USART2->SR & USART_SR_RXNE) ? true : false;
}

bool USART2_SendChar_Buffered(RingBuffer_t *tx_buffer, char ch){
    return RingBuffer_Push(tx_buffer, (uint8_t)ch);
}

bool USART2_SendString_Buffered(RingBuffer_t *tx_buffer, const char *str){
    while (*str != '\0'){
        if (!USART2_SendChar_Buffered(tx_buffer, *str)){
            return false; // Buffer is full before string done push
        }
        str++;
    }
    return true;
}

void USART2_Flush_TX(RingBuffer_t *tx_buffer){
    uint8_t byte_to_send;

    // Drain data: while buffer is NOT empty AND hardware TXE is ready (1)
    while (!RingBuffer_IsEmpty(tx_buffer) && (USART2->SR & USART_SR_TXE)){
        if (RingBuffer_Pop(tx_buffer, &byte_to_send)){
            USART2->DR = byte_to_send; // pass byte to hardware register
        }
    }
}

bool RingBuffer_IsEmpty(const RingBuffer_t *rb){
    return (rb->head == rb->tail);
}
