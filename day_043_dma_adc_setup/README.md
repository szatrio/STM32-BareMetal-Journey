# Day 043: DMA Setup & Generic Driver Abstraction

### Objective

Focus on laying the bare-metal foundational driver for Direct Memory Access (DMA) on the STM32F401RE controller. The primary goal is to build a generic, modular DMA driver interface that offloads data memory transfers from the CPU by configuring the control, address, and data counter registers.


### System Architecture & Execution Pipeline

```text
+-----------------------------------------------------------------------+
|                    AHB1 Bus Domain Mapping                            |
|  DMA1 Base: 0x40026000UL  |  DMA2 Base: 0x40026400UL                  |
+-----------------------------------------------------------------------+
                                   |
                                   v
+-----------------------------------------------------------------------+
|                DMA_TypeDef Controller Struct Map                      |
|  1. Status Registers: LISR (Offset 0x00) & HISR (Offset 0x04)         |
|  2. Flag Clear Registers: LIFCR (Offset 0x08) & HIFCR (Offset 0x0C)   |
|  3. Stream Array: Stream[8] Indexed Array Mapping (Offset 0x10-0xBE)  |
+-----------------------------------------------------------------------+
                                   |
                                   v
+-----------------------------------------------------------------------+
|             DMA_Stream_TypeDef Register Configurations                |
|  1. Disable Stream: CR &= ~DMA_SxCR_EN (Wait for hardware halt)       |
|  2. Source Address: PAR = Peripheral Base Pointer                     |
|  3. Destination Address: M0AR = RAM Buffer Memory Pointer             |
|  4. Transfer Count: NDTR = Buffer Item Size                           |
|  5. Control Config (CR): Channel Select | P2M | MINC | CIRC | PL      |
|  6. Enable Stream: CR |= DMA_SxCR_EN                                  |
+-----------------------------------------------------------------------+
```

### Key Technical Implementations

* **Generic DMA Abstraction (`DMA_Stream_Init`)**  
  Instead of hardcoding stream-specific or peripheral-bound configurations, the driver accepts pointers to DMA_Stream_TypeDef, source address, destination address, buffer length, and channel selection. This design ensures full reusability across all streams for DMA1 and DMA2 peripherals.

* **Automatic Bus Clock Gating**  
  The driver dynamically determines whether the stream instance belongs to DMA1 or DMA2 by evaluating its memory base address against DMA2_BASE, automatically enabling the corresponding clock bit in RCC->AHB1ENR.

* **Safe Configuration Sequence**  
  Following STM32 reference manual requirements, streams are explicitly disabled (CR.EN = 0) and verified halted via polling before modifying configuration registers (PAR, M0AR, NDTR, and CR).
  
---

### Conclusion

Day 043 establishes the core driver structures and generic initialization pipeline for DMA transfers. Hardware peripheral integration remains deferred to maintain clear progression milestones across upcoming project days.