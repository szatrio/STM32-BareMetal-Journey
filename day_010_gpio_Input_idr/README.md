
# Project Title

A brief description of what this project does and who it's for

# Day 010: Bare-Metal Peripheral Control (GPIO Input & Clock Gating)

### Objective
Successfully implement bare-metal firmware to read a physical push-button (B1) on PC13 and control an onboard LED (PA5), bypassing standard HAL libraries to interact directly with STM32 registers.

### Key Learnings

- **Register-Level Input/Output:** Understood the difference between Output Data Register (`ODR`) and Input Data Register (`IDR`). Reading the state of a pin involves masking and shifting bits within the `IDR`.
- **Memory Mapping:** Mastered the concept of *Base Address + Offset*. Realized that while different ports (A and C) share the same register offsets (e.g., `MODER` at `0x00`), they point to distinct memory locations because their base addresses are different.


### Technical Implementation
- **Pin Configuration:**
    - **LED (PA5):** Configured as Push-Pull Output.
    - **Button (PC13):** Configured as Input (using `GPIOC_MODER`).
- **Clock Setup:** Enabled clock access for both GPIOA and GPIOC simultaneously using `RCC_AHB1ENR |= (1UL << 0) | (1UL << 2)`.
- **Logic:** Implemented a continuous `while(1)` loop that polls `GPIOC_IDR` for the input state to toggle the `GPIOA_ODR` output.

### Hardware/Registers Involved
- **Registers:**
    - `RCC_AHB1ENR`: Peripheral clock control.
    - `GPIOx_MODER`: Pin mode selection (Input/Output).
    - `GPIOx_IDR`: Reading input pin levels.
    - `GPIOx_ODR`: Writing output pin levels.

### Conclusion
This experiment solidified the fundamental understanding of how firmware interacts with hardware at the register level. Transitioning from HAL to bare-metal control provides full transparency of the underlying CPU instructions and memory operations.