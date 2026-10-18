# Day 066: SPI DMA Configuration & Architecture

### Objective

Establish and configure the bare-metal DMA (Direct Memory Access) architecture for SPI1 full-duplex communication on the STM32F401RE microcontroller. This milestone transitions the firmware away from CPU blocking by setting up dedicated DMA streams for asynchronous TX and RX operations, eliminating CPU intervention during data shifting.

### Hardware Wiring Setup

```text
+-----------------------------------------------------------------------+
|          STM32F401RE SPI1 DMA Hardware Loopback Setup                 |
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

- **Dedicated SPI DMA Stream Mapping**
  - Configured **DMA2 Stream 3 (Channel 3)** as the Memory-to-Peripheral (M2P) channel for SPI1 TX.
  - Configured **DMA2 Stream 0 (Channel 3)** as the Peripheral-to-Memory (P2M) channel for SPI1 RX.

- **Flexible Low-Level DMA Driver Extension**
  - Added `DMA_SPI_Stream_Init()` inside `stm32f401_dma.c` to handle 8-bit normal mode transfers, enabling memory increment (`MINC`) while keeping peripheral addresses fixed to the SPI Data Register (`DR`).

- **SPI Peripheral DMA Handshake Integration**
  - Extended `stm32f401_spi.c` with `SPI1_DMA_Enable()` to programmatically assert the `TXDMAEN` and `RXDMAEN` bits inside the `SPI_CR2` register, authorizing the SPI peripheral to issue hardware DMA requests.

- **Real-Time UART Diagnostic Verification**
  - Initialized USART2 at 115200 baud to validate system stability, confirming fault-free register mapping and stream configuration prior to runtime data transfers.

```
==================================================
STM32F401 DAY 66: SPI DMA CONFIG & ARCHITECTURE
==================================================
[INFO] SPI1 initialized for DMA configuration.
[INFO] DMA Streams configured and SPI DMA requests enabled.
==================================================
```

### Conclusion
Day 066 successfully establishes the foundational pipeline for non-blocking SPI communication. By cleanly mapping DMA2 streams and tying peripheral request lines without encountering hardfaults, the firmware is fully prepared for executing asynchronous data transfers and running comparative performance profiling against the Day 065 polling benchmark.