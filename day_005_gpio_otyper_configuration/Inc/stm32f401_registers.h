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
#define GPIOA_OTYPER_OFFSET 0x04UL

// --- Pointers ---
#define RCC_AHB1ENR         (*(volatile uint32_t *)(RCC_BASE + RCC_AHB1ENR_OFFSET))
#define GPIOA_MODER         (*(volatile uint32_t *)(GPIOA_BASE + GPIOA_MODER_OFFSET))
#define GPIOA_ODR           (*(volatile uint32_t *)(GPIOA_BASE + GPIOA_ODR_OFFSET))
#define GPIOA_OTYPER		(*(volatile uint32_t *)(GPIOA_BASE + GPIOA_OTYPER_OFFSET))

// --- Bit Definitions ---
#define RCC_GPIOA_EN        	(1UL << 0)
#define GPIO_MODE_OUTPUT    	(1UL)

typedef enum {
    GPIO_OTYPE_PUSHPULL  = 0x00,
    GPIO_OTYPE_OPENDRAIN = 0x01
} GPIO_OType_t;

// --- Helper Functions (Static Inline) ---
static inline void RCC_EnableGPIOClock(void) {
    RCC_AHB1ENR |= RCC_GPIOA_EN;
}

static inline void GPIOA_SetMode(uint8_t pin, uint32_t mode) {
    uint32_t mask = (3UL << (pin * 2UL));

//    Bits 2y:2y+1 MODERy[1:0]: Port x configuration bits (y = 0..15)
//    These bits are written by software to configure the I/O direction mode.
//    00: Input (reset state)
//    01: General purpose output mode
//    10: Alternate function mode
//    11: Analog mode

    GPIOA_MODER &= ~mask;
    GPIOA_MODER |= (mode << (pin * 2UL));
}

static inline void GPIOA_TogglePin(uint8_t pin) {
    if (pin > 15) return;
    GPIOA_ODR ^= (1UL << pin);
}

static inline void GPIOA_SetOutputOtype(uint8_t pin, uint32_t type){
	if (pin > 15) return;

//	Bits 15:0 OTy: Port x configuration bits (y = 0..15)
//	These bits are written by software to configure the output type of the I/O port.
//	0: Output push-pull (reset state)
//	1: Output open-drain

	GPIOA_OTYPER &= ~(1UL << pin);
	GPIOA_OTYPER |= ((type & 0x01UL) << pin);

}

#endif
