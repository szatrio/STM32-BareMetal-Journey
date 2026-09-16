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
