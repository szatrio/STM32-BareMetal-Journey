# Day 038: Bare-Metal STM32F4 ADC1 Single Conversion Driver & Hardware Telemetry

### Objective
Design and implement a modular, register-level ADC1 (Analog-to-Digital Converter) driver in Single Conversion Mode using software triggering (SWSTART) and polling-based synchronization (EOC). Disable digital Schmitt triggers on the target analog pin (PA0) by configuring it to Analog Mode to prevent digital noise leakage. Interfaced the physical setup with an external 10K Potentiometer on a breadboard to acquire real-time variable voltage signals (0.00 V to 3.29 V / 0 to 4095 raw 12-bit counts) and log high-precision telemetry over UART using a lightweight floating-point string formatter.

---

### System Architecture & Hardware Execution Pipeline

```text
+-----------------------------------------------------------------------+
|                      External Hardware Domain                         |
|  10K Mono Potentiometer Wiring (VCC: 3.3V, GND: GND, Wiper: PA0)      |
+-----------------------------------------------------------------------+
                                   | (Analog Voltage: 0.0V - 3.3V)
                                   v
+-----------------------------------------------------------------------+
|                     GPIO Port A Domain (PA0 / IN0)                    |
|  GPIO_MODE_ANALOG (MODER = 0b11) Disables Digital Input Buffer        |
|  Eliminates Schmitt Trigger Power Leakage & Digital Switching Noise   |
+-----------------------------------------------------------------------+
                                   |
                                   v
+-----------------------------------------------------------------------+
|                         STM32F4 ADC1 Peripheral                       |
|  1. Enable Clock: RCC->APB2ENR |= RCC_APB2ENR_ADC1EN                  |
|  2. Select Channel: SQR3 (SQ1[4:0] = Channel 0)                       |
|  3. Power ON: CR2 |= ADC_CR2_ADON                                     |
|  4. Trigger Conversion: CR2 |= ADC_CR2_SWSTART                        |
+-----------------------------------------------------------------------+
                                   |
                                   v
+-----------------------------------------------------------------------+
|                      Data Acquisition & Telemetry                     |
|  1. Polling Wait: while (!(ADC1->SR & ADC_SR_EOC));                   |
|  2. Read 12-bit Result: uint16_t raw = ADC1->DR & 0xFFF;              |
|  3. Float Conversion: Voltage = (raw / 4095.0f) * 3.3f                |
|  4. Low-Overhead Formatting & USART2 Transmission @ 115200 Baud       |
+-----------------------------------------------------------------------+
```

### Physical Circuit Setup & Breadboard Wiring

To test and verify the ADC driver on physical hardware, a 10K Ohm potentiometer was interfaced using a standard breadboard and male-to-male jumper wires.

#### Hardware Components Used
- **MCU Board:** STM32F410RE / STM32F401RE Nucleo-64
- **Input Sensor:** 10K Mono Potentiometer
- **Prototyping:** Half-Size Breadboard
- **Interconnects:** 3x Male-to-Male Jumper Wires

#### Wiring Schematic Table

| Potentiometer Pin | Breadboard Placement | STM32 Nucleo Connection | Function / Signal Type |
| :--- | :--- | :--- | :--- |
| **Left Pin (Pin 1)** | Row 10 (e.g., A10) | **GND** | Ground Reference (0V) |
| **Middle Pin (Pin 2 / Wiper)** | Row 11 (e.g., A11) | **PA0** | Analog Signal Output (`ADC1_IN0`) |
| **Right Pin (Pin 3)** | Row 12 (e.g., A12) | **3V3** | Supply Power (+3.3V) |

---
### Step-by-Step Hardware Assembly & Physical Connections

1   . **Breadboard Mounting**
   Placed the 3 pins of the 10K Potentiometer across three separate consecutive rows on the breadboard (Row 10, Row 11, and Row 12) to avoid short-circuiting adjacent pins.

2   . **Ground Reference Connection**
   Inserted a jumper wire into Row 10 (sharing the same node as Potentiometer Pin 1) and connected the opposite end to the **GND** header on the Nucleo board.

3   . **Power Supply Connection**
   Inserted a jumper wire into Row 12 (sharing the same node as Potentiometer Pin 3) and connected the opposite end to the **3V3** power rail header on the Nucleo board.

