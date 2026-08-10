# Day 030: SysTick Setup & Core Timer Registers (ARM Cortex-M)

### Objective
Initialize the ARM Cortex-M SysTick internal timer to generate periodic 1 ms time bases at the register level. Map the `SYSTICK_BASE` address and configure the core registers (`CTRL`, `LOAD`, `VAL`) to act as the primary system heartbeat, establishing the foundation for future software debouncing and precise `delay_ms()` implementations.

---

### Key Technical Concepts

- **SysTick vs. Peripheral Timers (TIM2/TIM3):**
  - **SysTick:** A 24-bit down-counter built directly into the ARM Cortex-M core. It is the standard "system heartbeat" used universally for RTOS ticks and basic time-keeping across all Cortex-M chips.
  - **Peripheral Timers:** Complex hardware blocks (e.g., TIM2) created by STMicroelectronics, attached to the APB bus, designed for advanced hardware control (PWM, input capture, encoder reading).
- **Core SysTick Registers:**
  - `SYST_CSR` (`CTRL`): Control and Status Register. Configures the clock source, enables interrupt generation (`TICKINT`), and starts the counter.
  - `SYST_RVR` (`LOAD`): Reload Value Register. Holds the starting value for the countdown.
  - `SYST_CVR` (`VAL`): Current Value Register. Shows the active countdown value; writing to it clears the counter and resets the `COUNTFLAG`.
- **Calculating Reload Value (1 ms Interval):**
  - Using the default HSI clock at 16 MHz (16.000.000 cycles per second).
  - Target: 1 ms interval = 16.000 cycles.
  - `LOAD` Value = Target Cycles - 1 = 15.999.

---

### Register Mapping & Bit Definitions

| Register | Address / Offset | Bit Mask / Value | Functional Impact |
| :--- | :--- | :--- | :--- |
| `SYSTICK_BASE` | `0xE000E010UL` | - | Memory mapped Base Address in Cortex-M System Control Space |
| `SysTick->CTRL` | `0x00` | `SYSTICK_CTRL_CLKSOURCE` (Bit 2) | Sets clock source to Processor Clock (HSI 16 MHz) |
| `SysTick->CTRL` | `0x00` | `SYSTICK_CTRL_TICKINT` (Bit 1) | Enables SysTick Exception/Interrupt request |
| `SysTick->CTRL` | `0x00` | `SYSTICK_CTRL_ENABLE` (Bit 0) | Enables the SysTick down-counter |
| `SysTick->LOAD` | `0x04` | `15999` | Sets the reload countdown boundary (16.000 - 1) |
| `SysTick->VAL` | `0x08` | `0` | Clears current counter and pending flags before start |

---

### Implementation Details

1. **Memory Map Abstraction (`Inc/stm32f401_registers.h`):**
   - Defined `SYSTICK_BASE` at `0xE000E010UL`.
   - Created `SysTick_TypeDef` struct mapping `CTRL`, `LOAD`, `VAL`, and `CALIB` offsets.
   - Defined core bitmasks for the `CTRL` register.

2. **SysTick Driver Initialization (`Src/stm32f401_systick.c`):**
   - Created `SysTick_Init(uint32_t ticks)` sequence:
     1. Disabled `CTRL` to prevent accidental triggers during setup.
     2. Loaded the calculated `ticks - 1` value into `LOAD`.
     3. Cleared the `VAL` register to force a clean start.
     4. Activated `CLKSOURCE`, `TICKINT`, and `ENABLE` bits simultaneously.

3. **System Integration (`Src/main.c`):**
   - Imported `stm32f401_systick.h` and executed `SysTick_Init(16000)` prior to the main infinite loop.
   - Preserved the existing asynchronous EXTI event logic for PC13 and PA5 LED.

---

### Hardware Verification & Results
- **Seamless Integration:** The firmware compiles successfully and executes the EXTI asynchronous flag logic perfectly, proving the new SysTick initialization does not block or disrupt existing threads.
- **Background Execution:** Although a dedicated ISR is not yet implemented, the SysTick hardware is actively generating internal 1 ms exception requests to the NVIC in the background, preparing the system for Day 031's handler integration.

---

### Conclusion
Day 030 successfully decoupled the concept of standard Cortex-M core timekeeping from vendor-specific peripheral timers. Configuring the SysTick `CTRL`, `LOAD`, and `VAL` registers establishes a highly portable, precise 1 ms hardware heartbeat required for professional firmware timing operations.