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
#define TIM2_BASE           (APB1PERIPH_BASE + 0x0000UL)

// --- APB2 Bus Peripherals ---
#define APB2PERIPH_BASE     (PERIPH_BASE + 0x00010000UL)
#define SYSCFG_BASE         (APB2PERIPH_BASE + 0x3800UL)
#define EXTI_BASE           (APB2PERIPH_BASE + 0x3C00UL)
#define ADC1_BASE           (APB2PERIPH_BASE + 0x2000UL)

// --- RCC Register Definitions (Direct Addressing) ---
#define RCC_CR          (*(volatile uint32_t *)(RCC_BASE + 0x00UL))
#define RCC_PLLCFGR     (*(volatile uint32_t *)(RCC_BASE + 0x04UL))
#define RCC_CFGR        (*(volatile uint32_t *)(RCC_BASE + 0x08UL))
#define RCC_CIR         (*(volatile uint32_t *)(RCC_BASE + 0x0CUL))
#define RCC_AHB1RSTR    (*(volatile uint32_t *)(RCC_BASE + 0x10UL))
#define RCC_AHB2RSTR    (*(volatile uint32_t *)(RCC_BASE + 0x14UL))
#define RCC_APB1RSTR    (*(volatile uint32_t *)(RCC_BASE + 0x20UL))
#define RCC_APB2RSTR    (*(volatile uint32_t *)(RCC_BASE + 0x24UL))
#define RCC_AHB1ENR     (*(volatile uint32_t *)(RCC_BASE + 0x30UL))
#define RCC_AHB2ENR     (*(volatile uint32_t *)(RCC_BASE + 0x34UL))
#define RCC_APB1ENR     (*(volatile uint32_t *)(RCC_BASE + 0x40UL))
#define RCC_APB2ENR     (*(volatile uint32_t *)(RCC_BASE + 0x44UL))

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

// --- TIM2/TIM5 Register Struct Map ---
typedef struct {
    volatile uint32_t CR1;         // Offset: 0x00 - Control Register 1
    volatile uint32_t CR2;         // Offset: 0x04 - Control Register 2
    volatile uint32_t SMCR;        // Offset: 0x08 - Slave Mode Control Register
    volatile uint32_t DIER;        // Offset: 0x0C - DMA/Interrupt Enable Register
    volatile uint32_t SR;          // Offset: 0x10 - Status Register
    volatile uint32_t EGR;         // Offset: 0x14 - Event Generation Register
    volatile uint32_t CCMR1;       // Offset: 0x18 - Capture/Compare Mode Register 1
    volatile uint32_t CCMR2;       // Offset: 0x1C - Capture/Compare Mode Register 2
    volatile uint32_t CCER;        // Offset: 0x20 - Capture/Compare Enable Register
    volatile uint32_t CNT;         // Offset: 0x24 - Counter Register
    volatile uint32_t PSC;         // Offset: 0x28 - Prescaler Register
    volatile uint32_t ARR;         // Offset: 0x2C - Auto-Reload Register
    uint32_t RESERVED0;            // Offset: 0x30 - Reserved
    volatile uint32_t CCR1;        // Offset: 0x34 - Capture/Compare Register 1
    volatile uint32_t CCR2;        // Offset: 0x38 - Capture/Compare Register 2
    volatile uint32_t CCR3;        // Offset: 0x3C - Capture/Compare Register 3
    volatile uint32_t CCR4;        // Offset: 0x40 - Capture/Compare Register 4
    uint32_t RESERVED1;            // Offset: 0x44 - Reserved
    volatile uint32_t DCR;         // Offset: 0x48 - DMA Control Register
    volatile uint32_t DMAR;        // Offset: 0x4C - DMA Address for Full Transfer
    volatile uint32_t OR;          // Offset: 0x50 - Option Register (TIM2/TIM5)
} TIM_TypeDef;

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

