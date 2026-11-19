#include "cortex_m4_mpu.h"

/* Inline ARMv7-M memory barriers for deterministic MPU state transitions */
static inline void mpu_dsb(void)
{
    __asm volatile ("dsb 0xF" ::: "memory");
}

static inline void mpu_isb(void)
{
    __asm volatile ("isb 0xF" ::: "memory");
}

uint8_t MPU_GetRegionCount(void)
{
    return (uint8_t)((MPU->TYPE & MPU_TYPE_DREGION_Msk) >> MPU_TYPE_DREGION_Pos);
}

void MPU_Disable(void)
{
    /* Ensure all outstanding memory transfers complete before disabling MPU */
    mpu_dsb();
    MPU->CTRL = 0UL;
    mpu_isb();
}

void MPU_Enable(bool use_default_mem_map)
{
    uint32_t ctrl_val = MPU_CTRL_ENABLE_Msk;

    if (use_default_mem_map)
    {
        /* Allow privileged software to access background memory map outside defined regions */
        ctrl_val |= MPU_CTRL_PRIVDEFENA_Msk;
    }

    MPU->CTRL = ctrl_val;

    /* Synchronize pipeline so new MPU rules take effect immediately */
    mpu_dsb();
    mpu_isb();
}

bool MPU_ConfigureRegion(const mpu_region_config_t *config)
{
    uint8_t max_regions = MPU_GetRegionCount();
    uint32_t size_bytes = 0UL;
    uint32_t rasr_val   = 0UL;

    if ((config == (void *)0) || (config->region_num >= max_regions))
    {
        return false;
    }

    /* Validate base address alignment against region size (2^(size + 1)) */
    size_bytes = 1UL << ((uint32_t)config->size + 1U);
    if ((config->base_addr & (size_bytes - 1UL)) != 0UL)
    {
        return false;
    }

    /* Select target MPU region */
    MPU->RNR = (uint32_t)config->region_num;

    /* Set region base address (lower 5 bits masked out; RNR already selected) */
    MPU->RBAR = (config->base_addr & MPU_RBAR_ADDR_Msk);

    /* Build Region Attribute and Size Register (RASR) */
    rasr_val |= ((uint32_t)config->size << MPU_RASR_SIZE_Pos);
    rasr_val |= ((uint32_t)config->access_perm << MPU_RASR_AP_Pos);

    if (config->exec_never)
    {
        rasr_val |= MPU_RASR_XN_Msk;
    }
    if (config->shareable)
    {
        rasr_val |= MPU_RASR_S_Msk;
    }
    if (config->cacheable)
    {
        rasr_val |= MPU_RASR_C_Msk;
    }
    if (config->bufferable)
    {
        rasr_val |= MPU_RASR_B_Msk;
    }

    /* Enable this region */
    rasr_val |= MPU_RASR_ENABLE_Msk;
    MPU->RASR = rasr_val;

    return true;
}
