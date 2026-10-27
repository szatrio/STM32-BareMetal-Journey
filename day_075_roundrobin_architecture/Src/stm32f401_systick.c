#include "stm32f401_systick.h"
#include "rtos_task.h"

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
 * @note   Invoked automatically every 1ms. Advances tick counter and triggers RTOS scheduler.
 */
void SysTick_Handler(void) {
    g_ticks++; // Advance system heartbeat by 1 ms

    // Trigger the RTOS scheduler hook for round-robin multitasking
    RTOS_Scheduler();
}

uint32_t SysTick_GetTick(void) {
    return g_ticks;
}

void SysTick_DelayMs(uint32_t ms) {
    uint32_t start_time = SysTick_GetTick();

    // Unsigned subtraction safe from overflow
    while ((SysTick_GetTick() - start_time) < ms) {
        // Wait for duration
    }
}
