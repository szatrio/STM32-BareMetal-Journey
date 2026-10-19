# Day 067: UART DMA Transmission (Memory-to-Peripheral)

### Objective

Transition the firmware from CPU-blocking UART polling to asynchronous, non-blocking string transmission using the DMA (Direct Memory Access) controller on the STM32F401RE microcontroller. This milestone optimizes CPU resource utilization by offloading buffer transfers directly to the DMA hardware.

### Key Technical Implementations

- **UART DMA Stream Mapping**
  - Configured **DMA1 Stream 6 (Channel 4)** as the Memory-to-Peripheral (M2P) channel dedicated to `USART2_TX`.

- **Low-Level Driver & Register Integration**
  - Programmed the `USART_CR3_DMAT` bit via `USART_DMA_Enable()` to authorize the USART2 peripheral to issue hardware DMA requests whenever its transmit data register is empty.
  - Standardized the DMA initialization helper to `DMA_Stream_Init()` for flexible, peripheral-agnostic stream configuration (enabling memory increment `MINC` while keeping the peripheral address fixed to `USART2->DR`).

- **CPU Overhead Profiling via DWT**
  - Integrated the Cortex-M4 DWT (Data Watchpoint and Trace) cycle counter to measure exact CPU execution overhead required to initiate a DMA-driven transmission.

```text
==================================================
STM32F401 DAY 67: UART DMA TRANSMISSION
==================================================
[INFO] UART DMA Stream configured and DMAT enabled.
==================================================
[PROFILING] Trigger cycles: 15
```

### Conclusion
Day 067 successfully validates the performance superiority of asynchronous communication. By consuming only 15 CPU clock cycles to trigger the transfer, the firmware completely liberates the CPU from TXE busy-waiting loops, enabling background task execution while bytes are reliably streamed out via DMA in the background.