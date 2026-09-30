# Day 055: SPI Control Register 1 (CR1) Configuration & Clock Parameter Setup

### Objective

Configure the SPI1 Control Register 1 (`CR1`) on the STM32F401RE to establish the operational rhythm of serial communication—setting up Baud Rate prescalers, Clock Polarity (CPOL), Clock Phase (CPHA), and Master mode selection using clean, professional macro constants.

### System Architecture & Signaling Pipeline

```text
+-----------------------------------------------------------------------+
|                 SPI1 CR1 Configuration Pipeline                       |
+-----------------------------------------------------------------------+
                                   |
                                   v
+-----------------------------------------------------------------------+
|                 1. Peripheral Disable Protection (SPE = 0)            |
|  - Ensure SPI1 is safely disabled before tweaking control registers   |
+-----------------------------------------------------------------------+
                                   |
                                   v
+-----------------------------------------------------------------------+
|                 2. Parameter Calculation & Bitmasking                 |
|  - Apply Baud Rate prescaler options (e.g., PCLK/16 via BR[2:0])      |
|  - Configure Clock Polarity (CPOL) and Phase (CPHA) for timing sync   |
|  - Force Master Mode (MSTR) and Software Slave Management (SSM/SSI)   |
+-----------------------------------------------------------------------+
                                   |
                                   v
+-----------------------------------------------------------------------+
|                 3. Register Update & Peripheral Enable (SPE = 1)      |
|  - Commit configuration back to SPI1->CR1 and re-enable peripheral    |
|  - Stream successful configuration status log to PuTTY                |
+-----------------------------------------------------------------------+
```

### Key Technical Implementations

* **Modular Register Control (`CR1`)**  
  Implemented a dedicated configuration routine (`SPI1_Config`) inside `stm32f401_spi.c/.h` to safely manage bit-level modifications on the SPI control register without triggering hardware faults.

* **Self-Explanatory Constant Definitions**  
  Replaced magic numbers with clean macro parameters (`SPI_BAUDRATE_DIV16`, `SPI_CPOL_LOW`, `SPI_CPHA_1EDGE`) to ensure high code readability, hardware flexibility, and effortless bus parameter adjustments.

* **Master Mode & Software Slave Management**  
  Configured the STM32F401RE explicitly as an SPI Master (`MSTR = 1`) along with internal software slave management (`SSM = 1`, `SSI = 1`) to prevent mode faults on a single-master bus setup.

---

### Conclusion

Day 55 successfully programs the behavioral parameters of our high-speed serial interface. By abstracting raw bit shifts into clean, readable constant definitions, the firmware maintains strict separation of concerns, ensuring a robust and well-documented foundation for data transmission handling in the next phases.