# Day 059: SPI Hardware Loopback Test & Full-Duplex Verification

### Objective

Validate the physical transmission and reception capabilities of the bare-metal SPI1 driver on the STM32F401RE microcontroller by performing a hardware loopback test with a physical jumper wire connecting PA7 (MOSI) directly to PA6 (MISO).

### Hardware Wiring Setup

```text
+-----------------------------------------------------------------------+
|                    STM32F401RE Nucleo Loopback Setup                  |
+-----------------------------------------------------------------------+
|                                                                       |
|   +---------------------------------------------------------------+   |
|   |                       STM32F401RE SPI1                        |   |
|   |                                                               |   |
|   |   [PA5] SCK  ---------> (Clock active, unattached)            |   |
|   |                                                               |   |
|   |   [PA7] MOSI --------+                                        |   |
|   |                      | (Physical Jumper Wire)                 |   |
|   |   [PA6] MISO <-------+                                        |   |
|   +---------------------------------------------------------------+   |
|                                                                       |
+-----------------------------------------------------------------------+
```

### Key Technical Implementations

* **Full-Duplex Data Transfer Logic (`SPI1_TransmitReceive`)**  
  Implemented a unified transmission/reception routine that writes data to `DR`, polls `TXE` to ensure packet shift initiation, and subsequently polls `RXNE` to capture incoming hardware response data from the shift register.

* **Physical Loopback Validation**  
  Transmitted test byte `0x41` (ASCII `'A'`) over MOSI and successfully verified that the received byte on MISO matches the transmitted byte bit-for-bit.

* **Hardware Timing & Register Synchronization**  
  Confirmed that peripheral clock generation on SCK safely triggers simultaneous serial data shifting across physical pins without race conditions or buffer overrun issues.

### Terminal Verification (PuTTY Output)

```
===================================
  STM32F401 SPI HARDWARE LOOPBACK  
===================================
[INFO] SPI1 initialized in Master Mode.
[INFO] Make sure PA7 (MOSI) is connected to PA6 (MISO) via jumper!
[TEST] Sending byte: 0x41 ('A')...
[SUCCESS] Loopback PASSED! Data matches.
========================================
System entering normal operation loop...
```
### Conclusion

Day 59 successfully proves the physical and logical integrity of our bare-metal SPI driver. By establishing full-duplex communication via SPI1_TransmitReceive and verifying it with physical hardware wiring, the driver is fully validated and ready for full-duplex stream testing in Day 60.