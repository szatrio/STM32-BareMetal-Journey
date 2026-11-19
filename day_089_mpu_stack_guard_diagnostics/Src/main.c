#include <stdint.h>
#include <stdbool.h>
#include "cortex_m4_core.h"
#include "cortex_m4_mpu.h"

/* Linker script boundary symbols */
extern uint32_t _estack;
extern uint32_t _Min_Stack_Size;

/* Diagnostic capture variables for Debugger Expressions */
static volatile uint8_t  fault_mmfsr      = 0U;
static volatile uint32_t fault_mmfar      = 0UL;
static volatile bool     fault_mmar_valid = false;
static volatile uint32_t recursion_depth  = 0UL;

/* Dedicated MemManage Fault Handler */
void MemManage_Handler(void)
{
    /* Extract lower 8 bits of CFSR (MemManage Fault Status Register) */
    fault_mmfsr = (uint8_t)(SCB->CFSR & 0xFFUL);

    /* Capture exact offending memory address if MMARVALID bit is set */
    if ((fault_mmfsr & SCB_CFSR_MMARVALID_Msk) != 0U)
    {
        fault_mmar_valid = true;
        fault_mmfar      = SCB->MMFAR;
    }

    while (1)
    {
        /* Halt here for debugger inspection */
    }
}

/* Controlled recursive function to exhaust stack and hit MPU Guard */
__attribute__((noinline))
static uint32_t trigger_stack_overflow(uint32_t depth)
{
    volatile uint32_t frame_buffer[16];
    frame_buffer[0] = depth;
    recursion_depth = depth;

    if (depth >= 1000UL)
    {
        return frame_buffer[0];
    }

    return frame_buffer[0] + trigger_stack_overflow(depth + 1UL);
}

int main(void)
{
    /* Calculate stack bottom address & align up to 32-byte MPU boundary */
    uint32_t stack_top    = (uint32_t)&_estack;
    uint32_t stack_size   = (uint32_t)&_Min_Stack_Size;
    uint32_t stack_bottom = (stack_top - stack_size + 31UL) & ~31UL;

    bool guard_ok = false;

    /* 1. Enable dedicated MemManage Fault exception in SCB->SHCSR */
    SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk;

    /* 2. Configure 32-byte No-Access Stack Guard at the bottom of the stack */
    MPU_Disable();

    mpu_region_config_t stack_guard = {
        .region_num  = 0U,
        .base_addr   = stack_bottom,
        .size        = MPU_SIZE_32B,
        .access_perm = MPU_AP_NO_ACCESS,
        .exec_never  = true,
        .shareable   = true,
        .cacheable   = true,
        .bufferable  = false
    };

    guard_ok = MPU_ConfigureRegion(&stack_guard);

    if (guard_ok)
    {
        MPU_Enable(true);
    }

    /* 3. Intentionally overflow stack to verify hardware MPU interception */
    (void)trigger_stack_overflow(1UL);

    while (1)
    {
    }
}
