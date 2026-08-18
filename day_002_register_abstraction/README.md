# Day 002: Modular Header Abstraction (Register Offset Definition)

### Objective
Refactor direct memory-mapped register macros into a dedicated header file (`stm32f401_registers.h`), establishing a clean architectural separation between raw address calculations and application logic in `main.c`.

### Key Learnings

- **Bus Memory Hierarchy:** Learned how peripheral addresses are derived hierarchically in ARM Cortex-M architecture (`PERIPH_BASE` -> `AHB1PERIPH_BASE` -> `RCC_BASE` / `GPIOA_BASE`).
- **Offset-Based Addressing:** Understood that specifying register addresses using `Base Address + Offset` mirrors official STM32 reference manuals, making driver code scalable, maintainable, and readable.
- **Header File Guards:** Mastered using `#ifndef`, `#define`, and `#endif` macro preprocessor directives to prevent double inclusion errors during compilation.

### Technical Implementation

- **Header File Abstraction (`stm32f401_registers.h`):**
    - Defined system bus base memory (`0x40000000UL`) and AHB1 peripheral bus offset (`0x00020000UL`).
    - Computed `RCC_BASE` (`0x40023800UL`) and `GPIOA_BASE` (`0x40020000UL`).
    - Mapped register macros (`RCC_AHB1ENR`, `GPIOA_MODER`, `GPIOA_ODR`) using explicit `volatile uint32_t *` pointer casting.
- **Application Layer (`main.c`):**
    - Cleanly included `stm32f401_registers.h` and executed clock enabling, pin mode setup, and bit toggling without cluttering application code with raw hex values.

### Hardware/Registers Involved

- **Registers:**
    - `RCC_AHB1ENR` (Offset `0x30`): Peripheral clock enable register.
    - `GPIOA_MODER` (Offset `0x00`): GPIO port mode register.
    - `GPIOA_ODR` (Offset `0x14`): GPIO port output data register.

### Conclusion
Day 002 demonstrated the first step toward modular firmware architecture. Decoupling memory definitions into dedicated header files simplifies register management and lays the foundation for building custom peripheral HAL/LL drivers.