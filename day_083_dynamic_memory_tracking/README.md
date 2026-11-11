# Day 083: Memory Headroom & Stack Monitor 

### Objective

Implement a robust, deterministic memory tracking mechanism in a bare-metal environment (`STM32F401RE`) without violating safety standards. This milestone focuses on real-time **Stack-Heap Headroom Monitoring** using linker symbols (`_end`, `_estack`) and inline assembly register reading (`MSP`), completely avoiding dynamic memory allocation (`malloc`) in compliance with [MISRA C:2012 (Rule 21.3)](https://github.com/szatrio/Embedded-Compliance-Standards/blob/main/MISRA-C-2012/Rule-21.03.c).

---

### Key Technical Implementations

- **MISRA-Compliant Architecture (No `malloc`)**: 
  - Strictly eliminated dynamic memory allocation functions (`malloc`, `free`, `_sbrk`) to prevent non-deterministic timing, memory fragmentation, and fatal heap-stack collisions.
- **Linker Symbol Extraction (`_end`)**: 
  - Imported external linker boundaries (`_end`) into C to establish a static baseline address (`0x20000320`) representing the end of BSS/custom sections and the start of the heap region.
- **Inline Assembly Stack Pointer Tracking (`MSP`)**: 
  - Utilized pure GCC inline assembly (`MRS %0, msp`) to read the hardware **Main Stack Pointer** dynamically at runtime without depending on bloated external CMSIS library headers.
- **Safe Headroom Calculation**: 
  - Continuously calculated the safe distance between the downward-growing Stack Pointer and the upward-bound heap baseline to detect potential overflows instantly.

---

### Segment Mapping & Headroom Calculation (From .map Analysis)

By analyzing the linker map file (`STM32-BareMetal-Journey.map`), the absolute static layout was established to compute the available SRAM headroom:

- **Heap/BSS Boundary (_end)**: Located at **`0x20000320`** (post-alignment).
- **Stack Top Boundary (_estack)**: Located at the absolute end of the 96KB SRAM region: **`0x20018000`**.

### Mathematical Calculation:

    Max Headroom = estack - end

    Max Headroom = 0x20018000 - 0x20000320 = **0x17CE0 bytes**

    Converting to decimal and kilobytes:

    0x17CE0 = 97,504 bytes ≈ **95.2 KB**

### Memory Layout Structure:

* `0x20000000` -> SRAM Start (.data, .bss, .custom_ram_data)
* `0x20000320` -> `_end` / Heap Baseline
* `[ Available Stack-Heap Headroom ~95.2 KB ]` (Stack grows downward, MSP shifts dynamically)
* `0x20018000` -> `_estack` / Top of SRAM

---

### Conclusion

Day 083 successfully proves that rigorous safety-critical memory tracking can be achieved cleanly and deterministically. By combining linker symbol extraction with inline assembly register reading, the system monitors its stack-heap headroom in real-time while strictly adhering to MISRA C safety guidelines.