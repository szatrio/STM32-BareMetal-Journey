# Day 003: Bitwise Masking & Helper Macro Abstraction

### Objective
Implement safe bitwise operations and helper macros for peripheral clock gating and GPIO register manipulation, preventing unintended modifications to adjacent register bits.

### Key Learnings

- **Safe Read-Modify-Write Operations:** Learned why raw register assignment (`=`) is dangerous for multi-bit fields and mastered the 2-step bitwise masking technique: clear target bits with bitwise AND-NOT (`&= ~`), then set desired values using bitwise OR (`|=`).
- **Atomic Bit Defintions & Bitshifting:** Understood how defining descriptive bit-position macros (e.g., `RCC_GPIOA_EN`) improves code readability and eliminates hardcoded "magic numbers" in main application logic.
- **Helper Macro Encapsulation:** Mastered wrapping repetitive bitwise manipulation logic into functional helper macros (`RCC_ENABLE_GPIOA()`) to make application code self-documenting.

### Technical Implementation

- **Bitwise Definitions & Macros (`stm32f401_registers.h`):**
    - Defined `RCC_GPIOA_EN` as `(1 << 0)` to explicitly target the GPIOA clock enable bit in `RCC_AHB1ENR`.
    - Created abstraction macros `RCC_ENABLE_GPIOA()` and `RCC_DISABLE_GPIOA()` using read-modify-write syntax.
- **Application Logic (`main.c`):**
    - Safely enabled GPIOA clock using `RCC_ENABLE_GPIOA()`.
    - Masked out mode bits 10 and 11 in `GPIOA_MODER` using `~(3 << 10)` before writing `(1 << 10)` to configure PA5 as Output.
    - Toggled PA5 output in a super-loop using bitwise XOR (`^= (1 << 5)`).

### Hardware/Registers Involved

- **Registers:**
    - `RCC_AHB1ENR` (Offset `0x30`): Controlled safely via `RCC_ENABLE_GPIOA()` macro (Bit 0).
    - `GPIOA_MODER` (Offset `0x00`): Pin mode setup utilizing 2-bit mask operations (Bits 11:10).
    - `GPIOA_ODR` (Offset `0x14`): Output state toggled via XOR operation (Bit 5).

### Conclusion
Day 003 reinforced safe embedded C programming practices through bitwise masking. Utilizing explicit bit-clearing masks ensures robust peripheral configuration without corrupting adjacent bit states sharing the same register.