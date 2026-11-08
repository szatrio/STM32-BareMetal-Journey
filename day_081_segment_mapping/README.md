# Day 081: Segment Mapping Verification & .data/.bss Layout

### Objective

Perform deep-dive segment mapping and address validation for the custom bare-metal linker script (`STM32F401RETX_FLASH.ld`). This milestone focuses on validating the runtime separation of `.text`/`.rodata` (Flash) from `.data`/`.bss` (SRAM) through meticulous linker symbol verification and map file inspection.

---

### Key Technical Implementations

- **Explicit `.text` & `.rodata` Isolation**: 
  - Separated executable code (`.text`) and read-only constants/string banners (`.rodata`) into dedicated sections mapped securely within the non-volatile Flash region (`>FLASH`).
- **Initialized Data (`.data`) Runtime Allocation**: 
  - Configured initialized global variables to reside in SRAM at runtime (`>SRAM`) while retaining their initial load images in Flash (`AT> FLASH`), managed via `_sidata`, `_sdata`, and `_edata` runtime symbols.
- **Uninitialized Data (`.bss`) Zero-Filling**: 
  - Mapped uninitialized variables to the `.bss` section inside SRAM, bounded by `_sbss` and `_ebss` symbols to ensure proper zero-initialization sequences during system boot.

---

### Segment Mapping Verification (`.map` Analysis)

By inspecting the generated linker map file (`STM32-BareMetal-Journey.map`), the physical placement of variables and symbols was successfully verified against the target memory architecture:

- **`.data` Allocation**: `global_initialized_var` correctly starts precisely at the base of the SRAM boundary (`0x20000000`), proving successful variable initialization and linker relocation.
- **`.bss` Continuity**: `global_uninitialized_var` lands seamlessly right after the `.data` section at `0x200001cc`, ensuring zero memory fragmentation.
- **Heap & Stack Boundaries**: The `._user_heap_stack` block correctly follows the `.bss` termination point (`0x20000320`), reserving designated space for the 512-byte heap and 1KB stack anchored safely below `_estack`.

```text
Memory Region Layout Snapshot (from .map file):
.data           0x20000000      0x1cc --> SRAM Start (Initialized vars & libc data)
.bss            0x200001cc      0x154 --> SRAM Continuous (Uninitialized vars & zero-fill)
._user_heap_stack 0x20000320    0x600 --> Heap & Stack structural allocation
```

### Conclusion

Day 081 successfully verifies the absolute correctness of the custom memory segment routing. By leveraging map file diagnostics to confirm exact SRAM/Flash boundary mapping without anomalies, the bare-metal build architecture achieves production-grade reliability and is fully ready for advanced runtime integration.