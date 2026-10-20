# Day 068: High-Speed SPI DMA Transfer & Loopback Verification

### Objective

Execute and validate high-speed, full-duplex asynchronous SPI data transfers using the dual-stream DMA (Direct Memory Access) architecture on the STM32F401RE microcontroller. This milestone achieves simultaneous non-blocking transmission and reception across a 256-byte payload, verifying end-to-end data integrity via hardware loopback and profiling CPU trigger latency.

### Hardware Wiring Setup

```text
+-----------------------------------------------------------------------+
|       STM32F401RE High-Speed SPI1 DMA Hardware Loopback Setup         |
+-----------------------------------------------------------------------+
|                                                                       |
|   +---------------------------------------------------------------+   |
|   |                       STM32F401RE SPI1                        |   |
|   |                                                               |   |
|   |   [PA5] SCK  ----------------------------------- (Open/Float) |   |
|   |                                                               |   |
|   |   [PA7] MOSI -----------------+                               |   |
|   |                               | (Physical Jumper Loopback)    |   |
|   |   [PA6] MISO <----------------+                               |   |
|   |                                                               |   |
|   |   [GND] ---------------------------------------- (Common GND) |   |
|   +---------------------------------------------------------------+   |
|                                                                       |
|   * Note: Debugging logs sent via USART2 (115200 Baud)                |
+-----------------------------------------------------------------------+
```
### Key Technical Implementations

- **Simultaneous Dual-Stream Activation**
  - Triggered **DMA2 Stream 0 (RX)** followed immediately by **DMA2 Stream 3 (TX)** to initiate concurrent full-duplex DMA transfers without CPU polling intervention.

- **CPU Overhead Profiling via DWT**
  - Integrated the Cortex-M4 DWT cycle counter, confirming an ultra-low execution overhead of **only 24 CPU clock cycles** to trigger the entire 256-byte dual-stream transfer.

- **Transfer Synchronization & Status Polling**
  - Utilized dynamic interrupt status register helpers (`DMA_GetTCFlag` and `DMA_ClearTCFlag`) across LISR/HISR to reliably poll transfer completion flags (`TCIF`) for both DMA streams.

- **Automated Data Integrity Verification**
  - Implemented an automated post-transfer verification routine comparing the received RAM buffer (`rx_buffer`) against the transmitted payload (`tx_buffer`) byte by byte.
```
==================================================
STM32F401 DAY 68: HIGH-SPEED SPI DMA TRANSFER
==================================================
[INFO] SPI1 initialized.
[INFO] DMA streams configured & SPI DMA enabled.
==================================================
[PROFILING] Trigger cycles: 24
[INFO] SPI DMA Transfer Completed!
[SUCCESS] Data integrity verified! RX matches TX.
```

### Conclusion
Day 068 successfully validates full-duplex high-speed SPI communication driven entirely by DMA. By requiring only 24 cycles to trigger 256 bytes of simultaneous transfer and confirming 100% data fidelity over the physical loopback jumper, the firmware proves the absolute efficiency of non-blocking peripheral management.