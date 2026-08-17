# Day 001: Direct Register Access (Bare-Metal LED Blink)

### Objective
Implement a bare-metal C program to blink the onboard LED (PA5) on an STM32 microcontroller by directly manipulating hardware register memory addresses, bypassing all HAL libraries and standard peripheral drivers.

### Key Learnings

- **Direct Memory Access via Volatile Pointers:** Learned how to cast raw memory addresses to `volatile uint32_t *` pointers. Using `volatile` ensures the compiler writes directly to hardware addresses without optimizing out repeated access in delay loops or register writes.
- **Peripheral Clock Gating:** Understood that ARM Cortex-M peripherals are clock-gated by default to save power. Enabling the peripheral clock in `RCC_AHB1ENR` is mandatory before accessing any GPIO register.
- **Bitwise Register Manipulation:** Mastered standard Read-Modify-Write bit operations: clearing specific configuration bits with AND-NOT masks (`&= ~`), setting bits with OR (`|=`), and toggling state using XOR (`^=`).

### Technical Implementation

- **Clock Setup:** Enabled GPIOA peripheral clock by setting Bit 0 in `RCC_AHB1ENR`.
- **Pin Configuration (PA5):** Masked out bits 10 and 11 in `GPIOA_MODER` and set bit 10 to `1` to configure PA5 as General Purpose Output mode (`01`).
- **Super-Loop Logic:** Implemented an infinite `while(1)` loop that toggles Bit 5 of `GPIOA_ODR` via XOR and uses a simple `for` loop as a software delay.

### Hardware/Registers Involved

- **Registers:**
    - `RCC_AHB1ENR` (`0x40023830`): Enables clock for GPIOA (Bit 0).
    - `GPIOA_MODER` (`0x40020000`): Sets pin mode for PA5 (Bits 11:10).
    - `GPIOA_ODR` (`0x40020014`): Controls output state of PA5 (Bit 5).

### Conclusion
Day 001 established the core foundation of bare-metal micro-controller programming: memory-mapped I/O. Understanding how raw hardware addresses translate to register structures provides complete control over hardware execution and CPU instructions.