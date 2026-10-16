#include "stm32f401_system.h"
#include "stm32f401_gpio.h"
#include "stm32f401_usart.h"
#include "stm32f401_spi.h"
#include "cortex_m4_dwt.h"
#include <string.h>
#include <stdio.h>

#define SPI1_PORT       GPIOA
#define SPI1_PIN_SCK    5
#define SPI1_PIN_MISO   6
#define SPI1_PIN_MOSI   7
#define SPI1_AF_VALUE   5

#define BUFFER_SIZE     256  // Larger payload to highlight CPU polling bottleneck

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
    UART_Println("STM32F401 DAY 65: CPU POLLING BOTTLENECK ANALYSIS");
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

    UART_Println("[INFO] SPI1 initialized for polling bottleneck profiling.");

    // 3. Prepare Large Test Buffers
    uint8_t tx_buffer[BUFFER_SIZE];
    uint8_t rx_buffer[BUFFER_SIZE];

    for (int i = 0; i < BUFFER_SIZE; i++) {
        tx_buffer[i] = (uint8_t)(i & 0xFF);
    }
    memset(rx_buffer, 0, BUFFER_SIZE);

    // 4. Benchmark CPU Polling Transfer Performance
    UART_Println("[PROFILING] Starting large-scale SPI transfer using CPU Polling...");

    // Reset DWT cycle counter register directly before measurement
    DWT->CYCCNT = 0UL;
    uint32_t start_cycles = DWT_GetCycleCount();

    // Execute blocking polling transmission
    SPI1_TransmitReceiveStream(tx_buffer, rx_buffer, BUFFER_SIZE);

    uint32_t end_cycles = DWT_GetCycleCount();
    uint32_t cpu_cycles = end_cycles - start_cycles;

    // Calculate execution time in microseconds (84 cycles = 1 us at 84 MHz)
    float execution_time_us = (float)cpu_cycles / 84.0f;

    // Print profiling results
    char log_buf[128];
    snprintf(log_buf, sizeof(log_buf), "[RESULT] Bytes Transferred: %d", BUFFER_SIZE);
    UART_Println(log_buf);
    snprintf(log_buf, sizeof(log_buf), "[RESULT] CPU Cycles Consumed: %lu cycles", cpu_cycles);
    UART_Println(log_buf);
    snprintf(log_buf, sizeof(log_buf), "[RESULT] Execution Time: %.2f us (Blocking Polling)", execution_time_us);
    UART_Println(log_buf);

    UART_Println("==================================================");
    UART_Println("System entering normal operation loop...");

    while (1) {
        // IDLE loop
    }
}
