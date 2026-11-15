# Day 085: STM32F401 Memory Map & Bus Architecture Probing

### Objective

Explore and understand the 4GB address space architecture of the ARM Cortex-M4 core on the `STM32F401RE` micro-controller. This milestone focuses on mapping absolute physical memory boundaries (Flash, SRAM, and Peripherals) and implementing a runtime memory probing function to dynamically classify arbitrary pointers into their respective hardware bus regions.

---

### Key Technical Implementations

- **4GB Address Space Mapping**:
  - Leveraged the ARM Cortex-M4 unified memory map to segregate execution spaces, defining exact structural boundaries for Code, SRAM, Peripherals, and Private Peripheral Bus (PPB).
- **Bus Region Classification Logic (`mem_get_region_type`)**:
  - Implemented a deterministic boundary-checking function in C to inspect memory addresses at runtime and verify whether they map to internal Flash (`0x08000000`), SRAM (`0x20000000`), or Peripheral register blocks (`0x40000000`).
- **Direct Register Pointer Probing**:
  - Utilized structured peripheral base pointers (e.g., `&GPIOA->MODER`) to test real-world hardware address translation against expected bus ranges without reliance on high-level HAL abstraction layers.

---

### Verification and Results (From Debugger Expression View)

By setting up test pointers across distinct physical regions and inspecting them via the IDE **Expressions** view during runtime debugging:

- **Flash Pointer (`0x08000000`)**: Evaluated successfully as `MEM_TYPE_FLASH`.
- **SRAM Pointer (`0x20000000`)**: Evaluated successfully as `MEM_TYPE_SRAM`.
- **Peripheral Pointer (`&GPIOA->MODER`)**: Evaluated successfully as `MEM_TYPE_PERIPHERAL`.

### Memory Region Boundary Architecture:

- `0x00000000 - 0x1FFFFFFF` -> Code / Flash Region (ICode/DCode Bus)
- `0x20000000 - 0x3FFFFFFF` -> SRAM Region (System Bus / 96KB SRAM1)
- `0x40000000 - 0x5FFFFFFF` -> Peripheral Region (AHB/APB Bus Infrastructure)
- `0xE0000000 - 0xFFFFFFFF` -> Private Peripheral Bus / System Control Space (PPB/NVIC)

---

### Conclusion

Day 085 solidifies the foundational understanding of how the ARM Cortex-M4 CPU routes data across different hardware buses. By building a runtime memory region detector, firmware gains the ability to safely validate memory pointers before performing low-level read/write operations, preventing illegal bus accesses and unexpected faults.