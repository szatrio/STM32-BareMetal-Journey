# Day 004: Modular GPIO Mode Configuration & Static Inline Functions

### Objective
Implement generic, pin-parameterized GPIO driver functions using `static inline` functions in `stm32f401_registers.h` to automate bitwise position arithmetic for 2-bit mode registers and pin toggling.

### Key Learnings

- **2-Bit Mode Field Bit Arithmetic:** Understood that every GPIO pin occupies 2 consecutive bits in `GPIOx_MODER` (`MODERy[1:0]`). Calculated bit shifting dynamically using `(pin * 2UL)` to safely mask and assign modes across any pin (0 to 15).
- **Static Inline Functions vs Macros:** Learned why `static inline` functions are superior to standard preprocessor macros in embedded C—they provide strict compiler type-checking, improve code readability, and eliminate function call overhead by directly inlining assembly instructions.
- **STM32 GPIO Hardware Modes:** Understood the 4 native configuration modes defined in the reference manual: Input (`00`), General Purpose Output (`01`), Alternate Function (`10`), and Analog (`11`).

### Technical Implementation

- **Driver Helper API (`stm32f401_registers.h`):**
    - `RCC_EnableGPIOClock()`: Encapsulated GPIOA peripheral clock enablement.
    - `GPIOA_SetMode(uint8_t pin, uint32_t mode)`: Programmatically computed bitmask `(3UL << (pin * 2UL))` to clear existing mode bits before OR-ing the target mode `(mode << (pin * 2UL))`.
    - `GPIOA_TogglePin(uint8_t pin)`: Generalized bitwise XOR pin toggling using `(1UL << pin)`.
- **Application Layer (`main.c`):**
    - Replaced hardcoded bit shifts with clean function calls: `GPIOA_SetMode(5, GPIO_MODE_OUTPUT)`.
    - Toggled PA5 in a `while(1)` super-loop using `GPIOA_TogglePin(5)`.

### Hardware/Registers Involved

- **Registers:**
    - `RCC_AHB1ENR` (Offset `0x30`): Enabled clock for GPIOA via `RCC_EnableGPIOClock()`.
    - `GPIOA_MODER` (Offset `0x00`): Configured PA5 mode using dynamic 2-bit shifted bitmask.
    - `GPIOA_ODR` (Offset `0x14`): Toggled PA5 output state using dynamic single-bit shift.

### Conclusion
Day 004 elevated driver design from hardcoded register macros to scalable driver APIs. Utilizing `static inline` functions and dynamic bit-positioning formulas (`pin * 2`) allows clean driver expansion across multiple GPIO pins without compromising execution speed or type safety.