#include "stm32f401_system.h"
#include "stm32f401_gpio.h"
#include "stm32f401_usart.h"
#include "stm32f401_spi.h"
#include <string.h>

#define SPI1_PORT       GPIOA
#define SPI1_PIN_SCK    5
#define SPI1_PIN_MISO   6
#define SPI1_PIN_MOSI   7
#define SPI1_AF_VALUE   5

int main(void) {
    // 1. Enable FPU and initialize system clock (84 MHz)
    FPU_Enable();
    RCC_EnableGPIOClock();

    // 2. Initialize USART2 for debugging logs (115200 baud)
    USART2_Init(115200);

    UART_Println("========================================");
    UART_Println("  STM32F401 SPI FULL-DUPLEX STREAM TEST ");
    UART_Println("========================================");

    // 3. Configure SPI1 Alternate Function Pins (SCK, MISO, MOSI)
    GPIO_Init_t spi_pin_init = {
        .Mode  = GPIO_MODE_ALT,
        .OType = GPIO_OTYPE_PUSHPULL,
        .Pull  = GPIO_PUPDR_NOPULLUPDOWN
    };

    spi_pin_init.Pin = SPI1_PIN_SCK;
    GPIO_Init(SPI1_PORT, &spi_pin_init);

    spi_pin_init.Pin = SPI1_PIN_MISO;
    GPIO_Init(SPI1_PORT, &spi_pin_init);

    spi_pin_init.Pin = SPI1_PIN_MOSI;
    GPIO_Init(SPI1_PORT, &spi_pin_init);

    // Assign Alternate Function mapping (AF5) to SPI pins
    GPIO_SetAltFunction(SPI1_PORT, SPI1_PIN_SCK,  SPI1_AF_VALUE);
    GPIO_SetAltFunction(SPI1_PORT, SPI1_PIN_MISO, SPI1_AF_VALUE);
    GPIO_SetAltFunction(SPI1_PORT, SPI1_PIN_MOSI, SPI1_AF_VALUE);

    // 4. Initialize SPI1 Peripheral Clock
    SPI1_Init();

    // 5. Configure SPI1 Control Register 1 (CR1) -> Master Mode (1)
    SPI1_Config(SPI_BAUDRATE_DIV16, SPI_CPOL_LOW, SPI_CPHA_1EDGE, 1);

    UART_Println("[INFO] SPI1 initialized in Master Mode.");
    UART_Println("[INFO] Make sure PA7 (MOSI) is connected to PA6 (MISO) via jumper!");

    // 6. Full-Duplex Stream Data Test Execution
    uint8_t tx_buffer[] = "STM32_SPI_STREAM_TEST_OK";
    uint8_t rx_buffer[sizeof(tx_buffer)] = {0};
    uint16_t stream_len = sizeof(tx_buffer) - 1; // Exclude null terminator

    UART_Println("[TEST] Transmitting stream buffer via SPI1_TransmitReceiveStream()...");

    // Execute stream loopback transfer
    SPI1_TransmitReceiveStream(tx_buffer, rx_buffer, stream_len);

    // 7. Verification using memcmp
    if (memcmp(tx_buffer, rx_buffer, stream_len) == 0) {
        UART_Println("[SUCCESS] Stream Loopback PASSED!");
        UART_Println("[INFO] Received data matches TX buffer perfectly:");
        UART_Println((char*)rx_buffer);
    } else {
        UART_Println("[ERROR] Stream Loopback FAILED! Data mismatch.");
    }

    UART_Println("========================================");
    UART_Println("System entering normal operation loop...");

    while (1) {
        // IDLE
    }
}
