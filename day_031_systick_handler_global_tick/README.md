# Day 031: SysTick Exception Handler & Non-Blocking Delay Integration

### Objective
Implement the `SysTick_Handler` Exception Service Routine (ISR) and build a global millisecond tick counter (`g_ticks`). Integrate the SysTick driver with the existing EXTI interrupt system to demonstrate concurrent, non-blocking timekeeping and asynchronous button event handling in `main.c`.

---

### Key Technical Concepts

- **SysTick Exception Handler Routing:**
  - **Core Exception Vector:** `SysTick_Handler` is mapped directly to position 15 (offset `0x003C`) in the ARM Cortex-M Vector Table defined in `startup_stm32f401retx.s`.
  - **No NVIC Configuration Needed:** As an internal Cortex-M core exception, SysTick bypasses peripheral IRQ lines (like EXTI IRQ 40) and is managed directly via the `SysTick->CTRL` register.
- **Volatile Global Tick Tracking:**
  - `g_ticks` is updated asynchronously inside the 1 ms ISR. Declaring it `volatile` prevents compiler optimizations from caching the value in CPU registers during main loop comparisons.
- **Non-Blocking Delay Pattern:**
  - Evaluates elapsed time using elapsed tick arithmetic `(SysTick_GetTick() - last_heartbeat) >= interval` instead of CPU-blocking delay loops, enabling concurrent multitasking.
- **Interrupt Handling Best Practice:**
  - Cleared peripheral pending flags at the very start of peripheral ISRs (`EXTI15_10_IRQHandler`) to prevent duplicate entry bugs caused by peripheral bus write latency.

---

### Register Mapping & Bit Definitions

| Register / Symbol | Address / Offset | Bit Mask / Value | Functional Impact |
| :--- | :--- | :--- | :--- |
| `SysTick_Handler` | Vector Table (`0x003C`) | Label Pointer | Core Exception ISR called automatically every 1 ms |
| `EXTI->PR` | `0x14` | `EXTI_PR_PR13` (Bit 13) | Pending flag cleared first in ISR to prevent double triggers |
| `NVIC->ISER[1]` | `0xE000E100` | `1UL << 8` (IRQ 40) | Enables EXTI Line[15:10] peripheral interrupt line |
| `SysTick->CTRL` | `0x00` | `SYSTICK_CTRL_TICKINT` | Enables SysTick exception request to CPU core |

---

### Implementation Details

1. **Global Tick Base & ISR (`Src/stm32f401_systick.c`):**
   - Defined `volatile uint32_t g_ticks` and implemented `SysTick_Handler()` to increment `g_ticks` every 1 ms.
   - Provided `SysTick_GetTick()` helper function for atomic tick retrieval.

2. **Refactored EXTI Driver (`Src/stm32f401_exti.c`):**
   - Updated `EXTI15_10_IRQHandler()` to clear the pending flag (`EXTI->PR = EXTI_PR_PR13`) prior to updating `g_btn_event_flag`, resolving bus propagation latency issues.

3. **Non-Blocking Multitasking (`Src/main.c`):**
   - Configured `SysTick_Init(16000)` for 1 ms periodic ticks at 16 MHz HSI.
   - Implemented a 500 ms non-blocking heartbeat LED toggle alongside instantaneous EXTI push-button event handling in the `while(1)` super-loop.

---

### Hardware Verification & Results
- **Periodic Heartbeat:** The onboard LED toggles automatically every 500 ms based on `SysTick_GetTick()`, confirming `SysTick_Handler` executes continuously in the background.
- **Asynchronous Responsiveness:** Pressing the user push-button (PC13) triggers the EXTI ISR and toggles the LED immediately, proving concurrent event handling without timing degradation or blocking calls.

---

### Conclusion
Day 031 successfully established a working system heartbeat via the `SysTick_Handler` ISR. Transitioning to a non-blocking `g_ticks` architecture provides the fundamental building block for software debouncing, precise timekeeping, and RTOS task schedulers in production-grade firmware.