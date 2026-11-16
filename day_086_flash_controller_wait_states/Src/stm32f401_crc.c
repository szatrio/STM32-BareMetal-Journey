#include "stm32f401_crc.h"

/**
 * @brief Initialize CRC peripheral clock
 */
void CRC_Init(void) {
    // Enable CRC clock on AHB1 bus (Bit 12)
    RCC_AHB1ENR |= RCC_AHB1ENR_CRCEN;
}

/**
 * @brief Reset CRC calculation unit (resets DR to default 0xFFFFFFFF)
 */
void CRC_Reset(void) {
    CRC->CR = CRC_CR_RESET;
}

/**
 * @brief Calculate CRC for a single 32-bit word
 * @param data: Input data word
 * @return Computed CRC checksum
 */
uint32_t CRC_Calculate(uint32_t data) {
    CRC->DR = data;
    return CRC->DR;
}

/**
 * @brief Calculate CRC for an array/buffer of 32-bit words
 * @param pBuffer: Pointer to data buffer
 * @param length: Number of 32-bit words
 * @return Computed CRC checksum
 */
uint32_t CRC_CalculateBuffer(uint32_t *pBuffer, uint32_t length) {
    // Always reset before calculating a new block/stream
    CRC_Reset();

    for (uint32_t i = 0; i < length; i++) {
        CRC->DR = pBuffer[i];
    }

    return CRC->DR;
}
