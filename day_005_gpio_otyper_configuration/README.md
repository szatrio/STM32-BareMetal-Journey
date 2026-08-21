# Day 005: GPIO Output Type Configuration (OTYPER Driver Extension)

### Objective
Extend the custom register driver to configure GPIO output driver types—Push-Pull vs Open-Drain—by implementing `GPIOA_OTYPER` register access, C enumerations (`enum`), and defensive pin boundary checking.

### Key Learnings

- **Push-Pull vs Open-Drain Hardware Mechanics:** Understood the underlying transistor configurations in STM32 GPIO ports:
  - **Push-Pull (`0`):** Actively drives the pin HIGH (VCC) via P-MOSFET and LOW (GND) via N-MOSFET. Ideal for direct LED driving and standard digital signaling.
  - **Open-Drain (`1`):** Actively drives the pin LOW via N-MOSFET, but leaves HIGH state floating (high-impedance). Requires an external pull-up resistor to pull the line HIGH. Essential for bus communication protocols like I2C.
- **C Enumerations for Type Safety:** Mastered using strongly-typed C enumerations (`GPIO_OType_t`) to eliminate invalid configuration inputs at compile time.
- **Defensive Programming in Drivers:** Learned to implement boundary condition checks (`if (pin > 15) return;`) to prevent out-of-bounds bit shifts from corrupting memory or adjacent register states.

### Technical Implementation

- **Driver Architecture Extension (`stm32f401_registers.h`):**
  - Mapped `GPIOA_OTYPER` register using offset `0x04UL`.
  - Defined `GPIO_OType_t` enum for `GPIO_OTYPE_PUSHPULL` (`0x00`) and `GPIO_OTYPE_OPENDRAIN` (`0x01`).
  - Implemented `GPIOA_SetOutputOtype(uint8_t pin, uint32_t type)` using single-bit mask clearing `&= ~(1UL << pin)` followed by bitwise assignment `|= ((type & 0x01UL) << pin)`.
  - Added boundary validation (`pin > 15`) to both `GPIOA_TogglePin` and `GPIOA_SetOutputOtype`.
- **Application Layer (`main.c`):**
  - Explicitly initialized PA5 as Output Push-Pull via `GPIOA_SetOutputOtype(LED_PIN, GPIO_OTYPE_PUSHPULL)`.
  - Toggled the LED in a super-loop using parameterized pin macro `LED_PIN`.

### Hardware/Registers Involved

- **Registers:**
  - `RCC_AHB1ENR` (Offset `0x30`): Enabled clock for GPIOA.
  - `GPIOA_MODER` (Offset `0x00`): Configured PA5 as General Purpose Output mode (`01`).
  - `GPIOA_OTYPER` (Offset `0x04`): Configured output driver transistor state (Bit 5).
  - `GPIOA_ODR` (Offset `0x14`): Toggled output pin state.

### Conclusion
Day 005 expanded the bare-metal GPIO driver to control output transistor behavior. Implementing type-safe enums and defensive bounds checking ensures driver reliability as peripheral complexity grows.