// --- ADC Register Struct Map ---
typedef struct {
    volatile uint32_t SR;     // Offset: 0x00 - Status Register
    volatile uint32_t CR1;    // Offset: 0x04 - Control Register 1
    volatile uint32_t CR2;    // Offset: 0x08 - Control Register 2
    volatile uint32_t SMPR1;  // Offset: 0x0C - Sample Time Register 1
    volatile uint32_t SMPR2;  // Offset: 0x10 - Sample Time Register 2
    volatile uint32_t JOFR1;  // Offset: 0x14 - Injected Channel Data Offset Register 1
    volatile uint32_t JOFR2;  // Offset: 0x18 - Injected Channel Data Offset Register 2
    volatile uint32_t JOFR3;  // Offset: 0x1C - Injected Channel Data Offset Register 3
    volatile uint32_t JOFR4;  // Offset: 0x20 - Injected Channel Data Offset Register 4
    volatile uint32_t HTR;    // Offset: 0x24 - Watchdog Higher Threshold Register
    volatile uint32_t LTR;    // Offset: 0x28 - Watchdog Lower Threshold Register
    volatile uint32_t SQR1;   // Offset: 0x2C - Regular Sequence Register 1
    volatile uint32_t SQR2;   // Offset: 0x30 - Regular Sequence Register 2
    volatile uint32_t SQR3;   // Offset: 0x34 - Regular Sequence Register 3
    volatile uint32_t JSQR;   // Offset: 0x38 - Injected Sequence Register
    volatile uint32_t JDR1;   // Offset: 0x3C - Injected Data Register 1
    volatile uint32_t JDR2;   // Offset: 0x40 - Injected Data Register 2
    volatile uint32_t JDR3;   // Offset: 0x44 - Injected Data Register 3
    volatile uint32_t JDR4;   // Offset: 0x48 - Injected Data Register 4
    volatile uint32_t DR;     // Offset: 0x4C - Regular Data Register
} ADC_TypeDef;

// --- Peripheral Base Pointers ---
#define GPIOA               ((GPIO_TypeDef *) GPIOA_BASE)
#define GPIOC               ((GPIO_TypeDef *) GPIOC_BASE)
#define USART2              ((USART_TypeDef *) USART2_BASE)
#define IWDG                ((IWDG_TypeDef *) IWDG_BASE)
#define EXTI                ((EXTI_TypeDef *) EXTI_BASE)
#define SYSCFG              ((SYSCFG_TypeDef *) SYSCFG_BASE)
#define TIM2                ((TIM_TypeDef *) TIM2_BASE)
#define ADC1                ((ADC_TypeDef *) ADC1_BASE)

// --- Clock Enable Masks ---
#define RCC_GPIOA_EN          (1UL << 0)
#define RCC_GPIOC_EN          (1UL << 2)
#define RCC_APB2ENR_SYSCFGEN  (1UL << 14) // Bit 14: Clock enable SYSCFG
#define RCC_APB2ENR_ADC1EN    (1UL << 8)  // Bit 8: ADC1 clock enable

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

// --- RCC Clock Enable Bit Definition for TIM2 ---
#define RCC_APB1ENR_TIM2EN    (1UL << 0)   // Bit 0: TIM2 clock enable

// --- TIM2 Register Bit Definitions ---
#define TIM_CR1_CEN           (1UL << 0)   // Bit 0: Counter Enable
#define TIM_SR_UIF            (1UL << 0)   // Bit 0: Update Interrupt Flag
#define TIM_EGR_UG            (1UL << 0)   // Bit 0: Update Generation
#define TIM_DIER_UIE    	  (1UL << 0)   // Bit 0: Update interrupt Enable
#define TIM_CCMR1_OC1M_PWM1   (6UL << 4)   // Bit 6:4 = 110 (PWM Mode 1)
#define TIM_CCMR1_OC1PE       (1UL << 3)   // Bit 3: Output Compare 1 Preload Enable
#define TIM_CCER_CC1E         (1UL << 0)   // Bit 0: Capture/Compare 1 Output Enable
#define TIM_CCER_CC1P         (1UL << 1)   // Bit 1: Output Polarity (0 = Active High)

// --- EXTI Register Bit Definitions ---
#define EXTI_PR_PR13     	  (1UL << 13)  // Bit 0: Pending bit for line 13
#define EXTI_IMR_MR13         (1UL << 13) // Interrupt Mask Line 13
#define EXTI_FTSR_TR13        (1UL << 13) // Falling Trigger Line 13

