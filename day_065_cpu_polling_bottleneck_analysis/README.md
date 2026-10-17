# Day 065: SPI CPU Polling Bottleneck Analysis & Execution Profiling

### Objective

Analyze and profile the performance limitations of bare-metal SPI data transfers utilizing CPU polling (busy-waiting) on the STM32F401RE microcontroller. This milestone measures precise CPU cycle consumption and execution latency for large-scale data streams (256 bytes), establishing a baseline comparison for future interrupt-driven or DMA-based optimizations.

### Hardware Wiring Setup

```text
+-----------------------------------------------------------------------+
|          STM32F401RE SPI CPU Polling Profiling & Logic Capture        |
+-----------------------------------------------------------------------+
|                                                                       |
|   +---------------------------------------------------------------+   |
|   |                       STM32F401RE SPI1                        |   |
|   |                                                               |   |
|   |   [PA5] SCK  ----+----------------------------------------+   |   |
|   |                  |                                        |   |   |
|   |   [PA7] MOSI ----+--------+ (Breadboard Node A)           |   |   |
|   |                  |        |                               |   |   |
|   |                  |        +-------> [LA Channel 2: D2]    |   |   |
|   |                  |        |                               |   |   |
|   |   [PA6] MISO <---+---+    | (Physical Jumper Loopback)    |   |   |
|   |                      |    |                               |   |   |
|   |   [GND] -------------+----+-------> [LA Ground: Black]    |   |   |
|   +----------------------|----------------------------------------+   |
|                          v                                            |
|                  [LA Channel 1: D1] (SCK Monitoring)                  |
|                                                                       |
|   * Note: Debugging logs sent via USART2 (Virtual COM port)           |
+-----------------------------------------------------------------------+
```

### Key Technical Implementations

- **CPU Polling (Busy-Waiting) Architecture**
  - Implemented a blocking transmission mechanism for a 256-byte SPI data stream, holding the CPU exclusively dedicated to polling the SPI status registers (`TXE` and `RXNE`) until data shifting completes.

- **DWT Cycle Counter Precision Profiling**
  - Utilized the Cortex-M4 Data Watchpoint and Trace (DWT) cycle counter to measure exact clock cycles consumed during the blocking transfer window, yielding deterministic latency metrics.

- **Hardware-in-the-Loop Signal Verification**
  - Interfaced a Cypress FX2-based USB Logic Analyzer via breadboard node branching (`PA5` to LA Channel D1, `PA7` loopback and LA Channel D2) to visualize active clock and data lines in PulseView.

- **Real-Time UART Diagnostic Profiling**
  - Configured USART2 at 115200 baud to output exact operational metrics, tracking total bytes transferred, consumed CPU cycles, and total blocking execution time.

```
=================================================
STM32F401 DAY 65: CPU POLLING BOTTLENECK ANALYSIS
==================================================
[INFO] SPI1 initialized for polling bottleneck profiling.
[PROFILING] Starting large-scale SPI transfer using CPU Polling...
[RESULT] Bytes Transferred: 256
[RESULT] CPU Cycles Consumed: 46898 cycles
[RESULT] Execution Time: 558.31 us (Blocking Polling)
==================================================
System entering normal operation loop...
```

### Conclusion
Day 065 successfully profiles the heavy performance cost of CPU polling during high-throughput SPI transfers. Consuming 46,898 cycles (558.31 microseconds) of purely blocked execution time for just 256 bytes highlights a critical performance bottleneck, proving the absolute necessity for migrating toward interrupt-driven or DMA-based architectures in professional embedded firmware design.