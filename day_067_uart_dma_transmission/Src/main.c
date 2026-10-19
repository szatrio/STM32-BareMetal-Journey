#include "stm32f401_system.h"
#include "stm32f401_gpio.h"
#include "stm32f401_usart.h"
#include "stm32f401_dma.h"
#include "cortex_m4_dwt.h"
#include <string.h>
#include <stdio.h>

#define UART_DMA_BUFFER_SIZE  64

int main(void) {
    // 1. System & clock init
    FPU_Enable();
    RCC_EnableGPIOClock();
    DWT_Delay_Init(84000000UL);

    // 2. USART2 init (115200 baud)
    USART2_Init(115200);

    UART_Println("==================================================");
    UART_Println("STM32F401 DAY 67: UART DMA TRANSMISSION");
    UART_Println("==================================================");

    // 3. Prepare TX payload
    char uart_tx_buffer[UART_DMA_BUFFER_SIZE];
    snprintf(uart_tx_buffer, sizeof(uart_tx_buffer), "[DMA TX] Hello from STM32F401RE Asynchronous UART!\r\n");
    uint16_t data_len = strlen(uart_tx_buffer);

    // 4. Config DMA1 Stream 6 for USART2 TX (M2P, Channel 4)
    DMA_Stream_Init(&(DMA1->Stream[6]),
                    (uint32_t)&(USART2->DR),
                    (uint32_t)uart_tx_buffer,
                    data_len,
                    DMA_SxCR_CHSEL_4,
                    DMA_SxCR_DIR_M2P);

    // Enable DMA request on USART2 side
    USART_DMA_Enable(USART2);

    UART_Println("[INFO] UART DMA Stream configured and DMAT enabled.");
    UART_Println("==================================================");

    // 5. Profile trigger latency
    DWT->CYCCNT = 0;
    uint32_t start = DWT->CYCCNT;

    // Trigger DMA transfer
    DMA1->Stream[6].CR |= DMA_SxCR_EN;

    uint32_t cycles = DWT->CYCCNT - start;

    char profile_msg[64];
    snprintf(profile_msg, sizeof(profile_msg), "[PROFILING] Trigger cycles: %u\r\n", (unsigned int)cycles);
    UART_Print(profile_msg);

    while (1) {
        // CPU free
    }
}
