# Day 056: SPI Data Register (DR) Handling & Raw Byte Transmission

### Objective

Implement raw data transmission and reception routines for the SPI1 peripheral on the STM32F401RE by directly interacting with the SPI Data Register (`DR`), establishing the foundational data pipeline before implementing synchronization status polling.

### System Architecture & Signaling Pipeline 

```text
+-----------------------------------------------------------------------+
|                 SPI1 Data Register (DR) Pipeline                      |
+-----------------------------------------------------------------------+
                                   |
                                   v
+-----------------------------------------------------------------------+
|                 1. Data Staging & Injection (Transmit)                |
|  - Write raw 8-bit data (e.g., 0x55) directly into SPI1->DR           |
|  - Hardware automatically shifts bits out via the MOSI physical line  |
+-----------------------------------------------------------------------+
                                   |
                                   v
+-----------------------------------------------------------------------+
|                 2. Shift Register Pipeline                            |
|  - Internal hardware shift register clocks data across the serial bus |
|  - Simultaneous full-duplex shifting mechanism initiates              |
+-----------------------------------------------------------------------+
                                   |
                                   v
+-----------------------------------------------------------------------+
|                 3. Data Extraction (Receive)                          |
|  - Read incoming byte directly from SPI1->DR buffer                   |
|  - Stream successful transmission verification log to PuTTY           |
+-----------------------------------------------------------------------+
```

### Key Technical Implementations

* **Modular Data Register Functions (`DR`)**  
  Implemented dedicated low-level wrapper functions (`SPI1_Transmit` and `SPI1_Receive`) inside `stm32f401_spi.c/.h` to cleanly separate raw data handling from control register configurations.

* **Direct Register Injection (`SPI1->DR = data`)**  
  Pushed dummy test data (`0x55`) straight into the data register to verify that the hardware write path works seamlessly without triggering hard faults or system crashes.

* **Strict Separation of Concerns**  
  Focused strictly on raw data access without premature blocking loops or status flag polling, keeping the architecture modular and fully prepared for Status Register (`SR`) synchronization on Day 57.

---

### Terminal Verification (PuTTY Output)

```text
=================================
  STM32F401 SPI DATA REGISTER   
=================================
[INFO] SPI1 peripheral clock enabled.
[INFO] SPI pins successfully mapped to AF5.
[INFO] SPI1 CR1 configured (Master, PCLK/16, Mode 0).
[INFO] Dummy data (0x55) written to SPI1->DR.
========================================
System entering normal operation loop...
```
---

### Conclusion

Day 56 successfully establishes the raw data input/output pathways for our high-speed serial interface. By interacting directly with the Data Register (DR) through clean modular functions, the firmware maintains a robust foundation, ready for transmission synchronization using Status Register (SR) flags in the next phase.