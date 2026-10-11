# Day 061: Hardware CRC Introduction & Data Integrity Verification

### Objective

Introduce and validate the internal hardware CRC (Cyclic Redundancy Check) engine on the STM32F401RE microcontroller. This milestone establishes the foundation for data integrity verification by interfacing directly with the chip's built-in CRC registers (`CRC_DR`, `CRC_CR`) to compute cryptographic checksums without CPU-intensive mathematical overhead.

### Hardware Wiring Setup

```text
+-----------------------------------------------------------------------+
|                    STM32F401RE Hardware CRC Setup                     |
+-----------------------------------------------------------------------+
|                                                                       |
|   +---------------------------------------------------------------+   |
|   |                       STM32F401RE SPI1 / CRC                  |   |
|   |                                                               |   |
|   |   [PA5] SCK  ---------> (Clock active, internal generation)   |   |
|   |                                                               |   |
|   |   [PA7] MOSI --------+                                        |   |
|   |                      | (Physical Jumper Wire Loopback)        |   |
|   |   [PA6] MISO <-------+                                        |   |
|   |                                                               |   |
|   |   [AHB1] CRC Engine -> (Internal Hardware Checksum Generator) |   |
|   +---------------------------------------------------------------+   |
|                                                                       |
|   * Note: Debugging logs sent via USART2 (ST-LINK Virtual COM port)   |
+-----------------------------------------------------------------------+
```

### Key Technical Implementations

- **Peripheral Clock Enable (`RCC_AHB1ENR_CRCEN`)**  
  Activated the clock for the CRC hardware unit residing on the AHB1 bus architecture to power up the internal polynomial calculation registers.

- **Direct Register Mapping (`CRC_TypeDef`)**  
  Extended `stm32f401_registers.h` to incorporate the CRC base address (`0x40003000UL`), mapping out the Data Register (`DR`), Independent Data Register (`IDR`), and Control Register (`CR`).

- **Hardware Checksum Calculation (`CRC_Calculate`)**  
  Implemented a direct 32-bit word ingestion routine into `CRC->DR`, letting the hardware instantly evaluate data integrity using the standard IEEE 802.3 polynomial model.

- **Real-Time UART Debugging Infrastructure**  
  Integrated USART2 diagnostic logging at 115200 baud to track register states, clock activation status, and live hardware response metrics.

### Terminal Verification (PuTTY Output)

```text
==================================
   STM32F401 HARDWARE CRC INTRO
==================================
[INFO] CRC peripheral clock enabled.
[INFO] SPI1 initialized in Master Mode.
[INFO] Make sure PA7 (MOSI) is connected to PA6 (MISO) via jumper!
[TEST] Transmitting stream buffer via SPI1...
[TEST] Calculating Hardware CRC for word: 0x53544D33
[SUCCESS] Hardware CRC Engine Active & Responding!
========================================
System entering normal operation loop...
```

### Conclusion

Day 61 successfully integrates and validates the internal hardware CRC engine on bare-metal architecture. By leveraging the microcontroller's dedicated CRC hardware instead of software-based loops, the system achieves rapid checksum generation, laying a robust foundation for error-free SPI data transfers in upcoming milestones.