4   . **Analog Signal Channel Line**
   Inserted the signal jumper wire into Row 11 (connected to the wiper/middle pin) and plugged it directly into pin **PA0** (`ADC1_IN0`).

5   . **PC Interface & Power-Up**
   Connected the Nucleo board to the host PC via USB cable, powering the potentiometer voltage divider circuit and establishing ST-LINK debugging capability.

---

### Key Technical Concepts

* **Analog GPIO Configuration (`GPIO_MODE_ANALOG`)**
  * Configuring a GPIO pin as analog sets its `MODER` bits to `0b11`. This completely isolates the digital input logic (disables the Schmitt trigger) to eliminate parasitic current consumption and prevent high-frequency digital noise from interfering with analog conversions.

* **Sequence Mapping (`ADC_SQR3` Register)**
  * `SQR3` defines the conversion order for the regular group. For single-channel acquisition, the channel number (e.g., `0` for PA0) is written into `SQ1[4:0]` (Bits 4:0).

* **Power Control vs. Triggering (`ADON` vs. `SWSTART`)**
  * Setting the `ADON` bit in `ADC_CR2` wakes up the ADC peripheral from low-power mode. Setting the `SWSTART` bit initiates the actual conversion sequence for regular channels.

* **Hardware Clearing of `EOC` Flag**
  * The End of Conversion (`EOC`) flag in `ADC_SR` indicates data availability. Reading the data register `ADC_DR` automatically clears the `EOC` flag in hardware, simplifying software state management.

* **12-Bit Resolution Voltage Calculations**
  * The SAR ADC yields a digital quantization range of **0 to 4095** (2^12 - 1). The dynamic voltage level is derived using floating-point hardware scaling:

  `Voltage = (ADC_RAW / 4095.0f) * 3.3f`

---

### Task Breakdown Architecture

| Module / Component | Operation Type | Primary Action | Hardware / Register |
| :--- | :--- | :--- | :--- |
| **GPIO DRIVER** | Pin Initialization | Configures PA0 as `GPIO_MODE_ANALOG` without pull-up/down | `GPIOA->MODER` |
| **RCC DRIVER** | Clock Gating | Enables APB2 peripheral clock for ADC1 (`ADC1EN`) | `RCC->APB2ENR` (Bit 8) |
| **ADC DRIVER (INIT)** | Peripheral Setup | Maps Channel 0 to sequence slot `SQ1` and powers on ADC | `ADC1->SQR3`, `ADC1->CR2` (`ADON`) |
| **ADC DRIVER (READ)** | Conversion Execution | Triggers `SWSTART`, polls `EOC` flag, and reads raw output | `ADC1->CR2`, `ADC1->SR`, `ADC1->DR` |
| **TELEMETRY LOGIC** | Voltage Formatting | Converts raw 12-bit data to float voltage and formats string | `USART2->DR`, `snprintf` |

---

### Hardware Verification & Telemetry Results

* **Linear Response Verification:** Rotating the potentiometer shaft provided smooth, monotonic transitions across the entire 12-bit range (**0 to 4095**).

* **Stable Upper & Lower Boundaries:**
  * **Fully CCW (GND):** Raw count = `1` -> `0.00 V`
  * **Mid-Position:** Raw count = `2542` -> `2.04 V`
  * **Fully CW (3.3V Rail):** Raw count = `4095` -> `3.29 V`

* **UART Output Snapshot:**

```text
=== Day 038: STM32F401 Bare-Metal ADC1 Single Conversion Started ===
[ADC1 CH0] Raw:    1 | Voltage: 0.00 V
[ADC1 CH0] Raw:  624 | Voltage: 0.50 V
[ADC1 CH0] Raw: 1593 | Voltage: 1.28 V
[ADC1 CH0] Raw: 2542 | Voltage: 2.04 V
[ADC1 CH0] Raw: 3375 | Voltage: 2.71 V
[ADC1 CH0] Raw: 4091 | Voltage: 3.29 V
[ADC1 CH0] Raw: 4095 | Voltage: 3.29 V

```
---

### Conclusion

Day 038 successfully establishes a modular, register-level ADC1 driver running on physical bare-metal hardware. By correctly isolating pin PA0 in Analog Mode, configuring SQR3/CR2 registers, and executing polling-based data acquisition, precise real-time analog signal monitoring was achieved and verified via UART telemetry.