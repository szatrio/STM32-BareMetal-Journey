#include "stm32f401_systick.h"

// Global millisecond counter incremented inside the ISR
volatile uint32_t g_ticks = 0;

void SysTick_Init(uint32_t ticks) {
    SysTick->CTRL = 0;
	SysTick->LOAD = ticks - 1;
    SysTick->VAL = 0;
    SysTick->CTRL = SYSTICK_CTRL_CLKSOURCE | SYSTICK_CTRL_TICKINT | SYSTICK_CTRL_ENABLE;
}

/**
 * @brief  SysTick Exception Handler (Hardware Interrupt Service Routine).
 *
 * @note   This function is invoked automatically by the ARM Cortex-M core
 *         every time the SysTick counter reaches 0 (every 1ms).
 *
 * @note   1. Function name MUST match the symbol in startup_stm32f401xc.s vector table.
 *         2. Increments the global millisecond time-base counter (g_ticks).
 *         3. Hardware automatically clears the COUNTFLAG upon entry (no manual clear needed).
 *
 * @note   can check
 */
void SysTick_Handler(void) {
    g_ticks++; // Advance system heartbeat by 1 ms
}

uint32_t SysTick_GetTick(void) {
    return g_ticks;
}
