#include "stm32f401_system.h"
#include "stm32f401_gpio.h"
#include "stm32f401_usart.h"
#include "stm32f401_spi.h"

// --- SPI1 Pin Definitions (PA5: SCK, PA6: MISO, PA7: MOSI) ---
#define SPI1_PORT       GPIOA
#define SPI1_PIN_SCK    5
#define SPI1_PIN_MISO   6
#define SPI1_PIN_MOSI   7
#define SPI1_AF_VALUE   5   // AF5 for SPI1

int main(void) {
    // 1. Enable FPU and initialize system clock (84 MHz)
    FPU_Enable();
    RCC_EnableGPIOClock();

    // 2. Initialize USART2 for debugging logs (115200 baud)
    USART2_Init(115200);

    UART_Println("============================");
    UART_Println("  STM32F401 SPI SR POLLING  ");
    UART_Println("============================");

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
    UART_Println("[INFO] SPI1 peripheral clock enabled.");
    UART_Println("[INFO] SPI pins successfully mapped to AF5.");

    // 5. Configure SPI1 Control Register 1 (CR1) -> Master Mode (1)
    SPI1_Config(SPI_BAUDRATE_DIV16, SPI_CPOL_LOW, SPI_CPHA_1EDGE, 1);
    UART_Println("[INFO] SPI1 CR1 configured (Master, PCLK/16, Mode 0).");

    // 6. Test Polled Transmission using TXE flag
	SPI1_Transmit(0x55);
	UART_Println("[INFO] Dummy data (0x55) sent via TXE polling.");

    UART_Println("========================================");
    UART_Println("System entering normal operation loop...");

    // Main application loop
    while (1) {
        // Keep the system alive;
    }
}
