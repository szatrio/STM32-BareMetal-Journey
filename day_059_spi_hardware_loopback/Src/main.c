#include "stm32f401_system.h"
#include "stm32f401_gpio.h"
#include "stm32f401_usart.h"
#include "stm32f401_spi.h"

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

    UART_Println("===================================");
    UART_Println("  STM32F401 SPI HARDWARE LOOPBACK  ");
    UART_Println("===================================");

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

    // 4. Hardware Loopback Test Execution
    UART_Println("[INFO] Make sure PA7 (MOSI) is connected to PA6 (MISO) via jumper!");

    uint8_t test_data = 0x41; // ASCII 'A'
    uint8_t rx_data = 0;

    UART_Println("[TEST] Sending byte: 0x41 ('A')...");
    rx_data = SPI1_TransmitReceive(test_data);

    // 5. Verification
    if (rx_data == test_data) {
	   UART_Println("[SUCCESS] Loopback PASSED! Data matches.");
    } else {
	   UART_Println("[ERROR] Loopback FAILED! Data mismatch.");
    }

    UART_Println("========================================");
    UART_Println("System entering normal operation loop...");

    while (1) {
        // IDLE
    }
}
