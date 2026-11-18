# Day 088: Memory Protection Unit (MPU) Basic Setup

### Objective

Implement a modular, bare-metal driver for the ARMv7-M **Memory Protection Unit (MPU)** (`0xE000ED90`) on the `STM32F401RE` microcontroller. This milestone covers querying hardware MPU capabilities, enforcing memory alignment constraints, configuring region access permissions (**Read-Only** and **Execute-Never**), synchronizing the CPU pipeline via memory barriers (`DSB`/`ISB`), and verifying hardware-level fault interception upon illegal write attempts.

---

### Key Technical Implementations

- **Bare-Metal MPU Driver (`Inc/cortex_m4_mpu.h` & `Src/cortex_m4_mpu.c`)**:
  - Mapped the core MPU register block (`MPU->TYPE`, `MPU->CTRL`, `MPU->RNR`, `MPU->RBAR`, `MPU->RASR`) directly at `0xE000ED90`.
  - Implemented `MPU_GetRegionCount()` to extract `DREGION` (`bits [15:8]`) from `MPU->TYPE`, confirming 8 independent hardware regions.
  - Implemented `MPU_ConfigureRegion()` with automatic power-of-two base address alignment validation (`(base_addr & (size_bytes - 1)) == 0`) to prevent hardware misconfiguration.
  - Enforced deterministic pipeline synchronization using inline ARM assembly barriers (`dsb 0xF` and `isb 0xF`) inside `MPU_Disable()` and `MPU_Enable()`.
- **Region 0 Read-Only & Execute-Never (XN) Policy**:
  - Declared a 32-byte aligned SRAM buffer (`__attribute__((aligned(32)))`) to meet the minimum ARMv7-M MPU region granularity.
  - Configured Region 0 with `MPU_AP_PRIV_RO` (Privileged Read-Only) and `exec_never = true` (`XN` bit set), while enabling `PRIVDEFENA` in `MPU->CTRL` (`0x05`) so unmapped background memory remains accessible to privileged code.
- **Hardware Access Verification**:
  - Verified normal read operations from `protected_buffer[0]` succeed under the Read-Only policy, followed by an intentional write (`0xDEADBEEF`) to verify immediate hardware interception into the fault handler.

---

### Verification and Results (From Debugger Expression View)

By stepping through execution (`Step Over`) up to the illegal write instruction and inspecting variables in the IDE **Expressions** panel:

| Variable | Captured Decimal | Hexadecimal Equivalent | Hardware Verification Meaning |
| :--- | :--- | :--- | :--- |
| **`mpu_regions`** | `8` | `0x08` | Hardware MPU supports 8 independent protection regions |
| **`config_ok`** | `true` (`1`) | `0x01` | 32-byte alignment validated and Region 0 programmed cleanly |
| **`mpu_ctrl_stat`** | `5` | **`0x00000005`** | `MPU_CTRL_ENABLE` (bit 0) and `PRIVDEFENA` (bit 2) active |
| **`read_val`** | `286331153` | **`0x11111111`** | Legal read access from `protected_buffer[0]` succeeded |

### MPU Protection Execution Flow:

- `Query Regions` -> Read `MPU->TYPE` (`DREGION = 8`)
- `Disable MPU` -> Execute `DSB`, clear `MPU->CTRL`, execute `ISB`
- `Configure Region 0` -> Set 32-byte aligned base in `MPU->RBAR`, set `SIZE = 32B`, `AP = PRIV_RO`, `XN = 1` in `MPU->RASR`
- `Enable MPU` -> Set `ENABLE | PRIVDEFENA` (`0x05`) in `MPU->CTRL`, execute `DSB` & `ISB`
- `Verify Policy` -> Read `0x11111111` (Allowed) -> Attempt write `0xDEADBEEF` (Blocked by MPU hardware -> Fault triggered)

---

### Conclusion

Day 088 establishes a reusable, register-level MPU driver for the ARM Cortex-M4. Enforcing spatial memory isolation and hardware access rules prevents accidental corruption of critical data structures and forms the foundation for stack overflow guard zones and RTOS task isolation.