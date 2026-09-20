# Day 047: Bare-Metal I2C1 Register Setup & Clock Calibration

### Objective

Establish the core hardware foundation for the I2C1 peripheral on the STM32F401RE through direct register configuration. This setup configures GPIOB (PB6 for SCL, PB7 for SDA) in Open-Drain Alternate Function 4 (AF4) mode, calculates exact timing prescalers for 100 kHz Standard Mode operation, and prepares the I2C control registers (`CR1`, `CR2`, `CCR`, `TRISE`, `OAR1`) without relying on vendor HAL/LL libraries or active transaction loops.

### System Architecture & Hardware Setup Pipeline

```text
+-----------------------------------------------------------------------+
|                    System Clock & Bus Infrastructure                  |
|          HSI Clock (16 MHz) ---> APB1 Peripheral Bus (PCLK1)          |
+-----------------------------------------------------------------------+
                                   |
                                   v
+-----------------------------------------------------------------------+
|                      I2C1 Peripheral Configuration                    |
|  1. Enable Clock: RCC_AHB1ENR (GPIOB) & RCC_APB1ENR (I2C1)            |
|  2. Set FREQ: I2C1->CR2 = 16 (16 MHz Peripheral Clock)                |
|  3. Set Timing: I2C1->CCR = 80 (100 kHz Standard Speed)               |
|  4. Set Rise Time: I2C1->TRISE = 17 (Max 1000ns Rise Time)            |
|  5. Enable Peripheral: Set PE bit in I2C1->CR1                        |
+-----------------------------------------------------------------------+
                                   |
                                   v
+-----------------------------------------------------------------------+
|                       Physical GPIO Matrix (AF4)                      |
|  PB6 (I2C1_SCL) ---> Open-Drain + Internal Pull-Up + High Speed       |
|  PB7 (I2C1_SDA) ---> Open-Drain + Internal Pull-Up + High Speed       |
+-----------------------------------------------------------------------+
```
### Key Technical Implementations

* **Datasheet-Verified Pin Multiplexing (AF4)**  
  Mapped PB6 and PB7 to Alternate Function 4 (`AF04`) as verified in the STM32F401RE Datasheet pinout definitions. Pins were explicitly configured in **Open-Drain** mode to satisfy the wired-AND requirement of the I2C bus protocol.

* **Precise Clock Control Calculation (`CCR` & `TRISE`)**  
  Configured for 100 kHz Standard Mode using the 16 MHz HSI clock on the APB1 bus:  
  `CCR = FPCLK1 / (2 * FI2C) = 16,000,000 / (2 * 100,000) = 80`  
  `TRISE = (1000 ns / TPCLK1) + 1 = (1000 ns / 62.5 ns) + 1 = 17`

* **Isolated Hardware Initialization**  
  Ensured `I2C1->CR1` Peripheral Enable (`PE`) bit is cleared during setup to prevent latching invalid parameters, guaranteeing safe register updates prior to enabling the bus.

---

### Conclusion

Day 047 successfully establishes the register-level initialization routine for the I2C1 peripheral. By properly configuring the GPIO matrix, peripheral clock frequency, timing divisors, and control bits, the microcontroller hardware is now fully prepared to generate bus condition flags.