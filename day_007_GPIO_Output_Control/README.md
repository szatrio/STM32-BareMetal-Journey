# Day 007: Explicit GPIO Output Control (State-Based Pin Write API)

### Objective
Implement explicit HIGH/LOW output pin writing APIs (`GPIOA_WritePin`) to control pin state deterministically instead of relying solely on toggling operations, enabling precise pattern control like asymmetric blink timing.

### Key Learnings

- **Explicit Output State vs Toggling:** Understood the functional difference between state inversion (`^=`) and explicit pin assignment (`|=` vs `&= ~`). Explicit state driving is required for state machines, bus protocols, and deterministic output sequences.
- **Bitwise Read-Modify-Write (RMW) Operations:**
  - **Set HIGH (`1`):** Applied single-bit OR mask `GPIOA_ODR |= (1UL << pin)`.
  - **Clear LOW (`0`):** Applied bitwise AND-NOT mask `GPIOA_ODR &= ~(1UL << pin)`.
- **Asymmetric Timing Patterns:** Demonstrated how controlling state explicitly allows flexible timing parameters (e.g., 1,000,000 loop delay for HIGH vs 500,000 loop delay for LOW) to create distinct duty cycles.

### Technical Implementation

- **Driver Output API Extension (`stm32f401_registers.h`):**
  - Implemented `GPIOA_WritePin(uint8_t pin, uint8_t state)` with defensive bounds checking (`validatePin`).
  - Evaluated the input boolean/uint8 `state` to route logic between setting the corresponding bit high or clearing it low in `GPIOA_ODR`.
- **Application Layer (`main.c`):**
  - Configured PA5 as Output Push-Pull with No Pull-Up/Pull-Down.
  - Constructed an asymmetric blinking loop: driving PA5 HIGH with a longer delay loop followed by driving PA5 LOW with a shorter delay loop using `GPIOA_WritePin`.

### Hardware/Registers Involved

- **Registers:**
  - `RCC_AHB1ENR` (Offset `0x30`): Enabled clock for GPIOA.
  - `GPIOA_MODER` (Offset `0x00`): Configured PA5 as General Purpose Output mode (`01`).
  - `GPIOA_OTYPER` (Offset `0x04`): Configured output driver type as Push-Pull.
  - `GPIOA_PUPDR` (Offset `0x0C`): Configured internal pull state as No Pull-Up/Pull-Down.
  - `GPIOA_ODR` (Offset `0x14`): Directly modified bit 5 to drive HIGH (`|=`) and LOW (`&= ~`).

### Conclusion
Day 007 completed the output-side GPIO API set. Having explicit pin write functions provides the foundation for driving complex digital components like relays, displays, and communication lines where exact logical states must be set on demand.