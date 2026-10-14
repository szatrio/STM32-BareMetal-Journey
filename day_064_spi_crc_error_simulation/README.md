# Day 064: SPI Data Corruption Simulation & Error Detection Verification

### Objective

Validate the robustness of the hardware CRC (Cyclic Redundancy Check) engine on the STM32F401RE microcontroller by intentionally introducing data corruption (transmission noise simulation) into the SPI stream buffer. This milestone demonstrates the system's active error-detection capabilities, ensuring that compromised communication packets are successfully identified and flagged.

### Hardware Wiring Setup

```text
+-----------------------------------------------------------------------+
|             STM32F401RE CRC Error Detection & Simulation              |
+-----------------------------------------------------------------------+
|                                                                       |
|   +---------------------------------------------------------------+   |
|   |                       STM32F401RE SPI1 / CRC                  |   |
|   |                                                               |   |
|   |   [PA5] SCK  ---------> (Clock active, master generation)     |   |
|   |                                                               |   |
|   |   [PA7] MOSI --------+                                        |   |
|   |                      | (Physical Jumper Wire Loopback)        |   |
|   |   [PA6] MISO <-------+                                        |   |
|   |                                                               |   |
|   |   [AHB1] CRC Engine -> (Active Error Detection & Flagging)    |   |
|   +---------------------------------------------------------------+   |
|                                                                       |
|   * Note: Debugging logs sent via USART2 (Virtual COM port)           |
+-----------------------------------------------------------------------+
```

### Key Technical Implementations

- **Controlled Noise Injection (`rx_buffer[5] ^= 0xFF`)**  
  Introduced explicit bit-flipping on a target byte within the received payload buffer (`rx_buffer`) to simulate real-world electromagnetic interference, line noise, or packet corruption during serial transmission.

- **Asymmetric Checksum Validation**  
  Compared the pre-transmission sender checksum (`tx_crc`) against the post-corruption receiver checksum (`rx_crc`), verifying that alterations in data streams immediately invalidate the computed hardware polynomial value.

- **Direct Register-Level Architecture**  
  Maintained strict bare-metal compliance across SPI1 and AHB1-bus CRC peripherals, utilizing direct register mapping and cycle-efficient hardware computation blocks without high-level abstraction layers.

- **Real-Time UART Diagnostic & Error Logging**  
  Configured USART2 output at 115200 baud to trace execution flow step-by-step, capturing the transition from payload transmission to intentional corruption and successful error flagging.

```
============================================
STM32F401 DAY 64: SPI + CRC ERROR SIMULATION
============================================
[INFO] SPI1 and CRC hardware initialized.
[INFO] Sender computed CRC checksum for payload.
[TEST] Transmitting stream via SPI1 loopback...
[TEST] Simulating transmission noise / data corruption...
[INFO] Receiver computed CRC checksum from RX buffer.
[ERROR] CRC MISMATCH! Data corruption detected.
========================================
System entering normal operation loop...
```

### Conclusion
Day 064 successfully validates the error-detection reliability of the bare-metal SPI and CRC pipeline. By proving that simulated transmission noise instantly triggers a checksum mismatch ([ERROR] CRC MISMATCH!), the firmware architecture confirms its readiness for secure, robust serial communication tasks in professional embedded environments.