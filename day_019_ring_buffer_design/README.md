# Day 019: Ring Buffer Design & Circular Indexing Simulation

### Objective
Design and implement the core data structure for a FIRST-IN, FIRST-OUT (FIFO) Ring Buffer (Circular Buffer) in C for bare-metal embedded systems. Validate initial buffer state initialization (`head` and `tail` pointers) and verify memory-safe circular indexing logic using modulo arithmetic to prevent buffer overflows during data streaming.

---

### Key Learnings

- **Circular Buffer Data Structure Architecture:** Designed a contiguous fixed-size memory array bounded by `head` (write) and `tail` (read) index pointers, enabling efficient $O(1)$ enqueue and dequeue operations without shifting array elements.
- **Modulo Arithmetic for Wrap-Around:** Mastered circular boundary traversing using the modulo operator `(index + 1) % RING_BUFFER_SIZE`. Evaluated how this ensures index rollover back to `0` when reaching array capacity, eliminating memory leakage and *Index Out of Bounds* faults.
- **Encapsulation & Modularity:** Structural separation of memory layouts (`stm32f401_ring_buffer.h`) and initialization behavior (`stm32f401_ring_buffer.c`) to maintain a clean boundary between core data structures and peripheral hardware drivers.
- **Zero-Allocation Memory Strategy:** Implemented a static buffer allocation model suited for real-time embedded environments, avoiding dynamic memory allocation (`malloc`) to guarantee deterministic execution and zero heap fragmentation.

---

### Technical Implementation

- **Data Structure Definition (`RingBuffer_t`):**
  Engineered `RingBuffer_t` `struct` incorporating a fixed-size byte array (`uint8_t buffer[64]`) alongside 16-bit tracking indices (`head` and `tail`), providing structured state tracking across driver modules.

- **Initialization Routine (`RingBuffer_Init`):**
  Created `void RingBuffer_Init(RingBuffer_t *ring_buffer)` to explicitly zero out both `head` and `tail` pointers upon system startup, guaranteeing a known empty state before processing serial data payloads.

- **Circular Modulo Verification Engine (`main.c`):**
  Constructed a dry-run simulation in `main.c` executing **67** consecutive write-index increments (**64 + 3** steps). Verified that upon reaching the maximum capacity index (**63**), the write pointer seamlessly wrapped around to index **3** without throwing memory exceptions.
---

### Hardware/Registers Involved

- **Peripherals & Core Drivers:**
  - `USART2` (PA2 TX / PA3 RX): Configured via Alternate Function 7 (`AF7`) for real-time serial telemetry and simulation verification logging.
  - `RCC->AHB1ENR` & `RCC->APB1ENR`: Leveraged to supply stable peripheral clocking to `GPIOA` and `USART2`.
- **Memory Overhead:**
  - SRAM Consumption: **64 bytes** (Data Buffer) + **4 bytes** (Head/Tail Pointer tracking) = **68 bytes** total static RAM allocation.
---

### Verification & Hardware Results

- **Initialization State Check:**
  - Confirmed via UART log: `[SUCCESS] Ring Buffer Initialized (head=0, tail=0)`.
- **Circular Modulo Rollover Test:**
  - Simulated pointer movement past the **64**-byte boundary.
  - Confirmed via UART log: `[SUCCESS] Circular Modulo Verified! Head wrapped to index 3.`
  - Validated that `head` pointer rollover operates predictably without corrupting out-of-bounds memory.
  
---

### Conclusion
Day 019 successfully establishes the foundational memory structure and wrap-around logic for an embedded Ring Buffer. By verifying index rollover and state initialization, the driver is fully prepared for Day 020: implementing atomic push operations to handle incoming serial data interrupts safely.