#include "stm32f401_system.h"
#include "stm32f401_gpio.h"
#include "stm32f401_usart.h"
#include "stm32f401_spi.h"
#include "stm32f401_dma.h"
#include "cortex_m4_dwt.h"
#include <string.h>
#include <stdio.h>

#define SPI1_PORT       GPIOA
#define SPI1_PIN_SCK    5
#define SPI1_PIN_MISO   6
#define SPI1_PIN_MOSI   7
#define SPI1_AF_VALUE   5

#define BUFFER_SIZE     256  // Payload size for DMA transfer test

int main(void) {
    // 1. Enable FPU, system clock to 84 MHz, and initialize DWT for cycle counting
    FPU_Enable();
    RCC_EnableGPIOClock();

    // Initialize DWT with 84 MHz system frequency
    if (!DWT_Delay_Init(84000000UL)) {
        // Handle initialization failure if necessary
    }

    // 2. Initialize USART2 for debugging logs at 115200 baud rate
    USART2_Init(115200);

    UART_Println("==================================================");
    UART_Println("STM32F401 DAY 66: SPI DMA CONFIG & ARCHITECTURE");
    UART_Println("==================================================");

    // Configure SPI Pins & Settings
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

    SPI1_Init();
    SPI1_Config(SPI_BAUDRATE_DIV16, SPI_CPOL_LOW, SPI_CPHA_1EDGE, 1);

    UART_Println("[INFO] SPI1 initialized for DMA configuration.");

    // 3. Prepare Test Buffers
    uint8_t tx_buffer[BUFFER_SIZE];
    uint8_t rx_buffer[BUFFER_SIZE];

    for (int i = 0; i < BUFFER_SIZE; i++) {
        tx_buffer[i] = (uint8_t)(i & 0xFF);
    }
    memset(rx_buffer, 0, BUFFER_SIZE);

    // 4. Configure DMA Streams for SPI1 TX and RX
    // SPI1_TX uses DMA2, Stream 3, Channel 3 (Memory-to-Peripheral)
    // SPI1_RX uses DMA2, Stream 0, Channel 3 (Peripheral-to-Memory)

    // Initialize DMA2 Stream 3 for SPI1 TX (Stream 3, Channel 3)
	DMA_SPI_Stream_Init(&(DMA2->Stream[3]),
						(uint32_t)&(SPI1->DR),
						(uint32_t)tx_buffer,
						BUFFER_SIZE,
						DMA_SxCR_CHSEL_3,
						DMA_SxCR_DIR_M2P);

	// Initialize DMA2 Stream 0 for SPI1 RX (Stream 0, Channel 3)
	DMA_SPI_Stream_Init(&(DMA2->Stream[0]),
						(uint32_t)&(SPI1->DR),
						(uint32_t)rx_buffer,
						BUFFER_SIZE,
						DMA_SxCR_CHSEL_3,
						DMA_SxCR_DIR_P2M);

    // 5. Enable DMA requests generation inside SPI1 (TXDMAEN & RXDMAEN)
    SPI1_DMA_Enable(1, 1);

    UART_Println("[INFO] DMA Streams configured and SPI DMA requests enabled.");
    UART_Println("==================================================");

    while (1) {
        // IDLE loop or background tasks
    }
}
