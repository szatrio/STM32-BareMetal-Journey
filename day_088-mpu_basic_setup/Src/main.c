#include <stdint.h>
#include <stdbool.h>
#include "cortex_m4_mpu.h"

/* 32-byte aligned buffer in SRAM to satisfy MPU minimum region alignment */
__attribute__((aligned(32)))
static volatile uint32_t protected_buffer[8] = {
    0x11111111UL, 0x22222222UL, 0x33333333UL, 0x44444444UL,
    0x55555555UL, 0x66666666UL, 0x77777777UL, 0x88888888UL
};

int main(void)
{
    uint8_t  mpu_regions   = 0U;
    bool     config_ok     = false;
    uint32_t read_val      = 0UL;
    uint32_t mpu_ctrl_stat = 0UL;

    /* 1. Query hardware MPU capability (Expect 8 regions on STM32F401RE) */
    mpu_regions = MPU_GetRegionCount();

    /* 2. Disable MPU prior to region configuration */
    MPU_Disable();

    /* 3. Configure Region 0 as Read-Only & Execute-Never (XN) */
    mpu_region_config_t ro_region = {
        .region_num  = 0U,
        .base_addr   = (uint32_t)&protected_buffer[0],
        .size        = MPU_SIZE_32B,
        .access_perm = MPU_AP_PRIV_RO,
        .exec_never  = true,
        .shareable   = true,
        .cacheable   = true,
        .bufferable  = false
    };

    config_ok = MPU_ConfigureRegion(&ro_region);

    /* 4. Enable MPU with background default memory map (PRIVDEFENA = 1) */
    if (config_ok)
    {
        MPU_Enable(true);
    }

    mpu_ctrl_stat = MPU->CTRL;

    /* 5. Legal Read Access: Succeeds cleanly under Read-Only policy */
    read_val = protected_buffer[0];

    /* Prevent compiler optimization before fault trigger */
    (void)mpu_regions;
    (void)mpu_ctrl_stat;
    (void)read_val;

    /* 6. Illegal Write Access: Hardware MPU intercepts & triggers Fault */
    protected_buffer[0] = 0xDEADBEEFUL;

    while (1)
    {
    }
}
