#ifndef STM32F401_REGISTERS_H
#define STM32F401_REGISTERS_H

#include <stdint.h>
#include <stdbool.h>

#define PERIPH_BASE         0x40000000UL

// --- AHB1 Bus Peripherals ---
#define AHB1PERIPH_BASE     (PERIPH_BASE + 0x00020000UL)
#define RCC_BASE            (AHB1PERIPH_BASE + 0x3800UL)
#define GPIOA_BASE          (AHB1PERIPH_BASE + 0x0000UL)
#define GPIOC_BASE          (AHB1PERIPH_BASE + 0x0800UL)

// --- APB1 Bus Peripherals ---
#define APB1PERIPH_BASE     (PERIPH_BASE + 0x00000000UL)
#define USART2_BASE         (APB1PERIPH_BASE + 0x4400UL)

// --- RCC Register Offsets & Macro ---
#define RCC_AHB1ENR_OFFSET  0x30UL
#define RCC_AHB1ENR         (*(volatile uint32_t *)(RCC_BASE + RCC_AHB1ENR_OFFSET))

#define RCC_APB1ENR_OFFSET  0x40UL
#define RCC_APB1ENR         (*(volatile uint32_t *)(RCC_BASE + RCC_APB1ENR_OFFSET))

// --- GPIO Register Struct Map ---
typedef struct {
    volatile uint32_t MODER;    // 0x00
    volatile uint32_t OTYPER;   // 0x04
    volatile uint32_t OSPEEDR;  // 0x08
    volatile uint32_t PUPDR;    // 0x0C
    volatile uint32_t IDR;      // 0x10
    volatile uint32_t ODR;      // 0x14
    volatile uint32_t BSRR;     // 0x18
    volatile uint32_t LCKR;		// 0x1C
    volatile uint32_t AFR[2];	// 0x20 & 0x24
} GPIO_TypeDef;

// --- USART Register Struct Map ---
typedef struct {
    volatile uint32_t SR;   // 0x00 - Status Register
    volatile uint32_t DR;   // 0x04 - Data Register
    volatile uint32_t BRR;  // 0x08 - Baud Rate Register
    volatile uint32_t CR1;  // 0x0C - Control Register 1
    volatile uint32_t CR2;  // 0x10 - Control Register 2
    volatile uint32_t CR3;  // 0x14 - Control Register 3
    volatile uint32_t GTPR; // 0x18 - Guard Time and Prescaler Register
} USART_TypeDef;

// --- Peripheral Base Pointers ---
#define GPIOA               ((GPIO_TypeDef *) GPIOA_BASE)
#define GPIOC               ((GPIO_TypeDef *) GPIOC_BASE)
#define USART2              ((USART_TypeDef *) USART2_BASE)

// --- Clock Enable Masks ---
#define RCC_GPIOA_EN        (1UL << 0)
#define RCC_GPIOC_EN        (1UL << 2)

// --- USART Register Bit Definitions ---
#define USART_SR_TXE         (1U << 7)   // Bit 7: Transmit data register empty
#define USART_SR_RXNE  		 (1U << 5)	 // Bit 5: Read data register not empty
#define USART_CR1_RE         (1U << 2)   // Bit 2: Receiver enable
#define USART_CR1_TE         (1U << 3)   // Bit 3: Transmitter enable
#define USART_CR1_UE         (1U << 13)  // Bit 13: USART enable
#define RCC_APB1ENR_USART2EN (1U << 17)  // Bit 17: USART2 clock enable

// --- Enums & Configurations ---
typedef enum {
    GPIO_MODE_INPUT    = 0x00,
    GPIO_MODE_OUTPUT   = 0x01,
    GPIO_MODE_ALT      = 0x02,
    GPIO_MODE_ANALOG   = 0x03
} GPIO_Mode_t;

typedef enum {
    GPIO_OTYPE_PUSHPULL  = 0x00,
    GPIO_OTYPE_OPENDRAIN = 0x01
} GPIO_OType_t;

typedef enum {
    GPIO_PUPDR_NOPULLUPDOWN = 0x00,
    GPIO_PUPDR_PULLUP       = 0x01,
    GPIO_PUPDR_PULLDOWN     = 0x02,
    GPIO_PUPDR_RESERVED     = 0x03
} GPIO_PUPDR_t;

// --- GPIO Initialization Struct ---
typedef struct {
    uint32_t Pin;
    uint32_t Mode;
    uint32_t OType;
    uint32_t Pull;
} GPIO_Init_t;

// --- Button FSM States ---
typedef enum {
    BUTTON_RELEASED = 0,
    BUTTON_DEBOUNCING_DOWN,
    BUTTON_PRESSED,
    BUTTON_DEBOUNCING_UP
} ButtonState_t;

#define DEBOUNCE_THRESHOLD_TICKS  5

#endif
