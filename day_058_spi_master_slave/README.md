# Day 058: SPI Master vs. Slave Mode Configuration & NSS Management

### Objective

Enhance the SPI1 configuration routine on the STM32F401RE by introducing dynamic Master/Slave selection via the `CR1` register's `MSTR` bit, alongside configuring Software Slave Management (`SSM` and `SSI`) to prevent Master Mode Faults (`MODF`).

### System Architecture & Signaling Pipeline

```text
+-----------------------------------------------------------------------+
|              SPI1 Dynamic Role & NSS Control Pipeline                 |
+-----------------------------------------------------------------------+
                                   |
                                   v
+-----------------------------------------------------------------------+
|                 1. SPI Peripheral Disable (SPE = 0)                   |
|  - Disable SPI1 in CR1 (Bit 6) before altering operating modes        |
+-----------------------------------------------------------------------+
                                   |
                                   v
+-----------------------------------------------------------------------+
|                 2. Dynamic Role Assignment                            |
|  - Master Mode (master_mode = 1): Set MSTR (Bit 2) = 1                |
|  - Slave Mode  (master_mode = 0): Clear MSTR (Bit 2) = 0              |
+-----------------------------------------------------------------------+
                                   |
                                   v
+-----------------------------------------------------------------------+
|                 3. Software Slave Management (SSM / SSI)              |
|  - Master Mode: Set SSM (Bit 9) = 1, SSI (Bit 8) = 1                  |
|    (Forces internal NSS HIGH, preventing Master Mode Fault)           |
|  - Slave Mode:  Set SSM (Bit 9) = 1, SSI (Bit 8) = 0                  |
|    (Forces internal NSS LOW, locking device in Slave role)            |
+-----------------------------------------------------------------------+
```
### Key Technical Implementations

* **Dynamic Role Selection (`MSTR` Bit Configuration)**  
  Updated `SPI1_Config` with a `master_mode` parameter, allowing runtime/compile-time switching between Master (`MSTR = 1`) and Slave (`MSTR = 0`) operational modes.

* **Software Slave Management (`SSM` & `SSI` Bits)**  
  Configured `SSM = 1` and `SSI = 1` for Master mode to tie the internal Slave Select signal to VDD in software, preventing hardware-driven Master Mode Faults (`MODF`) when physical NSS pins are unmanaged.

* **Clean Register Reconfiguration**  
  Maintained strict sequence by clearing the `SPE` bit before modifying control bitfields, re-enabling the peripheral only after all settings were safely committed to `CR1`.

### Terminal Verification (PuTTY Output)

```
=====================================
  STM32F401 SPI MASTER-SLAVE CONFIG  
=====================================
[INFO] SPI1 peripheral clock enabled.
[INFO] SPI pins successfully mapped to AF5.
[INFO] SPI1 CR1 configured (Master, PCLK/16, Mode 0).
[INFO] Dummy data (0x55) sent via TXE polling.
========================================
System entering normal operation loop...
```

### Conclusion

Day 58 successfully establishes dynamic role configuration and robust NSS signal management for our bare-metal SPI driver. By safely handling `MSTR`, `SSM`, and `SSI` register bits, the peripheral is fully protected against mode faults and completely prepared for physical loopback testing in Day 59.
