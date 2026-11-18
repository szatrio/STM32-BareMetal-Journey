#ifndef CORTEX_M4_MPU_H
#define CORTEX_M4_MPU_H

#include <stdint.h>
#include <stdbool.h>

/* ARMv7-M MPU Base Address & Register Map */
#define MPU_BASE_ADDR           (0xE000ED90UL)

typedef struct
{
    volatile const uint32_t TYPE;       /* 0x00: MPU Type Register */
    volatile uint32_t       CTRL;       /* 0x04: MPU Control Register */
    volatile uint32_t       RNR;        /* 0x08: MPU Region Number Register */
    volatile uint32_t       RBAR;       /* 0x0C: MPU Region Base Address Register */
    volatile uint32_t       RASR;       /* 0x10: MPU Region Attribute and Size Register */
} MPU_TypeDef;

#define MPU                     ((MPU_TypeDef *)MPU_BASE_ADDR)

/* MPU_TYPE Bit Definitions */
#define MPU_TYPE_DREGION_Pos    (8U)
#define MPU_TYPE_DREGION_Msk    (0xFFUL << MPU_TYPE_DREGION_Pos)

/* MPU_CTRL Bit Definitions */
#define MPU_CTRL_ENABLE_Msk     (1UL << 0U)
#define MPU_CTRL_HFNMIENA_Msk   (1UL << 1U)
#define MPU_CTRL_PRIVDEFENA_Msk (1UL << 2U)

/* MPU_RBAR Bit Definitions */
#define MPU_RBAR_VALID_Msk      (1UL << 4U)
#define MPU_RBAR_ADDR_Msk       (0xFFFFFFE0UL)

/* MPU_RASR Bit Definitions */
#define MPU_RASR_ENABLE_Msk     (1UL << 0U)
#define MPU_RASR_SIZE_Pos       (1U)
#define MPU_RASR_SRD_Pos        (8U)
#define MPU_RASR_B_Msk          (1UL << 16U)
#define MPU_RASR_C_Msk          (1UL << 17U)
#define MPU_RASR_S_Msk          (1UL << 18U)
#define MPU_RASR_TEX_Pos        (19U)
#define MPU_RASR_AP_Pos         (24U)
#define MPU_RASR_XN_Msk         (1UL << 28U)

/* MPU Access Permission (AP) Encodings */
typedef enum
{
    MPU_AP_NO_ACCESS        = 0x0U,     /* Privileged: No access, Unprivileged: No access */
    MPU_AP_PRIV_RW          = 0x1U,     /* Privileged: RW,        Unprivileged: No access */
    MPU_AP_PRIV_RW_USER_RO  = 0x2U,     /* Privileged: RW,        Unprivileged: Read-only */
    MPU_AP_FULL_ACCESS      = 0x3U,     /* Privileged: RW,        Unprivileged: RW */
    MPU_AP_PRIV_RO          = 0x5U,     /* Privileged: Read-only, Unprivileged: No access */
    MPU_AP_ALL_RO           = 0x6U      /* Privileged: Read-only, Unprivileged: Read-only */
} mpu_access_perm_t;

/* MPU Region Size Encodings: Size = 2^(N + 1) bytes */
typedef enum
{
    MPU_SIZE_32B            = 0x04U,
    MPU_SIZE_64B            = 0x05U,
    MPU_SIZE_128B           = 0x06U,
    MPU_SIZE_256B           = 0x07U,
    MPU_SIZE_512B           = 0x08U,
    MPU_SIZE_1KB            = 0x09U,
    MPU_SIZE_2KB            = 0x0AU,
    MPU_SIZE_4KB            = 0x0BU,
    MPU_SIZE_8KB            = 0x0CU,
    MPU_SIZE_16KB           = 0x0DU,
    MPU_SIZE_32KB           = 0x0EU,
    MPU_SIZE_64KB           = 0x0FU,
    MPU_SIZE_128KB          = 0x10U,
    MPU_SIZE_256KB          = 0x11U,
    MPU_SIZE_512KB          = 0x12U
} mpu_region_size_t;

/* MPU Region Configuration Structure */
typedef struct
{
    uint8_t           region_num;       /* Region index (0 to 7 on Cortex-M4) */
    uint32_t          base_addr;        /* Must be aligned to region size */
    mpu_region_size_t size;             /* Encoded region size */
    mpu_access_perm_t access_perm;      /* Read/Write access permissions */
    bool              exec_never;       /* true = Execute Never (XN), false = Executable */
    bool              shareable;        /* S bit */
    bool              cacheable;        /* C bit */
    bool              bufferable;       /* B bit */
} mpu_region_config_t;

/* Public API Prototypes */
uint8_t MPU_GetRegionCount(void);
void MPU_Disable(void);
void MPU_Enable(bool use_default_mem_map);
bool MPU_ConfigureRegion(const mpu_region_config_t *config);

#endif /* CORTEX_M4_MPU_H */
