# Day 033: Non-Blocking Co-operative SysTick Scheduler & Timeout Guard

### Objective
Implement a non-blocking, time-sliced co-operative task scheduler in `main.c` using SysTick timestamping and rollover-safe delta checks. Transition the architecture from rigid, blocking `SysTick_DelayMs()` routines to concurrent task execution, handling asynchronous interrupts, periodic background tasks, and safety timeout guards simultaneously without CPU starvation.

---

### Key Technical Concepts

- **Non-Blocking Time-Slicing Scheduler:**
  - Instead of stalling CPU execution inside blocking delay loops, the main super-loop continually polls individual tasks. Execution of periodic tasks is gated by timestamp comparisons (`Has_Timeout_Elapsed()`), releasing CPU cycles immediately when interval criteria are not met.
- **Rollover-Safe Tick Arithmetic:**
  - Standard time comparison `(current - start >= interval)` using unsigned 32-bit integers guarantees exact evaluation even across `g_ticks` overflow (~49.7 days), leveraging standard 2's complement arithmetic rules in C.
- **Asynchronous & Periodic Task Coexistence:**
  - High-priority event flags (`g_btn_event_flag` from EXTI) and low-priority periodic tasks (Heartbeat LED at 500 ms, UART Telemetry at 1000 ms) run in a unified super-loop with near-zero latency degradation.
- **Non-Blocking Timeout Guard Pattern:**
  - Replaces dangerous infinite blocking wait states (`while(!flag)`) with deadline-driven timeout monitoring. If an external response fails to arrive within 2000 ms, the guard safely aborts waiting state and reports a system warning without freezing concurrent tasks.

---

### Task Breakdown Architecture

| Task ID | Type | Trigger / Interval | Primary Action | Hardware / Subsystem |
| :--- | :--- | :--- | :--- | :--- |
| **TASK A** | Asynchronous Event | EXTI Button Press (PC13) | Clears event flag, toggles LED, resets Timeout Guard | GPIOA (PA5), EXTI13, UART2 |
| **TASK B** | Periodic Scheduled | Every 500 ms | Toggles Heartbeat LED to signal core vitality | GPIOA (PA5), SysTick Timer |
| **TASK C** | Periodic Scheduled | Every 1000 ms | Formats and transmits system uptime log | USART2, SysTick Timer |
| **TASK D** | Safety Mechanism | 2000 ms Deadline | Aborts pending state if response times out | Software State Flag, UART2 |

---

### Implementation Details

1. **Inline Non-Blocking Helper (`Src/main.c`):**
   - Implemented `Has_Timeout_Elapsed()` as a `static inline` helper in application space to evaluate elapsed time without call-stack overhead:
     ```c
     static inline uint8_t Has_Timeout_Elapsed(uint32_t start_tick, uint32_t interval_ms) {
         return ((SysTick_GetTick() - start_tick) >= interval_ms);
     }
     ```

2. **State & Timestamp Tracking (`Src/main.c`):**
   - Established independent timestamp markers (`last_heartbeat_tick`, `last_telemetry_tick`, `process_start_tick`) and state flags (`is_waiting_response`) inside `main()`.

3. **Super-Loop Execution Flow (`Src/main.c`):**
   - Configured non-blocking evaluation blocks for Tasks A, B, C, and D inside `while(1)`. Replaced all blocking delay calls with hardware tick checks.

---

### Hardware Verification & Results

- **Concurrent Task Execution:** Confirmed via USART2 terminal (115200 baud) that UART uptime logs (1000 ms) and LED toggles (500 ms) run continuously without pause.
- **Asynchronous Event Responsiveness:** Pressing PC13 button triggers `[EVENT]` instantly with zero latency, interrupting neither the background heartbeat LED nor telemetry logging.
- **Timeout Guard Safety Trigger:** Verified that allowing 2000 ms to elapse without pressing PC13 successfully triggers `[WARN/ERROR] Timeout Guard Triggered!` while system uptime counters and periodic tasks continue executing smoothly.

---

### Conclusion
Day 033 successfully transformed the application architecture from single-threaded blocking execution into a modular, non-blocking co-operative scheduler. This event-driven pattern serves as the structural prerequisite for Finite State Machines (FSM) and future Real-Time Operating System (RTOS) task management.