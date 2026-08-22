# Day 006: GPIO Pull-Up/Pull-Down Configuration (DRY Driver Refactoring)

### Objective
Implement internal resistor pull-up/pull-down configuration (`GPIOx_PUPDR`) in the driver, introduce strongly-typed PUPDR enumerations, and apply the DRY (Don't Repeat Yourself) principle by refactoring repetitive 2-bit field register manipulation into a generalized helper function.

### Key Learnings

- **Internal Resistor Mechanics (PUPDR):** Understood how internal pull-up and pull-down resistors prevent floating input states (unpredictable high-impedance voltage levels) and stabilize digital lines without external components:
  - **No Pull-up/Pull-down (`00`):** Floating mode, typically used for active Push-Pull outputs or analog inputs.
  - **Pull-up (`01`):** Weak internal resistor tied to VDD (prevents floating LOW).
  - **Pull-down (`10`):** Weak internal resistor tied to GND (prevents floating HIGH).
- **DRY Refactoring in Bare-Metal Drivers:** Realized that `MODER` and `PUPDR` share identical 2-bit field mapping per pin (`pin * 2UL`). Creating a generic pointer-based utility (`GPIO_Set2BitField`) eliminated code duplication across multi-bit register modifiers.
- **Generic Register Pointer Operations:** Mastered passing volatile register addresses (`volatile uint32_t *reg`) into static inline helper functions to modify arbitrary peripheral registers dynamically.

### Technical Implementation

- **Driver Architecture Refactoring (`stm32f401_registers.h`):**
  - Mapped `GPIOA_PUPDR` register using offset `0x0CUL`.
  - Defined `GPIO_PUPDR_t` enum containing `GPIO_PUPDR_NOPULLUPDOWN` (`0x00`), `GPIO_PUPDR_PULLUP` (`0x01`), `GPIO_PUPDR_PULLDOWN` (`0x02`), and `GPIO_PUPDR_RESERVED` (`0x03`).
  - Implemented generic `GPIO_Set2BitField(volatile uint32_t *reg, uint8_t pin, uint32_t value)` function to perform 2-bit mask clearing and value shifting.
  - Refactored `GPIOA_SetMode` and added `GPIOA_SetPullUpDown` to utilize `GPIO_Set2BitField`.
- **Application Layer (`main.c`):**
  - Explicitly configured PA5 as Output Push-Pull with No Pull-Up/Pull-Down (`GPIO_PUPDR_NOPULLUPDOWN`).
  - Toggled PA5 in a `while(1)` super-loop.

### Hardware/Registers Involved

- **Registers:**
  - `RCC_AHB1ENR` (Offset `0x30`): Peripheral clock enable register.
  - `GPIOA_MODER` (Offset `0x00`): Configured mode via `GPIO_Set2BitField`.
  - `GPIOA_OTYPER` (Offset `0x04`): Configured output driver type.
  - `GPIOA_PUPDR` (Offset `0x0C`): Configured internal pull resistors via `GPIO_Set2BitField`.
  - `GPIOA_ODR` (Offset `0x14`): Output state register.

### Conclusion
Day 006 introduced internal resistor configuration while significantly improving driver design elegance. Abstracting common 2-bit field operations into `GPIO_Set2BitField()` made the bare-metal driver cleaner, less error-prone, and ready for future peripheral expansions.