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
#define IWDG_BASE           (APB1PERIPH_BASE + 0x3000UL)

// --- APB2 Bus Peripherals ---
#define APB2PERIPH_BASE     (PERIPH_BASE + 0x00010000UL)
#define SYSCFG_BASE         (APB2PERIPH_BASE + 0x3800UL)
#define EXTI_BASE           (APB2PERIPH_BASE + 0x3C00UL)

// --- RCC Register Offsets & Macro ---
#define RCC_AHB1ENR_OFFSET  0x30UL
#define RCC_AHB1ENR         (*(volatile uint32_t *)(RCC_BASE + RCC_AHB1ENR_OFFSET))

#define RCC_APB1ENR_OFFSET  0x40UL
#define RCC_APB1ENR         (*(volatile uint32_t *)(RCC_BASE + RCC_APB1ENR_OFFSET))

#define RCC_APB2ENR_OFFSET  0x44UL
#define RCC_APB2ENR         (*(volatile uint32_t *)(RCC_BASE + RCC_APB2ENR_OFFSET))

// --- GPIO Register Struct Map ---
typedef struct {
    volatile uint32_t MODER;    // 0x00
    volatile uint32_t OTYPER;   // 0x04
    volatile uint32_t OSPEEDR;  // 0x08
    volatile uint32_t PUPDR;    // 0x0C
    volatile uint32_t IDR;      // 0x10
    volatile uint32_t ODR;      // 0x14
    volatile uint32_t BSRR;     // 0x18
    volatile uint32_t LCKR;     // 0x1C
    volatile uint32_t AFR[2];   // 0x20 & 0x24
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

// --- IWDG Register Struct Map ---
typedef struct {
    volatile uint32_t KR;   // Key register (Write-only / Key access)   - Offset: 0x00
    volatile uint32_t PR;   // Prescaler register                       - Offset: 0x04
    volatile uint32_t RLR;  // Reload register                          - Offset: 0x08
    volatile uint32_t SR;   // Status register                          - Offset: 0x0C
} IWDG_TypeDef;

// --- EXTI Register Struct Map ---
typedef struct {
    volatile uint32_t IMR;   // Offset: 0x00 Interrupt Mask Register
    volatile uint32_t EMR;   // Offset: 0x04 Event Mask Register
    volatile uint32_t RTSR;  // Offset: 0x08 Rising Trigger Selection Register
    volatile uint32_t FTSR;  // Offset: 0x0C Falling Trigger Selection Register
    volatile uint32_t SWIER; // Offset: 0x10 Software Interrupt Event Register
    volatile uint32_t PR;    // Offset: 0x14 Pending Register
} EXTI_TypeDef;

// --- SYSCFG Register Struct Map ---
typedef struct {
    volatile uint32_t MEMRMP;    // Offset: 0x00
    volatile uint32_t PMC;       // Offset: 0x04
    volatile uint32_t EXTICR[4]; // Offset: 0x08 - 0x14 (EXTICR1 s/d EXTICR4)
    uint32_t RESERVED[2];
    volatile uint32_t CMPCR;     // Offset: 0x20
} SYSCFG_TypeDef;

// --- Peripheral Base Pointers ---
#define GPIOA               ((GPIO_TypeDef *) GPIOA_BASE)
#define GPIOC               ((GPIO_TypeDef *) GPIOC_BASE)
#define USART2              ((USART_TypeDef *) USART2_BASE)
#define IWDG                ((IWDG_TypeDef *) IWDG_BASE)
#define EXTI                ((EXTI_TypeDef *) EXTI_BASE)
#define SYSCFG              ((SYSCFG_TypeDef *) SYSCFG_BASE)

// --- Clock Enable Masks ---
#define RCC_GPIOA_EN          (1UL << 0)
#define RCC_GPIOC_EN          (1UL << 2)
#define RCC_APB2ENR_SYSCFGEN  (1UL << 14) // Bit 14: Clock enable SYSCFG

// --- USART Register Bit Definitions ---
#define USART_SR_TXE         (1UL << 7)   // Bit 7: Transmit data register empty
#define USART_SR_RXNE        (1UL << 5)   // Bit 5: Read data register not empty
#define USART_CR1_RE         (1UL << 2)   // Bit 2: Receiver enable
#define USART_CR1_TE         (1UL << 3)   // Bit 3: Transmitter enable
#define USART_CR1_UE         (1UL << 13)  // Bit 13: USART enable
#define RCC_APB1ENR_USART2EN (1UL << 17)  // Bit 17: USART2 clock enable

// --- IWDG Key Values (KR Register) ---
#define IWDG_KEY_RELOAD       (0x0000AAAAUL) // Refresh/Feed Watchdog
#define IWDG_KEY_ENABLE       (0x0000CCCCUL) // Start Watchdog counter
#define IWDG_KEY_ACCESS       (0x00005555UL) // Enable PR & RLR register write access

// --- IWDG Prescaler values (PR Register) ---
#define IWDG_PR_DIV_4         (0x00000000UL)
#define IWDG_PR_DIV_8         (0x00000001UL)
#define IWDG_PR_DIV_16        (0x00000002UL)
#define IWDG_PR_DIV_32        (0x00000003UL)
#define IWDG_PR_DIV_64        (0x00000004UL)
#define IWDG_PR_DIV_128       (0x00000005UL)
#define IWDG_PR_DIV_256       (0x00000006UL)

// --- IWDG Status Register Bit Definitions ---
#define IWDG_SR_PVU           (1UL << 0)  // Prescaler value update flag
#define IWDG_SR_RVU           (1UL << 1)  // Reload value update flag

// --- EXTI Register Bit Definitions ---
#define EXTI_PR_PR13     	  (1UL << 13)  // Bit 0: Pending bit for line 13
#define EXTI_IMR_MR13         (1UL << 13) // Interrupt Mask Line 13
#define EXTI_FTSR_TR13        (1UL << 13) // Falling Trigger Line 13

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

#define DEBOUNCE_THRESHOLD_TICKS  3

// --- NVIC Base Address & Struct Map (Cortex-M4 Core) ---
#define NVIC_BASE             (0xE000E100UL)

typedef struct {
    volatile uint32_t ISER[8U];       // Offset: 0x000 Interrupt Set Enable Register
    uint32_t RESERVED0[24U];
    volatile uint32_t ICER[8U];       // Offset: 0x080 Interrupt Clear Enable Register
    uint32_t RESERVED1[24U];
    volatile uint32_t ISPR[8U];       // Offset: 0x100 Interrupt Set Pending Register
    uint32_t RESERVED2[24U];
    volatile uint32_t ICPR[8U];       // Offset: 0x180 Interrupt Clear Pending Register
    uint32_t RESERVED3[24U];
    volatile uint32_t IABR[8U];       // Offset: 0x200 Interrupt Active Bit Register
    uint32_t RESERVED4[56U];
    volatile uint32_t IPR[60U];       // Offset: 0x300 Interrupt Priority Register
} NVIC_TypeDef;

#define NVIC                  ((NVIC_TypeDef *) NVIC_BASE)

#endif
