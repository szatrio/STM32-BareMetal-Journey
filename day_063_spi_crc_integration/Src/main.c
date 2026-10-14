#include "stm32f401_system.h"
#include "stm32f401_gpio.h"
#include "stm32f401_usart.h"
#include "stm32f401_spi.h"
#include "stm32f401_crc.h"
#include <string.h>

#define SPI1_PORT       GPIOA
#define SPI1_PIN_SCK    5
#define SPI1_PIN_MISO   6
#define SPI1_PIN_MOSI   7
#define SPI1_AF_VALUE   5

int main(void) {
    // 1. Enable FPU and initialize system clock to 84 MHz
    FPU_Enable();
    RCC_EnableGPIOClock();

    // 2. Initialize USART2 for debugging logs at 115200 baud rate
    USART2_Init(115200);

    UART_Println("========================================");
    UART_Println("   STM32F401 DAY 63: SPI + CRC INTEGRATION");
    UART_Println("========================================");

    // 3. Initialize peripherals (CRC and SPI1)
    CRC_Init();

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

    UART_Println("[INFO] SPI1 and CRC hardware initialized.");

    // 4. Define Transmission Data Payload
    uint8_t tx_buffer[] = "FIRMWARE_CRC_OK";
    uint8_t rx_buffer[sizeof(tx_buffer)] = {0};
    uint16_t stream_len = sizeof(tx_buffer) - 1;

    // 5. Compute CRC on TX Buffer *before* sending (Sender Checksum)
    uint32_t tx_crc = CRC_CalculateBuffer((uint32_t *)tx_buffer, (stream_len + 3) / 4);
    UART_Println("[INFO] Sender computed CRC checksum for payload.");

    // 6. Execute SPI Stream Transmission (Loopback via PA7 -> PA6)
    UART_Println("[TEST] Transmitting stream via SPI1 loopback...");
    SPI1_TransmitReceiveStream(tx_buffer, rx_buffer, stream_len);

    // 7. Compute CRC on RX Buffer *after* receiving (Receiver Checksum)
    uint32_t rx_crc = CRC_CalculateBuffer((uint32_t *)rx_buffer, (stream_len + 3) / 4);
    UART_Println("[INFO] Receiver computed CRC checksum from RX buffer.");

    // 8. Validate Data Integrity
    if (tx_crc == rx_crc) {
        UART_Println("[SUCCESS] CRC MATCH! Data integrity verified perfectly.");
    } else {
        UART_Println("[ERROR] CRC MISMATCH! Data corruption detected.");
    }

    UART_Println("========================================");
    UART_Println("System entering normal operation loop...");

    while (1) {
        // IDLE loop
    }
}
