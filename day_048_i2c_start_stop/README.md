# Day 048: I2C1 Bus Transaction Control (START & STOP Signaling)

### Objective

Implement bare-metal `I2C1_Start()` and `I2C1_Stop()` transaction control routines for the I2C1 peripheral on the STM32F401RE. This setup handles protocol-level bus framing by generating START conditions, polling hardware status flags (`SR1->SB`), and initiating STOP conditions to control master bus ownership without relying on external HAL/LL libraries.

### System Architecture & Signaling Pipeline

```text
+-----------------------------------------------------------------------+
|                       I2C Bus Framing Pipeline                        |
+-----------------------------------------------------------------------+
                                   |
                                   v
+-----------------------------------------------------------------------+
|                    1. START Condition Generation                      |
|  - Write 1 to I2C1->CR1 (START Bit)                                   |
|  - Hardware pulls SDA LOW while SCL is HIGH                           |
|  - Hardware sets SB (Start Bit) flag in I2C1->SR1                     |
+-----------------------------------------------------------------------+
                                   |
                                   v
+-----------------------------------------------------------------------+
|                      2. Hardware Flag Polling                         |
|  - Block-poll: while (!(I2C1->SR1 & I2C_SR1_SB))                      |
|  - Confirms START signal is successfully placed on bus                |
+-----------------------------------------------------------------------+
                                   |
                                   v
+-----------------------------------------------------------------------+
|                     3. STOP Condition Generation                      |
|  - Write 1 to I2C1->CR1 (STOP Bit)                                    |
|  - Hardware releases SDA HIGH while SCL is HIGH                       |
|  - Bus returns to IDLE state                                          |
+-----------------------------------------------------------------------+
```

### Key Technical Implementations

* **Hardware START Generation & Flag Synchronization**  
  Asserted the `START` bit in `I2C1->CR1` to initiate bus mastership. Implemented a blocking poll on the `SB` (Start Bit) flag in `I2C1->SR1` to synchronize software execution with physical hardware signaling before proceeding to subsequent address phases.

* **STOP Condition & Bus Teardown**  
  Asserted the `STOP` bit in `I2C1->CR1` to signal transaction termination. The hardware automatically releases the open-drain SDA and SCL lines back to pull-up high idle state upon execution.

* **Register Bit Mask Definition**  
  Defined the `I2C_SR1_SB` bit mask (`1UL << 0`) in `stm32f401_registers.h` to enable direct status checking against the primary I2C status register.

---

### Conclusion

Day 048 successfully completes the protocol-level framing routines for the I2C1 peripheral. By mastering START condition generation with flag polling and STOP condition teardown, the driver is now fully equipped to move into slave addressing and byte-level write operations in Day 049.