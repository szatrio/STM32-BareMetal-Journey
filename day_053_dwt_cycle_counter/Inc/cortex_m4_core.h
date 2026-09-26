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
#define SYSTICK_CTRL    (*(volatile uint32_t *)(SYSTICK_BASE + 0x00UL)) // Control and Status Register
#define SYSTICK_LOAD    (*(volatile uint32_t *)(SYSTICK_BASE + 0x04UL)) // Reload Value Register
#define SYSTICK_VAL     (*(volatile uint32_t *)(SYSTICK_BASE + 0x08UL)) // Current Value Register
#define SYSTICK_CALIB   (*(volatile uint32_t *)(SYSTICK_BASE + 0x0CUL)) // Calibration Value Register

#define SYSTICK_CTRL_ENABLE     (1UL << 0)  // Counter enable
#define SYSTICK_CTRL_TICKINT    (1UL << 1)  // Counting down to 0 pends the SysTick exception
#define SYSTICK_CTRL_CLKSOURCE  (1UL << 2)  // Clock source selection (0 = External, 1 = Processor clock)
#define SYSTICK_CTRL_COUNTFLAG  (1UL << 16) // Returns 1 if timer counted to 0 since last read


// --- SCB (System Control Block - e.g., FPU CPACR) ---
#define SCB_BASE        (0xE000ED00UL)
#define SCB_CPACR       (*(volatile uint32_t *)(SCB_BASE + 0x88UL)) // Coprocessor Access Control Register

#define SCB_CPACR_CP10_FULL     (3UL << 20) // Full access for CP10 (FPU)
#define SCB_CPACR_CP11_FULL     (3UL << 22) // Full access for CP11 (FPU)


// --- CoreDebug (For Enabling Trace/Debug) ---
#define CoreDebug_BASE  (0xE000EDF0UL)
#define CoreDebug_DEMCR (*(volatile uint32_t *)(CoreDebug_BASE + 0x0CUL)) // Debug Exception and Monitor Control Register

#define CoreDebug_DEMCR_TRCENA_Pos  (24U)
#define CoreDebug_DEMCR_TRCENA_Msk  (1UL << CoreDebug_DEMCR_TRCENA_Pos) // Trace enable bit


// --- DWT (Data Watchpoint and Trace - For Cycle Counter / Profiling) ---
#define DWT_BASE        (0xE0001000UL)
#define DWT_CTRL        (*(volatile uint32_t *)(DWT_BASE + 0x00UL)) // Control Register
#define DWT_CYCCNT      (*(volatile uint32_t *)(DWT_BASE + 0x04UL)) // Cycle Count Register

#define DWT_CTRL_CYCCNTENA_Pos      (0U)
#define DWT_CTRL_CYCCNTENA_Msk      (1UL << DWT_CTRL_CYCCNTENA_Pos) // Cycle counter enable bit


// --- Small Utilities ---
#ifndef __NOP
#define __NOP() __asm__ volatile ("nop")
#endif

#endif // CORTEX_M4_CORE_H
