# Day 087: SRAM Code Execution (`.RamFunc`) vs Flash Execution

### Objective

Investigate and benchmark instruction execution from **SRAM (`0x20000000`)** versus **Embedded Flash (`0x08000000`)** on the `STM32F401RE` (ARM Cortex-M4) microcontroller. This milestone demonstrates how linker sections (`.RamFunc`) and startup memory-copy routines relocate machine code from non-volatile Flash (LMA) into volatile SRAM (VMA) at boot, and profiles real-time execution cycles using the **DWT Cycle Counter (`DWT->CYCCNT`)** across uncached Flash, ART-cached Flash, and SRAM.

---

### Key Technical Implementations

- **Linker & Startup Relocation (`.RamFunc` Section)**:
  - Utilized the `.RamFunc` section mapping (`>RAM AT> FLASH`) inside the `.data` initialization block of `STM32F401RETX_FLASH.ld`.
  - Applied `__attribute__((section(".RamFunc"), noinline))` to `run_in_sram()`, instructing `Reset_Handler` to copy the function's Thumb-2 machine instructions from Flash into SRAM prior to entering `main()`.
- **Runtime Function Address Verification**:
  - Inspected raw function pointer addresses via `uintptr_t` casts (`&run_in_flash` vs `&run_in_sram`) to confirm physical placement across the Cortex-M4 memory map (`0x0800xxxx` vs `0x2000xxxx`, with bit 0 set for Thumb state).
- **DWT Cycle Counter Benchmarking (`10,000` Iterations)**:
  - Measured exact clock cycles across three execution modes:
    1. **Flash Uncached (`FLASH->ACR = 0x00000002`)**: 2 Wait States enabled with Prefetch, I-Cache, and D-Cache disabled.
    2. **Flash Cached (`FLASH->ACR = 0x00000702`)**: 2 Wait States with full **ART Accelerator** enabled.
    3. **SRAM Execution (`.RamFunc`)**: Inherently zero wait-state memory access via the Cortex-M4 System Bus.

---

### Verification and Results (From Debugger Expression View)

#### 1. Physical Memory Placement Verification

| Variable | Captured Decimal | Hexadecimal Address | Memory Region & State |
| :--- | :--- | :--- | :--- |
| **`flash_func_addr`** | `134220861` | **`0x08000C3D`** | Embedded Flash (`.text` section, Thumb mode) |
| **`sram_func_addr`** | `536871369` | **`0x200001C9`** | Internal SRAM (`.RamFunc` relocated copy, Thumb mode) |

#### 2. CPU Execution Cycle Benchmark (`DWT_CYCCNT` for `10,000` Loops)

| Execution Mode | `FLASH->ACR` Config | Measured Cycles | Performance Observation |
| :--- | :--- | :--- | :--- |
| **Flash (Uncached)** | `0x00000002` | **`230,053`** | Slowest due to 2 Wait-State penalties on every instruction fetch |
| **Flash (ART Cached)** | `0x00000702` | **`190,048`** | **Fastest** — zero wait-state cache hits via dedicated **I-Code Bus** |
| **SRAM (`.RamFunc`)** | `0x00000702` | **`220,055`** | Faster than uncached Flash, ~30k cycles slower than cached Flash |

---

### Architectural Analysis: Why ART-Cached Flash Outperforms SRAM on Cortex-M4

- **Harvard Bus Matrix Separation**: On the ARM Cortex-M4, Flash (`0x00000000`–`0x1FFFFFFF`) is accessed via dedicated **I-Code** and **D-Code** buses, while SRAM (`0x20000000`+) is accessed via the **System Bus (S-Bus)**.
- **System Bus Contention in SRAM**: When executing `run_in_sram()`, both instruction fetches and stack variable read/writes (`volatile uint32_t acc` and loop counter `i`) share the same **System Bus**, introducing a 3-cycle bus arbitration overhead per loop iteration (`~30,000` cycles over `10,000` iterations).
- **True Engineering Purpose of `.RamFunc`**:
  1. **Self-Flashing & Bootloaders**: Prevents CPU bus stalls during Flash sector erase/program operations (since STM32F401 features a single-bank Flash).
  2. **Deterministic Real-Time ISRs**: Guarantees consistent execution latency without risk of cache misses during critical interrupt handling.

---

### Conclusion

Day 087 validates both the mechanical relocation of executable code into SRAM and the underlying ARM Cortex-M4 bus architecture. Combining Flash IAP knowledge (Day 086) with `.RamFunc` SRAM execution (Day 087) establishes the exact architectural foundation required to implement safe, stall-free custom bootloaders and OTA firmware update routines.