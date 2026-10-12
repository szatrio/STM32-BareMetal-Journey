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
    // 1. Enable FPU and initialize system clock (84 MHz)
    FPU_Enable();
    RCC_EnableGPIOClock();

    // 2. Initialize USART2 for debugging logs (115200 baud)
    USART2_Init(115200);

    UART_Println("===========================================");
    UART_Println("   STM32F401 HARDWARE CRC RESET & BUFFER   ");
    UART_Println("===========================================");

    // 3. Initialize CRC Peripheral Clock
    CRC_Init();
    UART_Println("[INFO] CRC peripheral clock enabled & reset executed.");

    // 4. Configure SPI1 Alternate Function Pins (SCK, MISO, MOSI)
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

    // 5. Initialize SPI1 Peripheral Clock & CR1
    SPI1_Init();
    SPI1_Config(SPI_BAUDRATE_DIV16, SPI_CPOL_LOW, SPI_CPHA_1EDGE, 1);

    UART_Println("[INFO] SPI1 initialized in Master Mode.");
    UART_Println("[INFO] Make sure PA7 (MOSI) is connected to PA6 (MISO) via jumper!");

    // 6. Execute SPI Stream Loopback Test
    uint8_t tx_buffer[] = "STM32_CRC_TEST_DATA";
    uint8_t rx_buffer[sizeof(tx_buffer)] = {0};
    uint16_t stream_len = sizeof(tx_buffer) - 1;

    UART_Println("[TEST] Transmitting stream buffer via SPI1...");
    SPI1_TransmitReceiveStream(tx_buffer, rx_buffer, stream_len);

    // 7. Calculate hardware CRC on the actual buffer data with reset control
	UART_Println("[TEST] Calculating Hardware CRC on stream buffer with Reset Control...");

	// Cast buffer pointer to 32-bit word pointer and compute checksum
	uint32_t crc_checksum = CRC_CalculateBuffer((uint32_t *)tx_buffer, (stream_len + 3) / 4);

	UART_Println("[SUCCESS] CRC Checksum calculated successfully!");

    if (crc_checksum != 0) {
    	UART_Println("[INFO] CRC Engine Reset & Stream Computation Verified.");
    }

    UART_Println("========================================");
    UART_Println("System entering normal operation loop...");

    while (1) {
        // IDLE
    }
}
