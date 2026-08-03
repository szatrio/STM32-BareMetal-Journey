# Day 024: Hardware Watchdog Timer (IWDG) Integration & System Resilience

### Objective
Integrate, configure, and validate a hardware-level Independent Watchdog Timer (IWDG) into the bare-metal STM32F401 firmware architecture. Focus on mitigating system lockups, implementing safe register initialization without execution freezes, handling watchdog reset recovery flags, and establishing fault injection testing loops to ensure robust production-grade reliability.

---

### Key Learnings

- **Watchdog Fundamentals:** Understood the mechanics of the Independent Watchdog (IWDG), which runs on an independent Low-Speed Internal (LSI ~32kHz) clock to guarantee system recovery even if the primary main system clock fails.
- **Initialization Trap Avoidance:** Discovered and resolved a common bare-metal pitfall where kaku (rigid) status flag checking (`while(IWDG->SR & ...)` loops) freezes execution due to LSI clock synchronization latency.
- **Safe Timeout Configuration:** Configured the IWDG prescaler (`/128`) and Reload Register (`RLR = 1250`) to establish a generous **~5-second timeout window**, accommodating normal application execution and software delays without false triggers.
- **Fault-Injection & Self-Recovery:** Developed a controlled fault-testing loop via GPIO input triggers to simulate system freezes, validating that the hardware watchdog automatically triggers a system reset and recovery mechanism.

---

### Technical Implementation

- **API Interface & Register Mapping:**
  Utilized direct register manipulation for IWDG control keys (`IWDG_KEY_ACCESS`, `IWDG_KEY_RELOAD`, `IWDG_KEY_ENABLE`), prescaler configuration (`IWDG_PR`), and reload management (`IWDG_RLR`).

- **Watchdog Driver Implementation (`Src/main.c` / Driver Layer):**
  - Implemented `IWDG_Init()` using a clean, non-blocking sequence to unlock registers, set scale dividers, load count values, and start the timer without deadlocking on status flag loops.
  - Implemented `IWDG_Feed()` to periodically reload the watchdog counter inside the main application loop.

- **System Integration & Verification Suite:**
  - Structured a dual-state verification setup combining visual heartbeat indicators (LED toggling) and serial output logging.
  - Added a controlled fault injection path triggered by button input (`PC13`), intentionally freezing the core execution thread to test hardware reset recovery.

---

### Hardware/Registers Involved

- **Peripherals & Core Drivers:**
  - `IWDG` (Independent Watchdog): Operates off the internal 32 kHz LSI clock.
  - `IWDG->KR` (Key Register): Used for unlocking write protection (`0x5555`), reloading (`0xAAAA`), and starting the peripheral (`0xCCCC`).
  - `IWDG->PR` (Prescaler Register): Configured with `/128` divider to slow down clock ticks.
  - `IWDG->RLR` (Reload Register): Set to `1250` for an extended ~5-second watchdog timeout threshold.
  - `GPIOC` (PC13 Button Input) & `GPIOA` (PA5 LED Output): Used for manual state verification and fault triggering.

---

### Verification & Hardware Results

- **Normal Operation Test:**
  - System boots up cleanly, printing initialization banners via `USART2`.
  - Periodic `IWDG_Feed()` calls inside the main loop prevent hardware timeouts, keeping the system stable while the heartbeat LED blinks consistently at ~500ms intervals.
- **Button-Driven Debounced Event Test:**
  - Integrated an 8-bit Shift Register history filter (`Button_Update_FSM`) to cleanly parse button presses without mechanical bounce interference.
- **Watchdog Fault-Injection & Recovery Test:**
  - Pressing the button stops `IWDG_Feed()` and forces an intentional infinite lockup loop.
  - Hardware IWDG times out after ~5 seconds, executing an automatic hardware reset of the STM32F401 MCU and gracefully restarting execution back to the boot banner.

---

### Conclusion
Day 024 successfully elevates the firmware architecture from basic functional execution to resilient, fault-tolerant production-grade engineering. By implementing a safe IWDG configuration and mastering real-time watchdog refreshing alongside FSM-based input handling, the system is fully equipped to recover autonomously from unexpected software freezes.