// --- ADC Register Bit Definitions ---
#define ADC_SR_EOC            (1UL << 1)   // Bit 1: End of conversion flag
#define ADC_SR_OVR            (1UL << 5)   // Bit 5: Overrun flag
#define ADC_CR2_ADON          (1UL << 0)   // Bit 0: A/D Converter ON / OFF
#define ADC_CR2_EOCS		  (1UL << 10)  // Bit 10: Overrun detection is enabled
#define ADC_CR2_SWSTART       (1UL << 30)  // Bit 30: Start conversion of regular channels
#define ADC_SQR3_SQ1_POS      0U           // Bits 4:0 - 1st conversion in regular sequence
#define ADC_CR2_CONT    	  (1U << 1)    // Bit 1: Continuous Conversion

// --- ADC Register Bit Alignment ---
#define ADC_CR2_ALIGN_POS     (11U)
#define ADC_CR2_ALIGN         (1U << ADC_CR2_ALIGN_POS) // 0: Right alignment, 1: Left alignment

// TIM2 IRQ Number in STM32
#define TIM2_IRQn        28

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



// --- Cortex-M4 Core Peripherals: SysTick Base Address ---
#define SYSTICK_BASE          (0xE000E010UL)

// --- SysTick Register Struct Map ---
typedef struct {
    volatile uint32_t CTRL;   // Offset: 0x00 - Control and Status Register
    volatile uint32_t LOAD;   // Offset: 0x04 - Reload Value Register
    volatile uint32_t VAL;    // Offset: 0x08 - Current Value Register
    volatile uint32_t CALIB;  // Offset: 0x0C - Calibration Value Register
} SysTick_TypeDef;

// --- Peripheral Base Pointer ---
#define SysTick               ((SysTick_TypeDef *) SYSTICK_BASE)

// --- SysTick CTRL Register Bit Definitions ---
#define SYSTICK_CTRL_ENABLE     (1UL << 0)   // Bit 0: Counter Enable
#define SYSTICK_CTRL_TICKINT    (1UL << 1)   // Bit 1: SysTick Exception/Interrupt Enable
#define SYSTICK_CTRL_CLKSOURCE  (1UL << 2)   // Bit 2: Clock Source (1 = Processor Clock / HSI 16MHz)
#define SYSTICK_CTRL_COUNTFLAG  (1UL << 16)  // Bit 16: Returns 1 if counter reached 0

// --- System Control Block (SCB) Base Address & Struct Map (Cortex-M4 Core) ---
#define SCB_BASE              (0xE000ED00UL)

typedef struct {
    volatile uint32_t CPUID;      // Offset: 0x00 - CPUID Base Register
    volatile uint32_t ICSR;       // Offset: 0x04 - Interrupt Control and State Register
    volatile uint32_t VTOR;       // Offset: 0x08 - Vector Table Offset Register
    volatile uint32_t AIRCR;      // Offset: 0x0C - Application Interrupt and Reset Control
    volatile uint32_t SCR;        // Offset: 0x10 - System Control Register
    volatile uint32_t CCR;        // Offset: 0x14 - Configuration and Control Register
    volatile uint32_t SHPR[3];    // Offset: 0x18 - System Handler Priority Registers
    volatile uint32_t SHCSR;      // Offset: 0x24 - System Handler Control and State
    volatile uint32_t CFSR;       // Offset: 0x28 - Configurable Fault Status Register
    volatile uint32_t HFSR;       // Offset: 0x2C - HardFault Status Register
    volatile uint32_t DFSR;       // Offset: 0x30 - Debug Fault Status Register
    volatile uint32_t MMFAR;      // Offset: 0x34 - MemManage Fault Address Register
    volatile uint32_t BFAR;       // Offset: 0x38 - BusFault Address Register
    volatile uint32_t AFSR;       // Offset: 0x3C - Auxiliary Fault Status Register
    uint32_t RESERVED0[18];
    volatile uint32_t CPACR;      // Offset: 0x88 - Coprocessor Access Control Register (FPU)
} SCB_TypeDef;

#define SCB                   ((SCB_TypeDef *) SCB_BASE)

// --- SCB CPACR Bit Definitions (FPU CP10 & CP11 Enable) ---
#define SCB_CPACR_CP10_FULL   (3UL << 20)  // Bits 21:20 = Full access for CP10
#define SCB_CPACR_CP11_FULL   (3UL << 22)  // Bits 23:22 = Full access for CP11

#endif
