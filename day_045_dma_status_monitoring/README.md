# Day 045: DMA Status Monitoring & Flag Clearing

### Objective

Implement robust hardware handshake mechanisms for the ADC1-to-DMA2 pipeline using status monitoring registers (`DMA_LISR`) and clear flag control registers (`DMA_LIFCR`). This ensures non-corrupt, synchronized data access from the RAM buffer at full hardware speed without relying on artificial CPU delays.

### System Architecture & Execution Pipeline

```text
+-----------------------------------------------------------------------+
|                    AHB1 / APB2 Bus Interconnect                       |
|  ADC1 Base: 0x40012000UL  |  DMA2 Base: 0x40026400UL                  |
+-----------------------------------------------------------------------+
                                   |
                                   v
+-----------------------------------------------------------------------+
|              Hardware Event & Status Flag Handshake                   |
|  1. ADC1 Continuous Conversion -> Triggers DMA2 Stream 0 Transfer     |
|  2. DMA Hardware writes ADC1->DR value into SRAM (adc_dma_buffer)     |
|  3. Transfer Completes -> Hardware sets TCIF0 bit in DMA2->LISR       |
+-----------------------------------------------------------------------+
                                   |
                                   v
+-----------------------------------------------------------------------+
|                 Synchronized CPU Access & Clear Loop                  |
|  1. CPU Polls Flag: Reads DMA2->LISR to check TCIF0 (Transfer Done)   |
|  2. Safe Execution: CPU accesses adc_dma_buffer[0] with 0% corruption |
|  3. Hardware Acknowledgment: CPU writes CTCIF0 to DMA2->LIFCR         |
|  4. Flag Resets: TCIF0 cleared back to 0 for next transfer cycle      |
+-----------------------------------------------------------------------+
```

### Key Technical Implementations

* **Hardware Handshake Abstraction (`DMA_LISR` & `DMA_LIFCR`)**  
  Implemented direct register masking for stream status monitoring. `TCIF0` (Transfer Complete Interrupt Flag) in `DMA2->LISR` signals valid SRAM writes, while writing `CTCIF0` to `DMA2->LIFCR` resets the status line to prevent register deadlocks.

* **Driver Refactoring (`stm32f401_dma.h` & `stm32f401_dma.c`)**  
  Added dedicated bare-metal helper functions `DMA_GetTCFlag()` and `DMA_ClearTCFlag()` to encapsulate status checks and register clearing for `DMA2_Stream0`.

* **Removal of Artificial CPU Throttling**  
  Eliminated software delays (`SysTick_DelayMs`) from the main loop. Data processing now runs synchronously at full hardware speed, strictly controlled by hardware transfer completion flags.

---
### Comparative Analysis: Day 044 vs. Day 045

| Metric / Feature | Day 044 (Basic DMA Trigger) | Day 045 (Status Monitored Handshake) |
| :--- | :--- | :--- |
| **Log Execution Trigger** | Artificial Timer (`SysTick_DelayMs`) | Hardware Event Flag (`DMA2->LISR` bit `TCIF0`) |
| **SRAM Data Integrity** | Risk of race conditions / partial reads | **100% Safe** (CPU reads only on transfer completion) |
| **Execution Rate** | Throttled by software delays | Full hardware-speed (Limited only by UART baud rate) |
| **Handshake Protocol** | One-way (DMA pushes data, CPU assumes valid) | Two-way (DMA sets `TCIF0`, CPU clears via `CTCIF0`) |
| **Hardware Health Control** | Blind memory polling | Active register debugging & status monitoring |

---

### Conclusion

Day 045 successfully elevates the ADC-DMA pipeline from blind memory sampling to a fully synchronized hardware-handshake architecture. By monitoring `DMA_LISR` and explicitly clearing flags via `DMA_LIFCR`, the driver guarantees zero-data-corruption access at full hardware speed.