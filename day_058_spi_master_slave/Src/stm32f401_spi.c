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
 * @param master_mode: 1 for Master mode, 0 for Slave mode
 */
void SPI1_Config(uint32_t baud_rate_prescaler, uint8_t cpol, uint8_t cpha, uint8_t master_mode) {
    // Note: SPI1 must be disabled (SPE = 0) before modifying configuration registers
    SPI1->CR1 &= ~(1UL << 6); // Clear SPE bit

    uint32_t tmpreg = SPI1->CR1;

    // 1. Clear Baud Rate control bits (BR[2:0] are at bits 5:3)
    tmpreg &= ~(0x7UL << 3);
    tmpreg |= (baud_rate_prescaler & (0x7UL << 3));

    // 2. Configure Clock Polarity (CPOL at bit 1) & Phase (CPHA at bit 0)
	tmpreg = cpol ? (tmpreg | (1UL << 1)) : (tmpreg & ~(1UL << 1));
	tmpreg = cpha ? (tmpreg | (1UL << 0)) : (tmpreg & ~(1UL << 0));

	// 3. Configure Master vs Slave mode (MSTR, SSM, SSI)
	if (master_mode) {
		tmpreg |= (1UL << 2);                 // MSTR = 1 (Master)
		tmpreg |= (1UL << 9) | (1UL << 8);    // SSM = 1, SSI = 1
	} else {
		tmpreg &= ~(1UL << 2);                // MSTR = 0 (Slave)
		tmpreg |= (1UL << 9);                 // SSM = 1
		tmpreg &= ~(1UL << 8);                // SSI = 0
	}

    // Write back configuration to CR1 register
    SPI1->CR1 = tmpreg;

    // Enable SPI peripheral (SPE = 1 at bit 6)
    SPI1->CR1 |= (1UL << 6);
}

/**
 * @brief Send a single byte of data via SPI1 Data Register (DR)
 * @param data: 8-bit data to transmit
 */
void SPI1_Transmit(uint8_t data) {
    // Wait until TX buffer is empty (TXE = 1)
    while (!(SPI1->SR & (1UL << 1)));

    // Load data into DR
    SPI1->DR = data;
}

/**
 * @brief Receive a single byte of data from SPI1 Data Register (DR)
 * @return uint8_t: Received 8-bit data
 */
uint8_t SPI1_Receive(void) {
    // Wait until RX buffer is not empty (RXNE = 1)
    while (!(SPI1->SR & (1UL << 0)));

    // Read received data from DR
    return (uint8_t)SPI1->DR;
}
