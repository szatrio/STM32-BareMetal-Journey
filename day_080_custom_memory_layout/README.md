# Day 080: Custom Linker Script & Memory Layout Architecture

### Objective

Transition from vendor-provided default linker scripts to a custom, manually architected linker script (`STM32F401RETX_FLASH.ld`) for the STM32F401RE microcontroller. This milestone focuses on taking absolute control over physical memory boundaries, explicitly defining Flash and SRAM regions, and managing runtime memory symbols (`_estack`, `_sdata`, `_sbss`, etc.) from first principles.

---

### Key Technical Implementations

- **Custom Memory Boundary Mapping (`MEMORY`)**: 
  - Explicitly defined physical memory regions tailored for the STM32F401RE chip architecture, allocating 512KB of Flash starting at `0x08000000` and 96KB of SRAM starting at `0x20000000`.
- **Runtime Segment & Symbol Regulation (`SECTIONS`)**: 
  - Manually mapped `.isr_vector`, `.text`, `.rodata`, `.data`, and `.bss` sections to ensure proper startup code execution, zero-filling for uninitialized data, and correct data-copying routines from Flash to SRAM.
- **Stack & Heap Allocation Control**: 
  - Defined strict boundaries for user heap and stack sizes (`_Min_Heap_Size` and `_Min_Stack_Size`) anchored directly to the high-water mark of the SRAM region (`_estack`).

---

### Conclusion

Day 080 successfully establishes complete architectural control over the bare-metal build environment. By successfully authoring, linking, and flashing a custom memory layout without vendor abstraction templates, the system is fully primed for advanced low-level kernel memory management and upcoming FreeRTOS integration phases.