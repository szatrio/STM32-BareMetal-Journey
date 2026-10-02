# Day 057: SPI Status Register (SR) Polling & Hardware Synchronization

### Objective

Implement active status polling loops for the SPI1 peripheral on the STM32F401RE by monitoring the Status Register (SR)—specifically utilizing TXE (Transmit Buffer Empty) and RXNE (Receive Buffer Not Empty) flags to ensure safe, race-condition-free data transfers.

### System Architecture & Signaling Pipeline 

```text
+-----------------------------------------------------------------------+
|                 SPI1 Status Register (SR) Polling Pipeline             |
+-----------------------------------------------------------------------+
                                   |
                                   v
+-----------------------------------------------------------------------+
|                 1. Transmit Gatekeeper (TXE Polling)                  |
|  - Active loop monitors SPI1->SR Bit 1 (TXE)                          |
|  - CPU halts/waits until transmission buffer is completely empty      |
+-----------------------------------------------------------------------+
                                   |
                                   v
+-----------------------------------------------------------------------+
|                 2. Data Commit & Shifting                             |
|  - Safe injection of data (e.g., 0x55) into SPI1->DR                  |
|  - Hardware shifts data out via MOSI without risk of overwrite        |
+-----------------------------------------------------------------------+
                                   |
                                   v
+-----------------------------------------------------------------------+
|                 3. Receive Gatekeeper (RXNE Polling)                  |
|  - Active loop monitors SPI1->SR Bit 0 (RXNE)                         |
|  - CPU halts/waits until valid incoming data arrives in buffer        |
+-----------------------------------------------------------------------+
```

### Key Technical Implementations

* **TXE Flag Polling (`while (!(SPI1->SR & (1UL << 1)))`)**  
  Upgraded `SPI1_Transmit` to actively check the Transmit Buffer Empty flag before writing data to `DR`, preventing data corruption and hardware-level race conditions.

* **RXNE Flag Polling (`while (!(SPI1->SR & (1UL << 0)))`)**  
  Upgraded `SPI1_Receive` to actively check the Receive Buffer Not Empty flag, ensuring the firmware only extracts valid data once reception is fully completed.

* **Robust Bare-Metal Synchronization**  
  Bridged raw register operations with hardware timing checks, locking down a solid, predictable data pipeline in preparation for Master-Slave communication.

---

```text
=================================
  STM32F401 SPI SR POLLING      
=================================
[INFO] SPI1 peripheral clock enabled.
[INFO] SPI pins successfully mapped to AF5.
[INFO] SPI1 CR1 configured (Master, PCLK/16, Mode 0).
[INFO] Dummy data (0x55) sent via TXE polling.
========================================
System entering normal operation loop...
```
---

### Conclusion

Day 57 successfully transforms our raw SPI driver into a synchronized, hardware-safe communication layer. By introducing active `SR` flag polling for both transmission and reception, the firmware eliminates blind register writes and establishes a reliable foundation for physical loopback testing.

### Terminal Verification (PuTTY Output)