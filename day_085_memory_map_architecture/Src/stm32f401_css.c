#include "stm32f401_css.h"

// Flag to track clock security system failure state
static volatile bool g_css_failure_detected = false;

bool CSS_Init(void) {
    // --- SIMULATION CONTROL FOR CONTENT DEMO ---
    // Change to '1' to simulate HSE startup failure (triggers fallback path)
    // Change to '0' for normal hardware execution
    #if 1
        // [SIMULATION MODE] Force failure immediately to test the fallback LED behavior
        return false;
    #else
        // [REAL HARDWARE MODE] Original implementation
        // 1. Enable High Speed External Oscillator (HSE) in RCC_CR
        RCC_CR |= RCC_CR_HSEON;

        // 2. Poll until HSE is ready (HSERDY set) with a safety timeout
        uint32_t timeout = 200000UL;
        while (!(RCC_CR & RCC_CR_HSERDY)) {
            timeout--;
            if (timeout == 0U) {
                return false; // HSE startup failed
            }
        }

        // 3. Enable Clock Security System (CSS)
        // Note: CSSON can only be set when HSE is ready (HSERDY = 1)
        RCC_CR |= RCC_CR_CSSON;

        return true;
    #endif
}

void NMI_Handler(void) {
    // Check if the interrupt is triggered by the Clock Security System
    if (RCC_CIR & RCC_CIR_CSSF) {
        g_css_failure_detected = true;

        // Clear the CSS Interrupt Flag (CSSC) to prevent infinite interrupt loops
        RCC_CIR |= RCC_CIR_CSSC;

        // Hardware automatically switches the system clock to HSI (Internal Clock).
        // Implement fallback logic or error indication here (e.g., turn on error LED).
    }
}
