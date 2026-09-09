#include "stm32f401_nvic.h"

void NVIC_EnableIRQ_Custom(uint8_t irq_num) {
    // Write 1 to enable bit (irq_num / 32 and irq_num % 32)
    NVIC->ISER[irq_num >> 5] = (1U << (irq_num & 0x1FU));
}

void NVIC_DisableIRQ_Custom(uint8_t irq_num) {
    // Write 1 to clear enable bit
    NVIC->ICER[irq_num >> 5] = (1U << (irq_num & 0x1FU));
}

uint8_t NVIC_GetEnableIRQ_Custom(uint8_t irq_num) {
    // Read bit status: 1 = enabled, 0 = disabled
    if ((NVIC->ISER[irq_num >> 5] & (1U << (irq_num & 0x1FU))) != 0U) {
        return 1U;
    }
    return 0U;
}
