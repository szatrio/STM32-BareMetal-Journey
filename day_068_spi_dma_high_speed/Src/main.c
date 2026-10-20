#include "stm32f401_system.h"
#include "stm32f401_gpio.h"
#include "stm32f401_usart.h"
#include "stm32f401_spi.h"
#include "stm32f401_dma.h"
#include "cortex_m4_dwt.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#define SPI1_PORT       GPIOA
#define SPI1_PIN_SCK    5
#define SPI1_PIN_MISO   6
#define SPI1_PIN_MOSI   7
#define SPI1_AF_VALUE   5

#define BUFFER_SIZE     256

int main(void) {
    // Init system, clock, and DWT
    FPU_Enable();
    RCC_EnableGPIOClock();
    DWT_Delay_Init(84000000UL);

    // Init UART for debug log
    USART2_Init(115200);

    UART_Println("==================================================");
    UART_Println("STM32F401 DAY 68: HIGH-SPEED SPI DMA TRANSFER");
    UART_Println("==================================================");

    // Configure SPI GPIO pins (SCK, MISO, MOSI)
    GPIO_Init_t spi_pin_init = {
        .Mode  = GPIO_MODE_ALT,
        .OType = GPIO_OTYPE_PUSHPULL,
        .Pull  = GPIO_PUPDR_NOPULLUPDOWN
    };

    spi_pin_init.Pin = SPI1_PIN_SCK;  GPIO_Init(SPI1_PORT, &spi_pin_init);
    spi_pin_init.Pin = SPI1_PIN_MISO; GPIO_Init(SPI1_PORT, &spi_pin_init);
    spi_pin_init.Pin = SPI1_PIN_MOSI; GPIO_Init(SPI1_PORT, &spi_pin_init);

    GPIO_SetAltFunction(SPI1_PORT, SPI1_PIN_SCK,  SPI1_AF_VALUE);
    GPIO_SetAltFunction(SPI1_PORT, SPI1_PIN_MISO, SPI1_AF_VALUE);
    GPIO_SetAltFunction(SPI1_PORT, SPI1_PIN_MOSI, SPI1_AF_VALUE);

    // Init SPI1 as master
    SPI1_Init();
    SPI1_Config(SPI_BAUDRATE_DIV16, SPI_CPOL_LOW, SPI_CPHA_1EDGE, 1);

    UART_Println("[INFO] SPI1 initialized.");

    // Prepare TX and RX buffers
    uint8_t tx_buffer[BUFFER_SIZE];
    uint8_t rx_buffer[BUFFER_SIZE];

    for (int i = 0; i < BUFFER_SIZE; i++) {
        tx_buffer[i] = (uint8_t)(i & 0xFF);
    }
    memset(rx_buffer, 0, BUFFER_SIZE);

    // Config DMA2 Stream 3 for SPI1 TX (Memory-to-Peripheral, Channel 3)
    DMA_Stream_Init(&(DMA2->Stream[3]),
                    (uint32_t)&(SPI1->DR),
                    (uint32_t)tx_buffer,
                    BUFFER_SIZE,
                    DMA_SxCR_CHSEL_3,
                    DMA_SxCR_DIR_M2P);

    // Config DMA2 Stream 0 for SPI1 RX (Peripheral-to-Memory, Channel 3)
    DMA_Stream_Init(&(DMA2->Stream[0]),
                    (uint32_t)&(SPI1->DR),
                    (uint32_t)rx_buffer,
                    BUFFER_SIZE,
                    DMA_SxCR_CHSEL_3,
                    DMA_SxCR_DIR_P2M);

    // Enable DMA request lines on SPI1 side
    SPI1_DMA_Enable(1, 1);

    UART_Println("[INFO] DMA streams configured & SPI DMA enabled.");
    UART_Println("==================================================");

    // Profile trigger latency
    DWT->CYCCNT = 0;
    uint32_t start = DWT->CYCCNT;

    // Trigger RX first, then TX
    DMA2->Stream[0].CR |= DMA_SxCR_EN;
    DMA2->Stream[3].CR |= DMA_SxCR_EN;

    uint32_t cycles = DWT->CYCCNT - start;

    char profile_msg[64];
    snprintf(profile_msg, sizeof(profile_msg), "[PROFILING] Trigger cycles: %u\r\n", (unsigned int)cycles);
    UART_Print(profile_msg);

    // Wait until both streams finish transfer (using helper or checking flag)
    while (!DMA_GetTCFlag(DMA2, 3)); // Wait TX (Stream 3)
    while (!DMA_GetTCFlag(DMA2, 0)); // Wait RX (Stream 0)

    // Clear flags
    DMA_ClearTCFlag(DMA2, 3);
    DMA_ClearTCFlag(DMA2, 0);

    UART_Println("[INFO] SPI DMA Transfer Completed!");

    // Verify loopback data integrity (MOSI bridged to MISO)
    bool match = true;
    for (int i = 0; i < BUFFER_SIZE; i++) {
        if (rx_buffer[i] != tx_buffer[i]) {
            match = false;
            break;
        }
    }

    if (match) {
        UART_Println("[SUCCESS] Data integrity verified! RX matches TX.");
    } else {
        UART_Println("[ERROR] Data mismatch in loopback!");
    }

    while (1) {
        // CPU free
    }
}
