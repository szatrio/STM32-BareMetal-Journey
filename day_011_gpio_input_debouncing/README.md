# Day 011: Debouncing Logic & Toggle Switch Implementation

### Objective
Implement a robust button-toggle mechanism by integrating software debouncing. This ensures that a single physical press on the PC13 button results in exactly one state change (toggle) of the PA5 LED, eliminating signal noise.

### Key Learnings

- **Software Debouncing:** Learned that mechanical switches generate "noise" (bouncing) during the state transition. Implemented a simple delay-based polling method to verify the signal stability before confirming a state change.
- **Toggle Logic vs. Direct Write:** Utilized `GPIO_TogglePin` with an XOR (`^=`) operation to flip the output state, which is cleaner and more efficient than reading and then writing the `ODR` register.
- **Latch Prevention:** Implemented a "wait-for-release" loop (`while` loop for button state) to ensure that holding the button down doesn't result in rapid, unintended multiple toggles.
- **Modular Design:** Refactored the button logic into a reusable `get_button_state` function and separated application-level logic (`is_button_pressed_debounced`) from hardware drivers.

### Technical Implementation

- **Debouncing Routine:** Created a two-step validation:
    1. Detect initial press.
    2. Wait (~20ms) to bypass mechanical bouncing.
    3. Verify the signal is still low to confirm a valid press.
- **Toggle Mechanism:** Used the bitwise XOR operator on `GPIOA_ODR` to toggle PA5 efficiently.
- **Input Stabilization:** Configured the button with an internal Pull-up to prevent floating states, ensuring a deterministic signal level.

### Hardware/Registers Involved

- **Registers:**
    - `GPIOA_ODR`: Used for XOR bit-toggling of the LED state.
    - `GPIOC_IDR`: Polled repeatedly to detect physical signal changes.
    - `GPIOA_PUPDR`: Configured for Pull-up mode to stabilize the button input.

### Conclusion
Day 11 bridged the gap between raw hardware input and reliable software behavior. Without debouncing, the toggle functionality would have been erratic due to signal noise. By introducing a simple debounce routine and "wait-for-release" logic, the system now handles user input consistently. This is a critical step in building stable firmware where user interaction must be precise and predictable.