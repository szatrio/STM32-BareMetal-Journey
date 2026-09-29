#include "stm32f401_spi.h"

/**
 * @brief Enable SPI1 peripheral clock on APB2 bus
 */
void SPI1_Init(void) {
    // Enable SPI1 clock (SPI1 is located on the APB2 bus)
    RCC_APB2ENR |= RCC_APB2ENR_SPI1EN;
}
