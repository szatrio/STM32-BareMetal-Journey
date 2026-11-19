# Day 089: MPU Stack Guard & MemManage Fault Diagnostics

### Objective

Implement a hardware-enforced **Stack Overflow Guard Zone** on the `STM32F401RE` microcontroller using the ARMv7-M **Memory Protection Unit (MPU)** and dedicated **MemManage Fault** diagnostics (`SCB->SHCSR`, `SCB->CFSR`, and `SCB->MMFAR`). This milestone demonstrates how to prevent silent stack corruption of adjacent `.bss`/`.data` SRAM variables and capture the exact offending memory address at the moment a stack overflow occurs.

---

### Key Technical Implementations

- **System Control Block (SCB) Fault Definitions (`Inc/cortex_m4_core.h`)**:
  - Extended core Cortex-M4 definitions with bitmasks for enabling dedicated MemManage exceptions (`SCB_SHCSR_MEMFAULTENA_Msk` at bit 16) and decoding the lower 8-bit **MemManage Fault Status Register (`MMFSR`)** inside `SCB->CFSR` (`DACCVIOL`, `MSTKERR`, and `MMARVALID`).
- **Dynamic Linker-Derived Stack Guard (`Src/main.c`)**:
  - Calculated the exact lower boundary of the main stack dynamically from linker script symbols (`_estack` and `_Min_Stack_Size`) aligned to a 32-byte MPU boundary (`0x20017C00` – `0x20017C1F`).
  - Configured **MPU Region 0** across this 32-byte guard zone with `MPU_AP_NO_ACCESS` and `exec_never = true`, while keeping `PRIVDEFENA` enabled in `MPU->CTRL` so normal stack and peripheral operations remain unaffected.
- **Dedicated `MemManage_Handler()` & Controlled Overflow Simulation**:
  - Enabled `MEMFAULTENA` in `SCB->SHCSR` so MPU violations route directly to `MemManage_Handler()` instead of escalating to `HardFault_Handler`.
  - Implemented `MemManage_Handler()` to inspect `SCB->CFSR & 0xFF` and extract the exact faulting address from `SCB->MMFAR` when `MMARVALID` is asserted.
  - Executed a controlled recursive function (`trigger_stack_overflow()`) allocating a 16-word local buffer per frame until the Stack Pointer (`SP`) breached the 32-byte guard region.

---

### Verification and Results (From Debugger Expression View)

By running the firmware in the IDE Debugger and inspecting the diagnostic variables once execution halted inside `MemManage_Handler()`:

| Variable | Captured Decimal | Hexadecimal Equivalent | Hardware Verification Meaning |
| :--- | :--- | :--- | :--- |
| **`guard_ok`** | `1` (`true`) | `0x01` | MPU Region 0 (32B `No-Access` at `0x20017C00`) configured cleanly |
| **`recursion_depth`** | `10` | `0x0A` | Stack exhausted after 10 recursive frames; overflow caught on frame 11 |
| **`fault_mmfsr`** | `146` | **`0x92`** | `MMARVALID` (`0x80`) + `MSTKERR` (`0x10`) + `DACCVIOL` (`0x02`) active |
| **`fault_mmar_valid`** | `1` (`true`) | `0x01` | Confirmed `SCB->MMFAR` holds a valid offending memory address |
| **`fault_mmfar`** | `536968212` | **`0x20017C14`** | Exact SRAM address inside the 32B guard (`0x20017C00`–`0x20017C1F`) where stack breached |

### Stack Guard Interception Flow:

- `Calculate Boundary` -> Derive 32-byte aligned stack base (`0x20017C00`) from `_estack` (`0x20018000`) and `_Min_Stack_Size` (`0x400`)
- `Enable MemManage` -> Set `SCB_SHCSR_MEMFAULTENA_Msk` in `SCB->SHCSR`
- `Arm Stack Guard` -> Program MPU Region 0 (`32B`, `MPU_AP_NO_ACCESS`, `XN = 1`) and enable MPU with `PRIVDEFENA`
- `Trigger Recursion` -> Push stack frames downward until `SP` crosses into `0x20017C00`–`0x20017C1F`
- `Hardware Interception` -> MPU blocks stack write at `0x20017C14` -> CPU enters `MemManage_Handler()` with `MMFSR = 0x92`

---

### Conclusion

Day 089 turns silent, hard-to-debug stack overflows into deterministic, immediately traceable hardware faults. By pairing a 32-byte `No-Access` MPU guard region at the base of the stack with `SCB->CFSR` and `SCB->MMFAR` diagnostics, the microcontroller halts execution before adjacent static variables (`.bss`/`.data`) can be corrupted and pinpoints the exact faulting address.