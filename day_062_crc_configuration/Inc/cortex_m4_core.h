#ifndef CORTEX_M4_CORE_H
#define CORTEX_M4_CORE_H

#include <stdint.h>
#include <stdbool.h>

// --- NVIC (Nested Vectored Interrupt Controller) ---
#define NVIC_BASE       (0xE000E100UL)

typedef struct {
    volatile uint32_t ISER[8U];   // Interrupt Set-Enable Registers
    uint32_t RESERVED0[24U];
    volatile uint32_t ICER[8U];   // Interrupt Clear-Enable Registers
    uint32_t RESERVED1[24U];
    volatile uint32_t ISPR[8U];   // Interrupt Set-Pending Registers
    uint32_t RESERVED2[24U];
    volatile uint32_t ICPR[8U];   // Interrupt Clear-Pending Registers
    uint32_t RESERVED3[24U];
    volatile uint32_t IABR[8U];   // Interrupt Active Bit Registers
    uint32_t RESERVED4[56U];
    volatile uint32_t IPR[60U];   // Interrupt Priority Registers
} NVIC_TypeDef;

#define NVIC            ((NVIC_TypeDef *) NVIC_BASE)


// --- SysTick (System Timer) ---
#define SYSTICK_BASE    (0xE000E010UL)

typedef struct {
    volatile uint32_t CTRL;   // Control and Status Register
    volatile uint32_t LOAD;   // Reload Value Register
    volatile uint32_t VAL;    // Current Value Register
    volatile uint32_t CALIB;  // Calibration Value Register
} SysTick_TypeDef;

#define SysTick         ((SysTick_TypeDef *) SYSTICK_BASE)

#define SYSTICK_CTRL_ENABLE     (1UL << 0)  // Counter enable
#define SYSTICK_CTRL_TICKINT    (1UL << 1)  // Counting down to 0 pends exception
#define SYSTICK_CTRL_CLKSOURCE  (1UL << 2)  // Clock source selection
#define SYSTICK_CTRL_COUNTFLAG  (1UL << 16) // Counted to 0 flag


// --- SCB (System Control Block - e.g., FPU CPACR) ---
#define SCB_BASE        (0xE000ED00UL)

typedef struct {
    volatile uint32_t CPUID;         // CPU ID Base Register
    volatile uint32_t ICSR;          // Interrupt Control and State Register
    volatile uint32_t VTOR;          // Vector Table Offset Register
    volatile uint32_t AIRCR;         // Application Interrupt and Reset Control Register
    volatile uint32_t SCR;           // System Control Register
    volatile uint32_t CCR;           // Configuration Control Register
    volatile uint32_t SHPR[3];       // System Handlers Priority Registers
    volatile uint32_t SHCSR;         // System Handler Control and State Register
    volatile uint32_t CFSR;          // Configurable Fault Status Register
    volatile uint32_t HFSR;          // HardFault Status Register
    volatile uint32_t DFSR;          // Debug Fault Status Register
    volatile uint32_t MMFAR;         // MemManage Fault Address Register
    volatile uint32_t BFAR;          // BusFault Address Register
    volatile uint32_t AFSR;          // Auxiliary Fault Status Register
    volatile uint32_t RESERVED[18];  // Padding to reach CPACR offset
    volatile uint32_t CPACR;         // Coprocessor Access Control Register
} SCB_Type;

#define SCB                     ((SCB_Type *) SCB_BASE)

#define SCB_SCR_SLEEPDEEP_Pos   (2U)
#define SCB_SCR_SLEEPDEEP_Msk   (1UL << SCB_SCR_SLEEPDEEP_Pos)
#define SCB_SCR_SLEEPDEEP       SCB_SCR_SLEEPDEEP_Msk

#define SCB_CPACR_CP10_FULL     (3UL << 20) // Full access for CP10 (FPU)
#define SCB_CPACR_CP11_FULL     (3UL << 22) // Full access for CP11 (FPU)


// --- CoreDebug (For Enabling Trace/Debug) ---
#define CoreDebug_BASE  (0xE000EDF0UL)

typedef struct {
    volatile uint32_t DHCSR;    // Debug Halting Control and Status Register
    volatile uint32_t DCRSR;    // Debug Core Register Selector Register
    volatile uint32_t DCRDR;    // Debug Core Register Data Register
    volatile uint32_t DEMCR;    // Debug Exception and Monitor Control Register
} CoreDebug_TypeDef;

#define CoreDebug       ((CoreDebug_TypeDef *) CoreDebug_BASE)

#define CoreDebug_DEMCR_TRCENA_Pos  (24U)
#define CoreDebug_DEMCR_TRCENA_Msk  (1UL << CoreDebug_DEMCR_TRCENA_Pos) // Trace enable bit


// --- DWT (Data Watchpoint and Trace - For Cycle Counter / Profiling) ---
#define DWT_BASE        (0xE0001000UL)

typedef struct {
    volatile uint32_t CTRL;     // Control Register
    volatile uint32_t CYCCNT;   // Cycle Count Register
    volatile uint32_t CPICNT;   // CPI Count Register
    volatile uint32_t EXCCNT;   // Exception Overhead Count Register
    volatile uint32_t SLEEPCNT; // Sleep Count Register
    volatile uint32_t LSUCNT;   // LSU Count Register
    volatile uint32_t FOLDCNT;  // Folded Instructions Count Register
} DWT_TypeDef;

#define DWT             ((DWT_TypeDef *) DWT_BASE)

#define DWT_CTRL_CYCCNTENA_Pos      (0U)
#define DWT_CTRL_CYCCNTENA_Msk      (1UL << DWT_CTRL_CYCCNTENA_Pos) // Cycle counter enable
// --- Small Utilities ---
#ifndef __NOP
#define __NOP() __asm__ volatile ("nop")
#endif

#endif // CORTEX_M4_CORE_H
