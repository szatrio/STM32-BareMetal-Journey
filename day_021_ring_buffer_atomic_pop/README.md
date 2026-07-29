# Day 021: Atomic Pop Operation & Underflow Protection

### Objective
Implement and validate the core data extraction mechanism (`RingBuffer_Pop`) for a bare-metal Ring Buffer driver in C. Focus on state-safe byte retrieval, First-In, First-Out (FIFO) queue order verification, underflow condition protection, and pointer dereferencing safety in embedded environments.

---

### Key Learnings

- **Data Extraction Mechanics (Dequeue):** Mastered non-destructive byte retrieval from continuous memory arrays, ensuring the read pointer (`tail`) advances predictably while tracking incoming data streams.
- **Underflow Protection Logic:** Implemented pre-read boundary validation (`head == tail`) to detect empty buffer states, preventing invalid pointer dereferencing and out-of-order array reads.
- **Pointer-Based Pass-by-Reference API Design:** Utilized C pass-by-reference semantics (`uint8_t *data`) to safely mutate output buffers while preserving atomic function status returns (`bool`).
- **Full FIFO Lifecycle Verification:** Validated complete data pipeline dynamics—ensuring the exact payload sequence pushed into memory via `head` is extracted identically in sequential order via `tail`.

---

### Technical Implementation

- **Data Dequeue API (`RingBuffer_Pop`):**
  Engineered `bool RingBuffer_Pop(RingBuffer_t *ring_buffer, uint8_t *data)` in `stm32f401_ring_buffer.c`. Validates read availability against active `head` position, extracts target payload from `buffer[tail]`, and advances `tail` via atomic circular indexing `(tail + 1) % RING_BUFFER_SIZE`.

- **Pre-Read Boundary Check:**
  Enforced strict underflow checks prior to memory access. If `head` equals `tail`, the function returns `false` without modifying pointer state or writing to the target memory address.

- **Hardware Verification Suite (`main.c`):**
  Constructed a complete FIFO validation sequence: pushed continuous characters (`'A'` through `'E'`), executed sequential pop calls verifying exact return payloads, and asserted underflow protection logic on a fully depleted buffer.

---

### Hardware/Registers Involved

- **Peripherals & Core Drivers:**
  - `USART2` (PA2 TX / PA3 RX): Configured via Alternate Function 7 (`AF7`) at 115200 baud for real-time serial execution logging.
  - `RCC->AHB1ENR` & `RCC->APB1ENR`: Supplying stable peripheral clocks to `GPIOA` and `USART2`.
- **Memory Overhead:**
  - Zero dynamic heap allocation. Static memory footprint maintained at **68 bytes** (64-byte payload array + 4-byte index tracking pointers).

---

### Verification & Hardware Results

- **Sequential FIFO Data Extraction Test:**
  - Confirmed via UART log: `[SUCCESS] Pushed ABCDE to Ring Buffer`.
  - Sequential Pop logs verified exact read order:
    - `1st Pop returned 'A' (First-In, First-Out Passed!)`
    - `2nd Pop returned 'B'`
    - `3rd Pop returned 'C'`
    - `4th Pop returned 'D'`
    - `5th Pop returned 'E'`
- **Underflow Protection Verification:**
  - Triggered post-depletion pop operation (`head == tail`).
  - Confirmed via UART log: `[SUCCESS] Pop rejected! Buffer is empty (Underflow Protected).`

---

### Conclusion
Day 021 successfully completes the full read/write cycle of the bare-metal Ring Buffer driver. With both `RingBuffer_Push` and `RingBuffer_Pop` verified for state safety, overflow/underflow resilience, and FIFO integrity, the driver is fully prepared for Day 022: integrating the Ring Buffer into real UART Receive Interrupt (RXNE) Handlers.