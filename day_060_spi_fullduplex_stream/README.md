# Day 060: Full-Duplex SPI Stream Data Transfer & UART Validation

### Objective

Validate multi-byte, full-duplex stream communication capabilities of the bare-metal SPI1 driver on the STM32F401RE microcontroller. This stage scales up from single-byte verification to streaming buffer transfer (`SPI1_TransmitReceiveStream`) combined with real-time debugging logs via UART2.

### Hardware Wiring Setup

```text
+-----------------------------------------------------------------------+
|                    STM32F401RE SPI Stream Setup                       |
+-----------------------------------------------------------------------+
|                                                                       |
|   +---------------------------------------------------------------+   |
|   |                       STM32F401RE SPI1                        |   |
|   |                                                               |   |
|   |   [PA5] SCK  ---------> (Clock active, internal generation)   |   |
|   |                                                               |   |
|   |   [PA7] MOSI --------+                                        |   |
|   |                      | (Physical Jumper Wire)                 |   |
|   |   [PA6] MISO <-------+                                        |   |
|   +---------------------------------------------------------------+   |
|                                                                       |
|   * Note: Debugging logs sent via USART2 (ST-LINK Virtual COM port)    |
+-----------------------------------------------------------------------+
```

### Key Technical Implementations

* **Stream-Based Full-Duplex Routine (`SPI1_TransmitReceiveStream`)**  
  Implemented a robust iterative buffer transfer loop that handles concurrent transmission and reception across a payload array (`uint8_t *pTxData` and `uint8_t *pRxData`) using safe register polling (`TXE` and `RXNE`).

* **Buffer Integrity & Validation (`memcmp`)**  
  Integrated a robust string/buffer verification step using `memcmp` to ensure every single byte in the transmitted payload array matches the received loopback buffer without data corruption or frame misalignment.

* **Real-Time UART Debugging Infrastructure**  
  Leveraged USART2 at 115200 baud to output descriptive runtime states, register configuration milestones, and structured test result logs directly to the host development terminal.

### Terminal Verification (PuTTY Output)

```text
========================================
  STM32F401 SPI FULL-DUPLEX STREAM TEST 
========================================
[INFO] SPI1 initialized in Master Mode.
[INFO] Make sure PA7 (MOSI) is connected to PA6 (MISO) via jumper!
[TEST] Transmitting stream buffer via SPI1_TransmitReceiveStream()...
[SUCCESS] Stream Loopback PASSED!
[INFO] Received data matches TX buffer perfectly:
STM32_SPI_STREAM_TEST_OK
========================================
System entering normal operation loop...
```

### Conclusion

Day 60 successfully establishes and verifies high-reliability stream transmission over SPI1 using a pure bare-metal architecture. With data arrays shifting accurately and verified via real-time UART logging, the SPI driver core is fully primed for high-level peripheral integration.