#ifndef STM32F401_DMA_H
#define STM32F401_DMA_H

#include "stm32f401_registers.h"

void DMA_Stream_Init(DMA_Stream_TypeDef *dma_stream,
                     uint32_t peripheral_base,
                     uint32_t memory_base,
                     uint16_t buffer_size,
                     uint32_t channel_sel);

uint8_t DMA_GetTCFlag(DMA_TypeDef *DMAx, uint8_t stream);
void DMA_ClearTCFlag(DMA_TypeDef *DMAx, uint8_t stream);

#endif
