# Day 029: Non-Blocking Event-Driven EXTI Interrupt Architecture

### Objective
Transition from synchronous ISR execution to an asynchronous, event-driven architecture using bare-metal STM32F4 C. Refactor the EXTI pipeline so that the Interrupt Service Routine (`EXTI15_10_IRQHandler`) acts strictly as a lightweight event producer, deferring action logic execution (toggling PA5 LED) to the main thread (`main()`) consumer loop via a `volatile` flag.

---

### Key Technical Concepts

- **Producer-Consumer Pattern in Embedded Firmware:**
  - **Producer (ISR):** Runs in Handler Mode. Captures hardware button press (PC13 Falling Edge), sets `g_btn_event_flag = 1`, clears `EXTI->PR`, and immediately returns.
  - **Consumer (`main` loop):** Runs in Thread Mode. Polls `g_btn_event_flag`, clears it (`0`), and performs the actual peripheral operation (`GPIOA->ODR ^= LED_PIN`).
- **Super-Fast / Minimalist ISR:**
  - Minimizes CPU time spent inside Handler Mode, ensuring deterministic interrupt latency and avoiding preemption/blocking of other equal-or-lower priority vectors.
- **Role of `volatile` Keyword:**
  - Declaring `extern volatile uint8_t g_btn_event_flag;` prevents compiler optimization from caching the variable in CPU registers during the `while(1)` polling loop, guaranteeing real-time reads directly from SRAM.

---

### System Execution Architecture

```
                       [ PC13 Active-LOW Press ]
                                   │
                                   ▼
                       [ EXTI Line 13 Hardware ]
                                   │
                                   ▼
                       [ NVIC Interrupt IRQ 40 ]
                                   │
                                   ▼
  ┌─────────────────────────────────────────────────────────────────┐
  │ HANDLER MODE (ISR)                                              │
  │ EXTI15_10_IRQHandler()                                          │
  │  ├── 1. Detect PR13 active                                      │
  │  ├── 2. Set volatile g_btn_event_flag = 1   <-- Producer        │
  │  └── 3. Clear EXTI->PR pending flag                             │
  └─────────────────────────────────────────────────────────────────┘
                                   │ (Fast Exit / Nanoseconds)
                                   ▼
  ┌─────────────────────────────────────────────────────────────────┐
  │ THREAD MODE (Main Application Loop)                             │
  │ while (1)                                                       │
  │  └── if (g_btn_event_flag)                  <-- Consumer        │
  │       ├── 1. Reset g_btn_event_flag = 0                         │
  │       └── 2. Execute PA5 Toggle (GPIOA->ODR ^= LED_PIN)         │
  └─────────────────────────────────────────────────────────────────┘
```

---

### Implementation Details

1. **Header Abstraction (`Inc/stm32f401_exti.h`):**
   - Declared global event flag: `extern volatile uint8_t g_btn_event_flag;`.
   - Included `<stdint.h>` for standard integer type definitions.

2. **EXTI Driver (`Src/stm32f401_exti.c`):**
   - Defined `volatile uint8_t g_btn_event_flag = 0;`.
   - Updated `EXTI15_10_IRQHandler()` to remove direct GPIO ODR manipulation and replaced it with flag assertions.

3. **Main Thread Logic (`Src/main.c`):**
   - Polled `g_btn_event_flag` inside `while(1)`.
   - Executed toggle action and reset flag atomically within the main execution loop.

---

### Hardware Verification & Results

- **ISR Execution Time:** Extremely brief, avoiding long delay stalls or hardware blocking in Handler Mode.
- **Asynchronous Flag Capture:** Pressing PC13 reliably sets the flag in background interrupt execution, and the main thread captures and processes the event instantly.
- **No Race Condition:** Flag reset (`g_btn_event_flag = 0`) occurs immediately prior to action execution, preventing double-triggering or missed event handling.

---

### Conclusion
Day 029 establishes the foundational event-driven paradigm used in professional embedded firmware and RTOS application design. Decoupling hardware event generation from application execution ensures maximum responsiveness, lower latency, and clean thread-safe state management.