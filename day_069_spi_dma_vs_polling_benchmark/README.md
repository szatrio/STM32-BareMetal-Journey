# Day 069: SPI Final Benchmark & Performance Profiling (Polling vs. DMA)

### Objective

Conclude the performance tuning milestone by conducting a rigorous, comparative benchmark between traditional CPU polling and the dual-stream DMA architecture on the STM32F401RE microcontroller. This milestone delivers empirical validation of the architectural efficiency gains, quantifying the massive reduction in CPU cycles and execution overhead for high-speed SPI communications.

### Hardware Wiring Setup

```text
+-----------------------------------------------------------------------+
|       STM32F401RE Final SPI Benchmark & Profiling Setup               |
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

- **Comparative Execution Profiling via DWT**
  - Integrated the Cortex-M4 DWT cycle counter to measure and contrast the exact CPU clock consumption between blocking CPU polling and non-blocking DMA triggering across an identical 256-byte **SPI** transfer payload.

- **Empirical Efficiency Quantization**
  - Captured precise metrics proving that **SPI** DMA dual-stream execution slashes CPU overhead from **46,886 cycles** down to just **24 cycles**, achieving a dramatic **~1,953x efficiency gain**.

- **Sequential Dual-Method Execution**
  - Structured `main.c` to sequentially execute the CPU polling **SPI** transmission stream and the DMA dual-stream transfer within a unified test session for direct, side-by-side terminal verification.

- **Real-Time UART Diagnostic Reporting**
  - Formatted and transmitted comprehensive **SPI** profiling summaries via USART2 at 115200 baud, outputting exact cycle counts, execution times in microseconds, and computed speedup ratios.
```
==================================================
STM32F401 DAY 69: SPI FINAL BENCHMARK & PROFILING
==================================================
[BENCHMARK] Payload Size      : 256 Bytes
[BENCHMARK] Polling Cycles    : 46886 cycles (558.17 us)
[BENCHMARK] DMA Trigger Cycles: 24 cycles (0.29 us)
[RESULT] Efficiency Gain      : ~1953ux fewer CPU cycles!
==================================================
```

### Conclusion
Day 069 successfully crowns the SPI DMA performance tuning journey with definitive empirical proof. By demonstrating a ~1,953x reduction in CPU cycle consumption for a 256-byte SPI transfer, the project conclusively validates that offloading data movement to the DMA controller frees up the processor entirely, setting a robust foundation for building high-performance, real-time embedded applications.