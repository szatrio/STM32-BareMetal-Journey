#include "stm32f401_usart.h"
#include "stm32f401_ring_buffer.h"

void USART2_Init(uint32_t baud_rate){
    // 1. Enable Clock for GPIOA and USART2
    RCC_AHB1ENR |= RCC_GPIOA_EN;
    RCC_APB1ENR |= RCC_APB1ENR_USART2EN;

    // 2. Configure PA2 (TX) and PA3 (RX) to Alternate Function Mode (AF7)
    // Clear MODER for PA2 & PA3, then set to 0x02 (GPIO_MODE_ALT)
    GPIOA->MODER &= ~((3UL << (2 * 2)) | (3UL << (3 * 2)));
    GPIOA->MODER |=  ((2UL << (2 * 2)) | (2UL << (3 * 2)));

    // Set AF7 (USART2) in AFR[0] for PA2 and PA3 (AFR[0] bits 8..11 and 12..15)
    GPIOA->AFR[0] &= ~((0x0FUL << 8) | (0x0FUL << 12));
    GPIOA->AFR[0] |=  ((7UL << 8)    | (7UL << 12));

    // 3. Set Baud Rate (115200 at 16 MHz HSI -> BRR = 0x008A)
    if (baud_rate == 115200) {
        USART2->BRR = 0x008A;
    }

    // 4. Enable Transmitter, Receiver, and USART Peripheral
    USART2->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
}

void UART_SendChar(char ch){
    while (!(USART2->SR & USART_SR_TXE));
    USART2->DR = ((uint32_t)ch & 0xFFU);
}

char UART_ReceiveChar(void){
    while (!(USART2->SR & USART_SR_RXNE));
    return (char)(USART2->DR & 0xFFU);
}

void UART_Print(const char *str){
    while (*str != '\0') {
        UART_SendChar(*str);
        str++;
    }
}

void UART_Println(const char *str){
    UART_Print(str);
    UART_SendChar('\r');
    UART_SendChar('\n');
}

bool UART_IsDataAvailable(void){
    return (USART2->SR & USART_SR_RXNE) ? true : false;
}

bool USART2_SendChar_Buffered(RingBuffer_t *tx_buffer, char ch){
    return RingBuffer_Push(tx_buffer, (uint8_t)ch);
}

bool USART2_SendString_Buffered(RingBuffer_t *tx_buffer, const char *str){
    while (*str != '\0'){
        if (!USART2_SendChar_Buffered(tx_buffer, *str)){
            return false;
        }
        str++;
    }
    return true;
}

void USART2_Flush_TX(RingBuffer_t *tx_buffer){
    uint8_t byte_to_send;

    while (!RingBuffer_IsEmpty(tx_buffer)) {
        while (!(USART2->SR & USART_SR_TXE));
        if (RingBuffer_Pop(tx_buffer, &byte_to_send)) {
            USART2->DR = byte_to_send;
        }
    }
}

bool RingBuffer_IsEmpty(const RingBuffer_t *rb){
    return (rb->head == rb->tail);
}
