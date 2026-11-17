#include <stdint.h>
#include "stm32f401_registers.h"
#include "cortex_m4_dwt.h"

#define LOOP_COUNT (10000UL)

/* Standard function placed in .text (Flash: 0x0800xxxx) */
__attribute__((noinline))
static uint32_t run_in_flash(uint32_t count)
{
    volatile uint32_t acc = 0UL;
    for (uint32_t i = 0UL; i < count; i++)
    {
        acc += (i ^ 0x5A5A5A5AUL);
    }
    return acc;
}

/* Function copied to & executed from SRAM (.RamFunc: 0x2000xxxx) */
__attribute__((section(".RamFunc"), noinline))
static uint32_t run_in_sram(uint32_t count)
{
    volatile uint32_t acc = 0UL;
    for (uint32_t i = 0UL; i < count; i++)
    {
        acc += (i ^ 0x5A5A5A5AUL);
    }
    return acc;
}

int main(void)
{
    /* 1. Verify physical execution addresses */
    uintptr_t flash_func_addr = (uintptr_t)&run_in_flash;
    uintptr_t sram_func_addr  = (uintptr_t)&run_in_sram;

    uint32_t start_cyc = 0UL;
    uint32_t cyc_flash_uncached = 0UL;
    uint32_t cyc_flash_cached   = 0UL;
    uint32_t cyc_sram           = 0UL;

    /* Enable DWT Cycle Counter using existing bare-metal driver */
    (void)DWT_Delay_Init(16000000UL);

    /* 2. Flash execution WITHOUT ART Cache (2 Wait States penalty) */
    FLASH->ACR = 0x00000002UL;
    start_cyc = DWT_GetCycleCount();
    (void)run_in_flash(LOOP_COUNT);
    cyc_flash_uncached = DWT_GetCycleCount() - start_cyc;

    /* 3. Flash execution WITH ART Cache (Prefetch + I-Cache + D-Cache) */
    FLASH->ACR = 0x00000702UL;
    start_cyc = DWT_GetCycleCount();
    (void)run_in_flash(LOOP_COUNT);
    cyc_flash_cached = DWT_GetCycleCount() - start_cyc;

    /* 4. SRAM execution (Zero Wait-State, immune to Flash bus stalls) */
    start_cyc = DWT_GetCycleCount();
    (void)run_in_sram(LOOP_COUNT);
    cyc_sram = DWT_GetCycleCount() - start_cyc;

    /* Prevent compiler optimization */
    (void)flash_func_addr;
    (void)sram_func_addr;
    (void)cyc_flash_uncached;
    (void)cyc_flash_cached;
    (void)cyc_sram;

    while (1)
    {
    }
}
