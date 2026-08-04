# Day 025: UART Stress Testing & High-Throughput System Stability

### Objective
Implement and execute a continuous high-throughput data transmission stress test over UART using the established bare-metal peripheral architecture. Focus on evaluating transmission stability, ring buffer capacity under continuous load, watchdog resilience during heavy serial traffic, and system endurance under intense CPU/UART interaction.

---

### Key Learnings

- **High-Throughput Stress Testing:** Pushed the buffered UART communication framework to its limits by continuously flooding the transmission channel with high-frequency `"Hello World"` messages.
- **Watchdog Harmonization:** Validated that the Independent Watchdog (IWDG) maintains steady synchronization and does not trigger false system resets during intense, uninterrupted serial communication loops.
- **Ring Buffer Endurance:** Observed how asynchronous buffer management handles heavy continuous payloads without dropping bytes or causing memory overflow.
- **Visual Heartbeat Verification:** Maintained real-time physical feedback via the status LED toggle (~500ms intervals) to confirm that the main application thread remains responsive and unblocked during continuous streaming.

---

### Technical Implementation

- **API Interface & Register Mapping:**
  Utilized the existing robust abstraction stack including direct register definitions, GPIO configuration for heartbeat output, and the `RingBuffer_t` data structure mapped to USART2.

- **Stress Test Implementation (`Src/main.c`):**
  - Configured USART2 at `115200` baud linked to the ST-LINK Virtual COM Port.
  - Initialized non-blocking ring buffer structures (`tx_buffer`) for safe data queuing.
  - Implemented an aggressive continuous-transmission loop designed to stream output data rapidly while regularly servicing the hardware watchdog (`IWDG_Feed()`).

- **System Integration & Verification Suite:**
  - Integrated the ~5-second timeout hardware watchdog within the heavy transmission loop to ensure system safety.
  - Retained the visual heartbeat LED indicator to monitor core execution health dynamically during execution.

---

### Hardware/Registers Involved

- **Peripherals & Core Drivers:**
  - `USART2` (Universal Synchronous Asynchronous Receiver-Transmitter): Configured for continuous serial data output via ST-LINK VCP.
  - `IWDG` (Independent Watchdog): Set with a ~5-second timeout threshold (`PR = /128`, `RLR = 1250`) to monitor loop health under heavy software load.
  - `GPIOA` (PA5 LED Output): Utilized for real-time visual system heartbeat tracking.

---

### Verification & Hardware Results

- **Continuous Transmission Test:**
  - The serial terminal successfully receives a continuous stream of `"Hello World! Stress test ongoing..."` messages without locking up or dropping frames.
- **System Stability Under Load:**
  - The hardware watchdog remains satisfied due to regular periodic servicing inside the loop, preventing unexpected MCU resets.
- **Responsive Heartbeat:**
  - The status LED continues to blink steadily at ~500ms intervals, confirming that the high-throughput UART streaming does not starve the core execution loop.

---

### Conclusion
Day 025 successfully validates the robustness and reliability of the bare-metal firmware architecture under heavy stress conditions. By combining continuous high-frequency UART data streaming with active watchdog monitoring and buffered transmission management, the system proves its capability to handle intensive operational loads dependably.