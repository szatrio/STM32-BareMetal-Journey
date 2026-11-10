# Day 082: Custom Section Placement & Attribute-Based Memory Mapping

### Objective

Implement and verify custom user-defined memory sections in the bare-metal linker script (`STM32F401RETX_FLASH.ld`) combined with GCC `__attribute__((section(...)))` extensions. This milestone ensures precise control over variable allocation, allowing specific data structures or variables to be isolated into designated SRAM sub-segments.

---

### Key Technical Implementations

- **Custom Linker Section Definition (`.custom_ram_data`)**: 
  - Added a dedicated memory segment inside the `SECTIONS` command of the linker script to capture specialized user data.
  - Defined strict boundary symbols (`_scustom` and `_ecustom`) around the custom section to track its exact memory footprint and alignment.
- **GCC Attribute-Based Variable Placement**: 
  - Utilized `__attribute__((section(".custom_data")))` on global variables in `main.c` (e.g., `my_custom_variable`) to force the compiler and linker to route them directly into our custom SRAM segment rather than default areas.
- **LMA and VMA Separation**: 
  - Maintained proper dual-addressing where initialized custom data retains its load image in Flash (`AT> FLASH`) and executes/resides in SRAM at runtime.

---

### Segment Mapping Verification (`.map` Analysis)

By inspecting the generated linker map file (`STM32-BareMetal-Journey.map`), the physical placement of the custom variable and section boundaries was successfully validated:

- **Precise SRAM Allocation**: `my_custom_variable` successfully landed at the exact calculated VMA address (`0x200001c8`), perfectly nested inside `.custom_ram_data`.
- **Boundary Tracking**: The start and end symbols (`_scustom` and `_ecustom`) correctly bound the 4-byte allocation of the `uint32_t` variable.
- **Seamless Integration**: The custom section integrates cleanly into the SRAM memory map right before the `.bss` section without causing fragmentation or overlap anomalies.

```text
Memory Region Layout Snapshot (from .map file):
.custom_ram_data    0x200001c8      0x4    --> Isolated custom SRAM segment start
  |_ my_custom_variable 0x200001c8    0x4    --> Target variable placed via attribute
  |_ _ecustom       0x200001cc             --> Custom section boundary end
.bss                0x200001cc      0x150  --> Uninitialized data continuation
```

### Conclusion

Day 082 successfully demonstrates absolute control over the GCC compiler and linker layout. By forcing custom variables into dedicated SRAM sections and verifying them via .map symbol analysis, the bare-metal build architecture achieves advanced memory isolation capabilities, paving the way for sophisticated runtime memory management and custom data buffering.