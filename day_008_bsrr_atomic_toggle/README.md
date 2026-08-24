# Day 008: Atomic Pin State Operations via BSRR (Thread-Safe Bit Operations)

### Objective
Eliminate Read-Modify-Write (RMW) race conditions by implementing atomic pin control via the `GPIOx_BSRR` register, using direct single-cycle write operations (`=`) alongside local software state caching to achieve atomic pin toggling.

### Key Learnings

- **Atomic Hardware Operations (`GPIOx_BSRR`):**
  - Bit Set/Reset Register allows direct atomic modification without needing to read the current state of the register first.
  - Writing a `1` to bits `[15:0]` sets the corresponding output pin HIGH (`BSy`).
  - Writing a `1` to bits `[31:16]` resets the corresponding output pin LOW (`BRy`).
  - Writing `0` to any bit has **no effect**, preserving other pins without masking operations.
- **RMW Safety vs Direct Assignment:**
  - Standard `ODR` manipulations (`|=` and `&= ~`) compile into multi-step Assembly sequences (Read $\rightarrow$ Modify $\rightarrow$ Write), making them vulnerable to interrupt preemptions.
  - `BSRR` allows using pure assignment (`=`), executing as a single-cycle write instruction in assembly, rendering the operation thread-safe/ISR-safe without disabling interrupts.
- **State Caching Strategy:**
  - Since `BSRR` is a write-only register for bit operations, a soft variable (`gpio_status_cache`) maintains the visual state shadow to decide whether to set (`pin`) or reset (`pin + 16`) atomically during toggle routines.

### Technical Implementation

- **Register Layer Update (`stm32f401_registers.h`):**
  - Mapped `GPIOA_BSRR_OFFSET` at `0x18UL`.
  - Added volatile pointer macro `GPIOA_BSRR`.
- **Driver Layer Separation (`stm32f401_gpio.h` / C Module):**
  - Separated header prototypes/inline utilities from implementation logic.
  - Implemented `GPIO_TogglePin(uint8_t pin)` utilizing `GPIOA_BSRR`:
    - Set state: `GPIOA_BSRR = (1UL << pin)`
    - Reset state: `GPIOA_BSRR = (1UL << (pin + 16UL))`
  - Kept track of pin outputs using a file-scoped status cache `gpio_status_cache`.
- **Application Layer (`main.c`):**
  - Driven PA5 using the new atomic `GPIO_TogglePin` function within the main super-loop.

### Hardware/Registers Involved

- **Registers:**
  - `RCC_AHB1ENR` (Offset `0x30`): GPIOA clock gating.
  - `GPIOA_MODER` (Offset `0x00`): Pin mode selector.
  - `GPIOA_OTYPER` (Offset `0x04`): Output type configuration.
  - `GPIOA_PUPDR` (Offset `0x0C`): Internal pull configuration.
  - `GPIOA_BSRR` (Offset `0x18`): Atomic bit set (low 16-bits) and bit reset (high 16-bits) register.

### Conclusion
Day 008 solves the fundamental race condition problem inherent to standard Read-Modify-Write operations on hardware registers. Transitioning pin manipulations to `BSRR` guarantees thread safety across interrupt service routines (ISRs) and multi-tasking contexts.