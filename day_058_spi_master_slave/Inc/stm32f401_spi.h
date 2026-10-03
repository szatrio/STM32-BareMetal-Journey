#ifndef STM32F401_SPI_H
#define STM32F401_SPI_H

#include "stm32f401_registers.h"

// --- SPI Baud Rate Prescaler Options ---
#define SPI_BAUDRATE_DIV2   (0x00UL << 3)
#define SPI_BAUDRATE_DIV4   (0x01UL << 3)
#define SPI_BAUDRATE_DIV8   (0x02UL << 3)
#define SPI_BAUDRATE_DIV16  (0x03UL << 3)
#define SPI_BAUDRATE_DIV32  (0x04UL << 3)
#define SPI_BAUDRATE_DIV64  (0x05UL << 3)
#define SPI_BAUDRATE_DIV128 (0x06UL << 3)
#define SPI_BAUDRATE_DIV256 (0x07UL << 3)

// --- SPI Clock Polarity (CPOL) ---
#define SPI_CPOL_LOW        0
#define SPI_CPOL_HIGH       1

// --- SPI Clock Phase (CPHA) ---
#define SPI_CPHA_1EDGE      0
#define SPI_CPHA_2EDGE      1

/**
 * @brief Initialize SPI1 peripheral clock
 */
void SPI1_Init(void);

/**
 * @brief Configure SPI1 Control Register 1 (CR1)
 */
void SPI1_Config(uint32_t baud_rate_prescaler, uint8_t cpol, uint8_t cpha, uint8_t master_mode);

/**
 * @brief Send a single byte of data via SPI1 Data Register (DR)
 */
void SPI1_Transmit(uint8_t data);

/**
 * @brief Receive a single byte of data from SPI1 Data Register (DR)
 */
uint8_t SPI1_Receive(void);

#endif // STM32F401_SPI_H
