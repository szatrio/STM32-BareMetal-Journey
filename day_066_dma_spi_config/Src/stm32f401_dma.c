#include "stm32f401_dma.h"

void DMA_Stream_Init(DMA_Stream_TypeDef *dma_stream,
                     uint32_t peripheral_base,
                     uint32_t memory_base,
                     uint16_t buffer_size,
                     uint32_t channel_sel)
{
    // 1. Automatically enable DMA1 or DMA2 clock based on the base pointer
    if ((uint32_t)dma_stream >= DMA2_BASE) {
        RCC_AHB1ENR |= RCC_AHB1ENR_DMA2EN;
    } else {
    	RCC_AHB1ENR |= RCC_AHB1ENR_DMA1EN;
    }

    // 2. Ensure the Stream is disabled before configuration
    dma_stream->CR &= ~DMA_SxCR_EN;
    while ((dma_stream->CR & DMA_SxCR_EN) != 0);

    // 3. Set Peripheral Address (PAR) & Memory Address (M0AR)
    dma_stream->PAR = peripheral_base;
    dma_stream->M0AR = memory_base;

    // 4. Set Number of Data items to transfer (NDTR)
    dma_stream->NDTR = buffer_size;

    // 5. Configure the Control Register (CR) flexibly
    dma_stream->CR = channel_sel |                     // Selected channel (e.g., Channel 0 for ADC1)
                     DMA_SxCR_DIR_P2M |                // Peripheral to Memory
                     DMA_SxCR_MINC |                   // Memory increment mode
                     DMA_SxCR_PSIZE_16 |               // Peripheral data size 16-bit
                     DMA_SxCR_MSIZE_16 |               // Memory data size 16-bit
                     DMA_SxCR_CIRC |                   // Circular mode
                     DMA_SxCR_PL_HIGH;                 // High priority level

    // 6. Enable the Stream back
    dma_stream->CR |= DMA_SxCR_EN;
}

/**
 * @brief Initialize DMA Stream specifically for SPI 8-bit normal mode transfer
 * @param dma_stream: Pointer to DMA stream (e.g., DMA2->Stream[3] for TX)
 * @param peripheral_base: Peripheral data register address (e.g., (uint32_t)&(SPI1->DR))
 * @param memory_base: RAM buffer address pointer
 * @param buffer_size: Number of bytes to transfer
 * @param channel_sel: DMA channel selection bitmask
 * @param direction: Transfer direction (DMA_SxCR_DIR_M2P for TX, DMA_SxCR_DIR_P2M for RX)
 */
void DMA_SPI_Stream_Init(DMA_Stream_TypeDef *dma_stream,
                         uint32_t peripheral_base,
                         uint32_t memory_base,
                         uint16_t buffer_size,
                         uint32_t channel_sel,
                         uint32_t direction)
{
    // 1. Enable DMA2 clock (SPI1 uses DMA2 on STM32F401)
    RCC_AHB1ENR |= RCC_AHB1ENR_DMA2EN;

    // 2. Ensure the Stream is disabled before configuration
    dma_stream->CR &= ~DMA_SxCR_EN;
    while ((dma_stream->CR & DMA_SxCR_EN) != 0);

    // 3. Set Peripheral and Memory base addresses
    dma_stream->PAR  = peripheral_base;
    dma_stream->M0AR = memory_base;
    dma_stream->NDTR = (uint32_t)buffer_size;

    // 4. Configure Control Register (CR) for 8-bit SPI normal mode transfer
    dma_stream->CR = channel_sel |                     // Selected DMA channel
                     direction |                       // Transfer direction (Memory-to-Peripheral or P2M)
                     DMA_SxCR_MINC |                   // Enable memory increment mode
                     (0 << 9)      |                   // Disable peripheral increment (PINC = 0, address fixed to DR)
                     DMA_SxCR_PSIZE_8  |               // Peripheral data size: 8-bit
                     DMA_SxCR_MSIZE_8  |               // Memory data size: 8-bit
                     DMA_SxCR_PL_HIGH;                 // High priority level
                     // Note: Circular mode bit (CIRC) is NOT set (Normal/One-shot mode)
}

// checking whether transfe complete flag is active at stream 0
uint8_t DMA_GetTCFlag(DMA_TypeDef *DMAx, uint8_t stream) {
    if (stream == 0) {
        return (DMAx->LISR & DMA_LISR_TCIF0) ? 1 : 0;
    }
    return 0;
}

// Clear Transfer Complete Flag at Stream 0 (must be clear via LIFCR)
void DMA_ClearTCFlag(DMA_TypeDef *DMAx, uint8_t stream) {
    if (stream == 0) {
        DMAx->LIFCR |= DMA_LIFCR_CTCIF0;
    }
}
