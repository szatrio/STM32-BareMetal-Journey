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
    UART_Println("STM32F401 DAY 69: SPI FINAL BENCHMARK & PROFILING");
    UART_Println("==================================================");

    // Configure SPI GPIO pins
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

    // Prepare test buffers
    uint8_t tx_buffer[BUFFER_SIZE];
    uint8_t rx_buffer_polling[BUFFER_SIZE];
    uint8_t rx_buffer_dma[BUFFER_SIZE];

    for (int i = 0; i < BUFFER_SIZE; i++) {
        tx_buffer[i] = (uint8_t)(i & 0xFF);
    }

    // --- 1. BENCHMARK METHOD A: CPU POLLING ---
    memset(rx_buffer_polling, 0, BUFFER_SIZE);

    DWT->CYCCNT = 0;
    uint32_t start_polling = DWT->CYCCNT;

    SPI1_TransmitReceiveStream(tx_buffer, rx_buffer_polling, BUFFER_SIZE);

    uint32_t polling_cycles = DWT->CYCCNT - start_polling;

    // --- 2. BENCHMARK METHOD B: DMA DUAL-STREAM ---
    memset(rx_buffer_dma, 0, BUFFER_SIZE);

    // Config DMA streams
    DMA_Stream_Init(&(DMA2->Stream[3]), (uint32_t)&(SPI1->DR), (uint32_t)tx_buffer, BUFFER_SIZE, DMA_SxCR_CHSEL_3, DMA_SxCR_DIR_M2P);
    DMA_Stream_Init(&(DMA2->Stream[0]), (uint32_t)&(SPI1->DR), (uint32_t)rx_buffer_dma, BUFFER_SIZE, DMA_SxCR_CHSEL_3, DMA_SxCR_DIR_P2M);
    SPI1_DMA_Enable(1, 1);

    DWT->CYCCNT = 0;
    uint32_t start_dma = DWT->CYCCNT;

    // Trigger DMA streams
    DMA2->Stream[0].CR |= DMA_SxCR_EN;
    DMA2->Stream[3].CR |= DMA_SxCR_EN;

    uint32_t dma_trigger_cycles = DWT->CYCCNT - start_dma;

    // Wait until DMA finish
    while (!DMA_GetTCFlag(DMA2, 3));
    while (!DMA_GetTCFlag(DMA2, 0));
    DMA_ClearTCFlag(DMA2, 3);
    DMA_ClearTCFlag(DMA2, 0);

    // --- 3. PRINT COMPARISON REPORT ---
	char log_buf[128];
	snprintf(log_buf, sizeof(log_buf), "[BENCHMARK] Payload Size      : %d Bytes\r\n", BUFFER_SIZE);
	UART_Print(log_buf);
	snprintf(log_buf, sizeof(log_buf), "[BENCHMARK] Polling Cycles    : %lu cycles (%.2f us)\r\n", polling_cycles, (float)polling_cycles / 84.0f);
	UART_Print(log_buf);
	snprintf(log_buf, sizeof(log_buf), "[BENCHMARK] DMA Trigger Cycles: %lu cycles (%.2f us)\r\n", dma_trigger_cycles, (float)dma_trigger_cycles / 84.0f);
	UART_Print(log_buf);

	uint32_t ratio = polling_cycles / dma_trigger_cycles;
	snprintf(log_buf, sizeof(log_buf), "[RESULT] Efficiency Gain      : ~%luux fewer CPU cycles!\r\n", ratio);
	UART_Print(log_buf);
	UART_Println("==================================================");

    while (1) {
        // CPU free
    }
}
