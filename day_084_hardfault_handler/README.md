# Day 084: Interactive Hard Fault Handler & Post-Mortem Diagnostics

### Objective

Implement an industrial-grade, deterministic **Hard Fault Handler** on the `STM32F401RE` (ARM Cortex-M4) to capture critical CPU hardware registers during runtime exceptions. Instead of allowing the system to silently hang in an uninformative infinite loop or perform blind resets, this milestone extracts the active stack frame (`MSP`/`PSP`) via naked inline assembly to pinpoint the exact instruction address (`PC`) and caller (`LR`) responsible for a crash.

---

### Key Technical Implementations

- **Naked Assembly Exception Entry (`HardFault_Handler`)**:
  - Implemented `__attribute__((naked))` to prevent the compiler from generating function prologue/epilogue instructions that would corrupt the hardware-stacked registers upon exception entry.
- **Active Stack Pointer Resolution (`EXC_RETURN`)**:
  - Evaluated bit 2 of the Link Register (`tst lr, #4`) using conditional `ITE` (If-Then-Else) assembly instructions to dynamically determine whether the **Main Stack Pointer (`MSP`)** or **Process Stack Pointer (`PSP`)** was active prior to the fault, passing the exact base address into `R0`.
- **Hardware Stack Frame Extraction (`StackFrame_t`)**:
  - Mapped the automatically pushed ARM Cortex-M4 exception frame (`R0`, `R1`, `R2`, `R3`, `R12`, `LR`, `PC`, `PSR`) into a structured C pointer for immediate post-mortem inspection.
- **System Control Block (SCB) Fault Register Decoding**:
  - Directly read memory-mapped fault status and address registers (`CFSR` at `0xE000ED28`, `MMFAR` at `0xE000ED34`, `BFAR` at `0xE000ED38`) without external HAL overhead.

---

### Post-Mortem Register Analysis (From Live Debugger Capture)

By intentionally triggering an invalid memory write (`*0x00000000 = 0xDEADBEEF`), execution trapped cleanly inside `fault_print_diagnostics()`, capturing the exact hardware state at `0x20017FA0`[cite: 10, 11]:

- **Active Stack Frame Base**: **`0x20017FA0`** (in SRAM)[cite: 10, 11]
- **Program Counter (`PC`)**: `134231844` -> **`0x08003744`** (Exact Flash address of the faulting instruction)[cite: 11]
- **Link Register (`LR`)**: `134224249` -> **`0x08001859`** (Return address of the caller function)[cite: 11]
- **Configurable Fault Status (`CFSR`)**: `65536` -> **`0x00010000`** (`INVSTATE` / UsageFault bit active)[cite: 10, 11]

### Exception Flow Architecture:

- `Normal Execution` -> Fault condition triggered (invalid memory access / state)
- `Hardware Stacking` -> Cortex-M4 core automatically pushes `R0-R3, R12, LR, PC, PSR` onto active stack
- `HardFault_Handler` -> Naked ASM inspects `LR` bit 2, loads `MSP`/`PSP` into `R0`, branches to C
- `fault_print_diagnostics` -> Reads `CFSR`/`MMFAR`/`BFAR` and halts cleanly for GDB inspection

---

### Conclusion

Day 084 establishes a critical post-mortem debugging safety net essential for bare-metal firmware engineering. By bridging low-level ARM Cortex-M4 exception mechanics with structured C diagnostics, any future hard fault can be traced directly back to its exact assembly/C source instruction in seconds.