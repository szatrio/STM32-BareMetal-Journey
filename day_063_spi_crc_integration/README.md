# Day 063: SPI and CRC Hardware Integration & Data Integrity Verification

### Objective

Integrate the internal hardware CRC (Cyclic Redundancy Check) engine directly into the SPI communication data stream on the STM32F401RE microcontroller. This milestone bridges independent peripheral modules, enabling end-to-end checksum generation and real-time verification for transmitted and received data payloads.

### Hardware Wiring Setup

```text
+-----------------------------------------------------------------------+
|             STM32F401RE SPI & CRC End-to-End Integration              |
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
|   |   [AHB1] CRC Engine -> (Sender & Receiver Checksum Matching)  |   |
|   +---------------------------------------------------------------+   |
|                                                                       |
|   * Note: Debugging logs sent via USART2 (Virtual COM port)           |
+-----------------------------------------------------------------------+
```

### Key Technical Implementations

- **End-to-End Payload Checksum (`CRC_CalculateBuffer`)**  
  Computed hardware-accelerated CRC checksums on the transmission buffer (`tx_buffer`) prior to sending, and subsequently on the reception buffer (`rx_buffer`) following the SPI loopback transfer.

- **SPI Stream Loopback Integration**  
  Interfaced the raw register-level SPI1 driver (`SPI1_TransmitReceiveStream`) with the hardware CRC unit to validate multi-byte data stream integrity without utilizing software-heavy mathematical loops.

- **Direct Register-Level Architecture**  
  Maintained strict bare-metal standards across both SPI and CRC peripherals, directly interacting with memory-mapped registers, alternate function configurations, and bus clocks without HAL abstraction layers.

- **Real-Time UART Diagnostic & Validation Logging**  
  Integrated USART2 debugging output at 115200 baud to monitor step-by-step execution, comparing sender and receiver checksums to confirm data transmission accuracy.

```
========================================
   STM32F401 DAY 63: SPI + CRC INTEGRATION
========================================
[INFO] SPI1 and CRC hardware initialized.
[INFO] Sender computed CRC checksum for payload.
[TEST] Transmitting stream via SPI1 loopback...
[INFO] Receiver computed CRC checksum from RX buffer.
[SUCCESS] CRC MATCH! Data integrity verified perfectly.
========================================
System entering normal operation loop...
```

### Conclusion
Day 063 successfully unites the SPI communication module with the hardware CRC engine in a complete end-to-end workflow. By validating that sender and receiver checksums match flawlessly over a physical loopback, the firmware architecture achieves a robust mechanism for reliable, error-detected serial communication.