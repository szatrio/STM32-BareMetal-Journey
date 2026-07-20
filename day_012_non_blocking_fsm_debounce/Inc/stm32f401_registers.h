#ifndef STM32F401_REGISTERS_H
#define STM32F401_REGISTERS_H

#include <stdint.h>

// --- Base Address ---
#define PERIPH_BASE         0x40000000UL
#define AHB1PERIPH_BASE     (PERIPH_BASE + 0x00020000UL)
#define RCC_BASE            (AHB1PERIPH_BASE + 0x3800UL)
#define GPIOA_BASE          (AHB1PERIPH_BASE + 0x0000UL)
#define GPIOC_BASE          (AHB1PERIPH_BASE + 0x0800UL)

// --- Register Offset ---
#define RCC_AHB1ENR_OFFSET  0x30UL
#define GPIOA_MODER_OFFSET  0x00UL
#define GPIOA_ODR_OFFSET    0x14UL
#define GPIOA_OTYPER_OFFSET 0x04UL
#define GPIOA_PUPDR_OFFSET  0x0CUL
#define GPIOA_BSRR_OFFSET   0x18UL
#define GPIOC_MODER_OFFSET  0x00UL
#define GPIOC_IDR_OFFSET    0x10UL

// --- Pointers ---
#define RCC_AHB1ENR         (*(volatile uint32_t *)(RCC_BASE + RCC_AHB1ENR_OFFSET))
#define GPIOA_MODER         (*(volatile uint32_t *)(GPIOA_BASE + GPIOA_MODER_OFFSET))
#define GPIOA_ODR           (*(volatile uint32_t *)(GPIOA_BASE + GPIOA_ODR_OFFSET))
#define GPIOA_OTYPER		(*(volatile uint32_t *)(GPIOA_BASE + GPIOA_OTYPER_OFFSET))
#define GPIOA_PUPDR			(*(volatile uint32_t *)(GPIOA_BASE + GPIOA_PUPDR_OFFSET))
#define GPIOA_BSRR			(*(volatile uint32_t *)(GPIOA_BASE + GPIOA_BSRR_OFFSET))
#define GPIOC_MODER         (*(volatile uint32_t *)(GPIOC_BASE + GPIOC_MODER_OFFSET))
#define GPIOC_IDR           (*(volatile uint32_t *)(GPIOC_BASE + GPIOC_IDR_OFFSET))

// --- Bit Definitions ---
#define RCC_GPIOA_EN        	(1UL << 0)
#define RCC_GPIOC_EN        	(1UL << 2)
#define GPIO_MODE_INPUT    		(0UL)
#define GPIO_MODE_OUTPUT    	(1UL)

typedef enum {
    GPIO_OTYPE_PUSHPULL  = 0x00,
    GPIO_OTYPE_OPENDRAIN = 0x01
} GPIO_OType_t;

typedef enum {
    GPIO_PUPDR_NOPULLUPDOWN  = 0x00,
	GPIO_PUPDR_PULLUP    	 = 0x01,
	GPIO_PUPDR_PULLDOWN  	 = 0x02,
	GPIO_PUPDR_RESERVED  	 = 0x03
} GPIO_PUPDR_t;

typedef enum {
    BUTTON_RELEASED = 0,
    BUTTON_DEBOUNCING_DOWN,
    BUTTON_PRESSED,
    BUTTON_DEBOUNCING_UP
} ButtonState_t;

#define DEBOUNCE_THRESHOLD_TICKS  5

#endif
