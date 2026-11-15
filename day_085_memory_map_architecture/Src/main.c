/**
 * @file main.c
 * @brief Day 85 Memory Map & Bus Architecture Probing Test
 */

#include <stdint.h>
#include "stm32f401_registers.h"
#include "fault_handler.h"

/* Memory region type definitions */
typedef enum {
    MEM_TYPE_FLASH = 0,
    MEM_TYPE_SRAM,
    MEM_TYPE_PERIPHERAL,
    MEM_TYPE_INVALID
} MemoryType_t;

/* Check memory bus region based on address boundaries */
static MemoryType_t mem_get_region_type(volatile uint32_t *address)
{
    uint32_t addr = (uint32_t)address;

    /* Flash Memory Region: 0x08000000 - 0x0807FFFF (512KB) */
    if ((addr >= 0x08000000U) && (addr < (0x08000000U + (512U * 1024U)))) {
        return MEM_TYPE_FLASH;
    }
    /* SRAM Region: 0x20000000 - 0x20017FFF (96KB) */
    else if ((addr >= 0x20000000U) && (addr < (0x20000000U + (96U * 1024U)))) {
        return MEM_TYPE_SRAM;
    }
    /* Peripheral Region: 0x40000000 - 0x5FFFFFFF */
    else if ((addr >= 0x40000000U) && (addr < 0x60000000U)) {
        return MEM_TYPE_PERIPHERAL;
    }

    return MEM_TYPE_INVALID;
}

int main(void)
{
    /* Test pointers across different bus regions */
    volatile uint32_t *flash_ptr  = (volatile uint32_t *)0x08000000U;
    volatile uint32_t *sram_ptr   = (volatile uint32_t *)0x20000000U;
    volatile uint32_t *periph_ptr = (volatile uint32_t *)&GPIOA->MODER;

    MemoryType_t type_flash;
    MemoryType_t type_sram;
    MemoryType_t type_periph;

    while (1)
    {
        /* Probing memory regions at runtime */
        type_flash  = mem_get_region_type(flash_ptr);
        type_sram   = mem_get_region_type(sram_ptr);
        type_periph = mem_get_region_type(periph_ptr);

        /* Prevent compiler optimization */
        (void)type_flash;
        (void)type_sram;
        (void)type_periph;
    }
}
