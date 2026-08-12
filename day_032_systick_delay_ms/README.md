# Day 032: High-Precision SysTick Blocking Delay Driver

### Objective
Implement `SysTick_DelayMs()` to establish a hardware-backed, precise blocking delay mechanism in the `stm32f401_systick` driver. Replace software dummy loops with hardware clock-referenced timekeeping, proving immunity against compiler optimizations while encapsulating millisecond time tracking inside driver layer APIs.

---

### Key Technical Concepts

- **Hardware-Backed Precision Blocking:**
  - Unlike software loops (`for`/`while`), `SysTick_DelayMs()` relies on core clock ticks generated independently by hardware. Time duration remains 100% accurate regardless of CPU clock frequency shifts or compiler optimization levels.
- **Compiler Optimization Immunity:**
  - Standard `for` loop delays are routinely removed or shortened by GCC `-O2`/`-O3` optimization flags. Querying `SysTick_GetTick()` inside a `while` check prevents loop elimination because `g_systick_ticks` is updated asynchronously by the hardware ISR.
- **Encapsulated Timekeeping API:**
  - Abstracting elapsed-time arithmetic into a reusable driver function (`SysTick_DelayMs`) removes timestamp state management (`last_heartbeat`) from `main.c`, reducing boilerplate code in initializations or sequential hardware startup routines.
- **ISR Preemption During Blocking:**
  - While `SysTick_DelayMs()` blocks the main execution thread in `main()`, hardware interrupts (such as `EXTI15_10_IRQHandler` and `SysTick_Handler`) continue to fire and execute in the background with zero latency penalty.

---

### Comparison of Delay Approaches

| Delay Methodology | Time Accuracy | CPU Behavior in main() | Compiler Optimization Safety | Primary Use Case |
| :--- | :--- | :--- | :--- | :--- |
| **Imprecise Blocking** (`for` dummy loop) | Unpredictable (CPU speed dependent) | Trapped in dummy loop | **Vulnerable** (frequently removed by `-O2`/`-O3`) | Quick temporary tests / prototyping only |
| **Precise Blocking** (`SysTick_DelayMs`) | **100% Exact** (Hardware clock referenced) | Trapped in driver query loop | **100% Immune** (backed by hardware ISR) | Peripheral startup waits, hardware resets |
| **Non-Blocking Guard** (`if(now - start >= ms)`) | **100% Exact** (Hardware clock referenced) | **Free** to run other tasks | **100% Immune** (backed by hardware ISR) | Main event loop multitasking & scheduling |

---

### Implementation Details

1. **Driver Interface Extension (`Inc/stm32f401_systick.h`):**
   - Declared `void SysTick_DelayMs(uint32_t ms);` as part of the public driver interface.

2. **Hardware Delay Implementation (`Src/stm32f401_systick.c`):**
   - Implemented `SysTick_DelayMs()` using rollover-safe unsigned subtraction:
     ```c
     void SysTick_DelayMs(uint32_t ms) {
         uint32_t start_time = SysTick_GetTick();
         while ((SysTick_GetTick() - start_time) < ms) {
             // CPU waits for hardware tick threshold
         }
     }
     ```

3. **Super-Loop Refactoring (`Src/main.c`):**
   - Initialized SysTick for 1 ms periodic interrupts (`16000` cycles at 16 MHz HSI).
   - Removed manual timestamp variables (`last_heartbeat`) and simplified the main loop to execute deterministic 500 ms LED toggles via `SysTick_DelayMs(500)`.

---

### Hardware Verification & Results

- **Deterministic Timing:** Verified with hardware flash that PA5 LED toggles at exact 500 ms intervals (1 Hz duty cycle) matching real-world time.
- **Interrupt Preemption:** Confirmed that pressing user button PC13 triggers `EXTI15_10_IRQHandler()` instantly even while CPU is inside `SysTick_DelayMs()`, proving core exception handling functions normally during blocking calls.
- **Build Output:** Compiled cleanly with zero errors (`0 errors, 0 warnings` after removing unused variables).

---

### Conclusion
Day 032 established a production-grade, hardware-backed blocking delay utility. Replacing unpredictable soft loops with `SysTick_DelayMs()` guarantees timing stability across build configurations, laying a clean foundation for upcoming non-blocking multitasking schedulers.