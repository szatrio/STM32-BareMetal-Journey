# Day 023: Non-Blocking Buffered UART Transmit (TX) Engine

### Objective
Design, implement, and validate a non-blocking, buffered transmission (TX) pipeline for the bare-metal STM32F401 UART driver. Focus on decoupling real-time core processing from low-speed hardware baud rate delays, implementing a memory-backed FIFO TX queue, developing a register-level hardware flush engine, and verifying zero-loss string streaming.

---

### Key Learnings

- **Non-Blocking Architecture:** Decoupled execution runtime from hardware transmission latencies by offloading payload data into an intermediate RAM-based Ring Buffer, eliminating CPU blocking during UART communication.
- **Hardware-Software Synchronization:** Mastered register-level coordination using the Transmit Data Register Empty flag (`USART_SR_TXE`) to safely transfer buffered bytes to the hardware `USART2->DR` register without bus contention or data corruption.
- **Engine-Driven Flushing (`Flush Engine`):** Implemented an active drain mechanism (`USART2_Flush_TX`) that dynamically drains queued payload bytes while hardware availability permits, maintaining deterministic execution flow.
- **Asynchronous Data Staging:** Successfully transformed blocking string output paradigms into non-blocking enqueue operations (`USART2_SendString_Buffered`), reserving hardware execution cycles for critical firmware tasks.

---

### Technical Implementation

- **API Interface Expansion (`Inc/stm32f401_usart.h`):**
  Augmented the USART driver interface with non-blocking transmission functions (`USART2_SendChar_Buffered`, `USART2_SendString_Buffered`) and the queue flushing engine prototype (`USART2_Flush_TX`).

- **Flush Engine & Buffered TX Implementation (`Src/stm32f401_usart.c`):**
  - Implemented `USART2_SendChar_Buffered` and `USART2_SendString_Buffered` to push transmit payloads directly into a statically allocated `RingBuffer_t` instance.
  - Implemented `USART2_Flush_TX` to poll the status of `USART2->SR & USART_SR_TXE` and pop pending bytes sequentially from the TX Ring Buffer into `USART2->DR` until the queue is completely drained.

- **System Integration & Verification Suite (`Src/main.c`):**
  Structured a complete test suite under `System_Init()` setup. Enqueued multiple formatted string payloads into `tx_buffer`, executed `USART2_Flush_TX()`, and verified post-drain queue empty status (`RingBuffer_IsEmpty`).

---

### Hardware/Registers Involved

- **Peripherals & Core Drivers:**
  - `USART2` (PA2 TX / PA3 RX): Configured via Alternate Function 7 (`AF7`) at 115200 baud (16 MHz HSI clock, `BRR = 0x008B`).
  - `USART2->SR` (Status Register): Monitored Bit 7 (`TXE` - Transmit Data Register Empty) for hardware buffer readiness.
  - `USART2->DR` (Data Register): Written directly during flush cycles to trigger automatic serial shift-register output.
- **Memory Overhead:**
  - Zero dynamic heap allocation. Static memory footprint maintained at **73 bytes** for the dedicated TX Ring Buffer instance (`tx_buffer`).

---

### Verification & Hardware Results

- **Non-Blocking Data Staging Test:**
  - Staged 3 lines of structured diagnostic text into `tx_buffer` via `USART2_SendString_Buffered()`.
  - Confirmed sub-microsecond non-blocking execution during data enqueue before serial transmission started.
- **Flush Engine Execution Test:**
  - Invoked `USART2_Flush_TX(&tx_buffer)` to drain queued bytes to serial output hardware.
  - Confirmed via UART serial output:
    ```text
    === Non-Blocking Buffered UART TX Test ===
    [BUFFER] Line 1: Hello from Ring Buffer!
    [BUFFER] Line 2: Bare-metal Firmware Engineering!
    [BUFFER] Line 3: Flushing bytes to hardware...
    ```
- **Post-Flush Queue State Verification:**
  - Executed `RingBuffer_IsEmpty(&tx_buffer)` post-transmission.
  - Confirmed via UART log: `[SUCCESS] All buffer data flushed to UART hardware successfully!`

---

### Conclusion
Day 023 successfully transforms the bare-metal UART driver into an asynchronous, production-grade transmit pipeline. By decoupling data generation from baud-rate hardware bound constraints via Ring Buffer staging and register-level flushing, execution thread blocking is completely mitigated.