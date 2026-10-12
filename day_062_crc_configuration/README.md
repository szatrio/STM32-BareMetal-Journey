# Day 062: CRC Reset Control & Stream Buffer Integrity Verification

### Objective

Expand the internal hardware CRC (Cyclic Redundancy Check) engine implementation on the STM32F401RE microcontroller by mastering peripheral reset control (`CRC_CR_RESET`) and multi-byte stream buffer processing. This milestone eliminates residual data state contamination and ensures reliable CRC checksum generation for real-world serial communication frames.

### Hardware Wiring Setup

```text
+-----------------------------------------------------------------------+
|              STM32F401RE CRC Reset & Stream Buffer Setup              |
+-----------------------------------------------------------------------+
|                                                                       |
|   +---------------------------------------------------------------+   |
|   |                       STM32F401RE SPI1 / CRC                  |   |
|   |                                                               |   |
|   |   [PA5] SCK  ---------> (Clock active, master generation)     |   |
|   |                                                               |   |
|   |   [PA7] MOSI --------+                                        |   |
|   |                      | (Physical Jumper Wire Loopback)        |   |
|   |   [PA6] MISO <-------+                                        |   |
|   |                                                               |   |
|   |   [AHB1] CRC Engine -> (Reset-controlled hardware checksum)   |   |
|   +---------------------------------------------------------------+   |
|                                                                       |
|   * Note: Debugging logs sent via USART2 (Virtual COM port)           |
+-----------------------------------------------------------------------+
```

### Key Technical Implementations

- **Hardware State Reset Control (`CRC_Reset`)**  
  Implemented explicit control register manipulation (`CRC->CR |= CRC_CR_RESET`) to force the hardware accumulator (`CRC->DR`) back to its default seed value (`0xFFFFFFFF`). This prevents data corruption caused by residual states from previous packet calculations.

- **Stream Buffer Ingestion (`CRC_CalculateBuffer`)**  
  Engineered a multi-word processing loop capable of ingesting arbitrary byte arrays (such as SPI loopback payload buffers) and evaluating live data integrity through the hardware peripheral without CPU-heavy software overhead.

- **Direct Register-Level Architecture**  
  Maintained strict bare-metal compliance without utilizing HAL abstractions, interacting directly with AHB1 bus peripheral clock registers (`RCC_AHB1ENR_CRCEN`) and control/data registers.

- **Real-Time UART Diagnostic Logging**  
  Integrated USART2 debugging output at 115200 baud to monitor peripheral initialization states, reset execution flags, and live stream computation validation.

### Terminal Verification (PuTTY Output)
```
===========================================
   STM32F401 HARDWARE CRC RESET & BUFFER
===========================================
[INFO] CRC peripheral clock enabled & reset executed.
[INFO] SPI1 initialized in Master Mode.
[INFO] Make sure PA7 (MOSI) is connected to PA6 (MISO) via jumper!
[TEST] Transmitting stream buffer via SPI1...
[TEST] Calculating Hardware CRC on stream buffer with Reset Control...
[SUCCESS] CRC Checksum calculated successfully!
[INFO] CRC Engine Reset & Stream Computation Verified.
========================================
System entering normal operation loop...
```

### Conclusion
Day 062 successfully integrates precise peripheral state management into the bare-metal firmware architecture. By implementing automated hardware reset routines before block processing, the system ensures immaculate checksum isolation across independent data frames, securing a robust foundation for upcoming communication protocol modules.