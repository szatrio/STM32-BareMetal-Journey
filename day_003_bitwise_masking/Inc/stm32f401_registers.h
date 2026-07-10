#ifndef STM32F401_REGISTERS_H
#define STM32F401_REGISTERS_H

#include <stdint.h>

// --- Base Address ---
#define PERIPH_BASE         0x40000000UL
#define AHB1PERIPH_BASE     (PERIPH_BASE + 0x00020000UL)
#define RCC_BASE            (AHB1PERIPH_BASE + 0x3800UL)
#define GPIOA_BASE          (AHB1PERIPH_BASE + 0x0000UL)

// --- Register Offset ---
#define RCC_AHB1ENR_OFFSET  0x30UL
#define GPIOA_MODER_OFFSET  0x00UL
#define GPIOA_ODR_OFFSET    0x14UL

// --- Pointers ---
#define RCC_AHB1ENR         (*(volatile uint32_t *)(RCC_BASE + RCC_AHB1ENR_OFFSET))
#define GPIOA_MODER         (*(volatile uint32_t *)(GPIOA_BASE + GPIOA_MODER_OFFSET))
#define GPIOA_ODR           (*(volatile uint32_t *)(GPIOA_BASE + GPIOA_ODR_OFFSET))

// Bit 0 = GPIOA, Bit 1 = GPIOB
#define RCC_GPIOA_EN        (1 << 0)

// Helper Macros
#define RCC_ENABLE_GPIOA()    (RCC_AHB1ENR |= RCC_GPIOA_EN)
#define RCC_DISABLE_GPIOA()   (RCC_AHB1ENR &= ~RCC_GPIOA_EN)

#endif
