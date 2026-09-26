#include "cortex_m4_dwt.h"

// CPU frequency (Hz)
static uint32_t dwt_cpu_freq_hz = 0UL;

bool DWT_Delay_Init(uint32_t cpu_freq_hz) {
    dwt_cpu_freq_hz = cpu_freq_hz;

    // TRCENA (Trace Enable) in CoreDebug register for accessing DWT modules
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

    // Reset cycle counter
    DWT_CYCCNT = 0UL;

    //CYCCNTENA (Cycle Counter Enable) in DWT_CTRL register
    DWT_CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    // Validating whether DWT is actually active by checking whether register cycle counter is run or not
    uint32_t startup_cycles = DWT_CYCCNT;

    // if persist 0, so failed unlocking DWT
    for (volatile uint32_t i = 0; i < 10UL; i++) {
        if (DWT_CYCCNT != startup_cycles) {
            return true; // Successfully Activate
        }
    }

    return false; // Failed Activate
}

uint32_t DWT_GetCycleCount(void) {
    // return 32-bit raw value from DWT_CYCCNT register
    return DWT_CYCCNT;
}

void DWT_Delay_us(uint32_t us) {
    // Checking if Initializing already started
    if (dwt_cpu_freq_hz == 0U) {
        return;
    }
    // count how many clock cycle needed to achieve specific microsecond (us)
    // cycle target = ((CPU frequency/s) / 1.000.000) * us
    uint32_t ticks_per_us = dwt_cpu_freq_hz / 1000000UL;
    uint32_t total_ticks = us * ticks_per_us;

    // getting cycle counter value when this function was started
    uint32_t start_tick = DWT_GetCycleCount();

    // Wait until target cycles are reached (safe from 32-bit unsigned overflow)
	while ((DWT_GetCycleCount() - start_tick) < total_ticks) {
		// waiting till cycle target is fulfilled
	}
}

void DWT_Delay_ms(uint32_t ms) {
    while (ms--) {
        DWT_Delay_us(1000U);
    }
}
