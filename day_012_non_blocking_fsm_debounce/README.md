# Day 012: Non-Blocking Debouncing via Finite State Machine (FSM)

### Objective
Upgrade the button-debouncing architecture from a blocking, delay-based polling method to a non-blocking Finite State Machine (FSM). This eliminates CPU stalling, allowing the microcontroller to handle user input asynchronously while maintaining maximum responsiveness and system efficiency.

### Key Learnings

- **Non-Blocking Architecture:** Transitioned away from blocking CPU delays (`HAL_Delay` or heavy busy-wait loops) during debounce checks, enabling the microcontroller to execute other background tasks freely.
- **Finite State Machine (FSM) Design:** Implemented a structured 4-state machine (`BUTTON_RELEASED`, `BUTTON_DEBOUNCING_DOWN`, `BUTTON_PRESSED`, `BUTTON_DEBOUNCING_UP`) to handle mechanical signal transitions reliably.
- **Tick-Based Sampling:** Utilized a counter threshold (`DEBOUNCE_THRESHOLD_TICKS`) driven by continuous, rapid main-loop polling rather than static wait times.
- **Code Decoupling:** Separated low-level hardware pin reading from state transition logic, establishing a cleaner and more professional firmware architecture.

### Technical Implementation

- **FSM Routine:** Created `Button_Update_FSM()` to process physical pin levels state-by-state:
    1. **`BUTTON_RELEASED`**: Monitors for the initial active-low press.
    2. **`BUTTON_DEBOUNCING_DOWN`**: Increments a counter across rapid polling cycles to bypass mechanical noise.
    3. **`BUTTON_PRESSED`**: Confirms a valid press event (returns `true` once per click) and transitions to wait for release.
    4. **`BUTTON_DEBOUNCING_UP`**: Validates the release transition before resetting to the idle state.
- **Standardized Types:** Integrated `<stdbool.h>` to maintain clean `bool`, `true`, and `false` syntax across the firmware modules.
- **Main Loop Integration:** Polled the FSM continuously with a minimal non-blocking interval (~2ms), ensuring zero CPU starvation.

### Hardware/Registers Involved

- **Registers:**
    - `GPIOA_ODR`: Bit-toggling for the PA5 LED upon a validated click event.
    - `GPIOC_IDR`: Continuously polled at high speed to inspect the physical state of the PC13 button.
    - `GPIOA_PUPDR`: Configured with an internal pull-up resistor to maintain a stable baseline signal level.

### Conclusion
Day 12 marks a critical evolution toward professional embedded systems development. By replacing blocking delays with a robust, non-blocking Finite State Machine, the firmware is now event-driven and scalable—laying down the exact architectural principles needed when transitioning into RTOS task management in the near future.