# Day 043: ADC Data Streaming with DMA Circular Buffer

### Objective

Integrate the generic DMA driver with ADC1 for continuous data acquisition on the STM32F401RE controller. Additionally, document a comparative technical analysis of data transfer mechanisms (Polling, Interrupt, and DMA) to contextualize hardware offloading performance.

### System Architecture & Execution Pipeline

```text
+-----------------------------------------------------------------------+
|                    AHB1 / APB2 Bus Interconnect                       |
|  ADC1 Base: 0x40012000UL  |  DMA2 Base: 0x40026400UL                  |
+-----------------------------------------------------------------------+
                                   |
                                   v
+-----------------------------------------------------------------------+
|                 Peripheral Trigger & Request Flow                     |
|  1. ADC Continuous Conversion -> Generates Hardware DMA Request       |
|  2. DMA2 Stream 0 (Channel 0) intercepts Peripheral Request           |
|  3. DMA Hardware Controller fetches data from ADC1->DR                |
+-----------------------------------------------------------------------+
                                   |
                                   v
+-----------------------------------------------------------------------+
|               Zero-CPU Overhead Data Transfer Loop                    |
|  1. Source: ADC1->DR (Peripheral Address Register / PAR)              |
|  2. Destination: adc_dma_buffer (Memory 0 Address Register / M0AR)    |
|  3. Count & Mode: NDTR Auto-Reload via Circular Mode (CIRC = 1)       |
|  4. CPU State: Fully Executing Main Loop without Polling / ISRs       |
+-----------------------------------------------------------------------+
```
### Key Technical Implementations

* **Pointer Struct Refactoring (`RCC_TypeDef`)**  
  Refactored RCC register definitions from raw direct macro addresses to a structured pointer map (`RCC_TypeDef`), bringing full alignment with peripheral drivers like GPIO, ADC, and DMA.

* **ADC1-to-DMA Hardware Integration**  
  Configured `DMA2_Stream0` (Channel 0) to stream conversion results directly from `ADC1->DR` to a local memory buffer using strict stream address casting (`&(DMA2->Stream[0])`).

* **Circular Transfer Mode**  
  Enabled continuous non-blocking transfer by setting `DMA_SxCR_CIRC`, allowing memory buffers to update automatically without software intervention or counter reset routines.

---

### Comparative Analysis: Data Transfer Strategies

| Metric / Feature | Polling (Blocking) | Interrupt (ISR) | Direct Memory Access (DMA) |
| :--- | :--- | :--- | :--- |
| **CPU Utilization** | High (100% busy-wait lock) | Medium (CPU overhead on every ISR entry/exit) | Very Low (0% CPU intervention during transfer) |
| **Data Throughput** | Low to Moderate | Moderate | Maximum (Hardware bus mastering speed) |
| **Latency & Jitter** | Deterministic, but blocks execution | Subject to interrupt priorities and context switching | Near-zero hardware propagation delay |
| **Hardware Complexity** | Minimal software design | Requires NVIC configuration & ISR handlers | Requires DMA stream, channel, & memory mapping |
| **Ideal Use Case** | Simple status checks / single conversions | Low-frequency events (e.g., Push Button, EXTI) | High-speed / Continuous stream (ADC, SPI, UART RX) |

---

### Conclusion

Day 044 completes the direct integration between the ADC1 peripheral and DMA2 hardware stream in continuous circular mode. This validates zero-CPU overhead data acquisition, setting up the bare-metal foundation for processing incoming sensor channels without execution blocking.