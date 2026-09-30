#include "stm32f401_spi.h"

/**
 * @brief Enable SPI1 peripheral clock on APB2 bus
 */
void SPI1_Init(void) {
    // Enable SPI1 clock (SPI1 is located on the APB2 bus)
    RCC_APB2ENR |= RCC_APB2ENR_SPI1EN;
}

/**
 * @brief Configure SPI1 Control Register 1 (CR1) parameters
 * @param baud_rate_prescaler: Baud rate control bits
 * @param cpol: Clock polarity (0: Idle LOW, 1: Idle HIGH)
 * @param cpha: Clock phase (0: First edge, 1: Second edge)
 */
void SPI1_Config(uint32_t baud_rate_prescaler, uint8_t cpol, uint8_t cpha) {
    // Note: SPI1 must be disabled (SPE = 0) before modifying configuration registers
    SPI1->CR1 &= ~(1UL << 6); // Clear SPE bit

    uint32_t tmpreg = SPI1->CR1;

    // 1. Clear Baud Rate control bits (BR[2:0] are at bits 5:3)
    tmpreg &= ~(0x7UL << 3);
    tmpreg |= (baud_rate_prescaler & (0x7UL << 3));

    // 2. Configure Clock Polarity (CPOL at bit 1)
    if (cpol) {
        tmpreg |= (1UL << 1);
    } else {
        tmpreg &= ~(1UL << 1);
    }

    // 3. Configure Clock Phase (CPHA at bit 0)
    if (cpha) {
        tmpreg |= (1UL << 0);
    } else {
        tmpreg &= ~(1UL << 0);
    }

    // 4. Set as Master Mode (MSTR at bit 2)
    tmpreg |= (1UL << 2);

    // 5. Software slave management (SSM = 1, SSI = 1 to prevent Master Mode Fault)
    tmpreg |= (1UL << 9) | (1UL << 8);

    // Write back configuration to CR1 register
    SPI1->CR1 = tmpreg;

    // Enable SPI peripheral (SPE = 1 at bit 6)
    SPI1->CR1 |= (1UL << 6);
}
