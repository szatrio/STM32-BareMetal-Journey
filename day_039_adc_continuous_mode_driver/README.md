# Day 039: Bare-Metal STM32F4 ADC1 Continuous Mode Driver & Autonomous Data Acquisition

### Objective
Migrate the bare-metal **ADC1 driver** from Single Conversion Polling Mode to **Continuous Conversion Mode** (`ADC_CR2_CONT`). Refactor hardware excitation logic by decoupling software triggering (`SWSTART`) from the main execution loop, transforming the ADC into an autonomous background hardware engine. Maintain hardware signal integrity on pin **PA0** in Analog Mode with an external **10K Potentiometer** dynamic voltage divider, validating continuous hardware register updates (`ADC_DR`) and streaming high-precision telemetry over UART using lightweight float formatting without CPU conversion overhead.

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
|                      STM32F4 ADC1 Autonomous Engine                   |
|  1. Enable Clock: RCC->APB2ENR |= RCC_APB2ENR_ADC1EN                  |
|  2. Select Channel: SQR3 (SQ1[4:0] = Channel 0)                       |
|  3. Enable Continuous Mode: CR2 |= ADC_CR2_CONT                       |
|  4. Power ON: CR2 |= ADC_CR2_ADON                                     |
|  5. Single-Shot Software Start Trigger: CR2 |= ADC_CR2_SWSTART        |
|     (Executes ONCE before main loop; ADC loops indefinitely in HW)    |
+-----------------------------------------------------------------------+
                                   |
                                   v
+-----------------------------------------------------------------------+
|                      Data Harvesting & Telemetry                      |
|  1. Read Latest Sample: uint16_t raw = ADC1->DR & 0xFFF;              |
|  2. Hardware Auto-Clears EOC Flag & Automatically Starts Next Conversion|
|  3. Float Conversion: Voltage = (raw / 4095.0f) * 3.3f                |
|  4. Low-Overhead Formatting & USART2 Transmission @ 115200 Baud       |
+-----------------------------------------------------------------------+
```

### Physical Circuit Setup & Breadboard Wiring 

To test and verify the continuous ADC driver on physical hardware, a 10K Ohm potentiometer was interfaced using a standard breadboard and male-to-male jumper wires. 

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

1. **Breadboard Mounting**  
   Placed the 3 pins of the 10K Potentiometer across three separate consecutive rows on the breadboard (Row 10, Row 11, and Row 12) to avoid short-circuiting adjacent pins. 

2. **Ground Reference Connection**  
   Inserted a jumper wire into Row 10 (sharing the same node as Potentiometer Pin 1) and connected the opposite end to the **GND** header on the Nucleo board. 

3. **Power Supply Connection**  
   Inserted a jumper wire into Row 12 (sharing the same node as Potentiometer Pin 3) and connected the opposite end to the **3V3** power rail header on the Nucleo board. 

4. **Analog Signal Channel Line**  
   Inserted the signal jumper wire into Row 11 (connected to the wiper/middle pin) and plugged it directly into pin **PA0** (`ADC1_IN0`). 

5. **PC Interface & Power-Up**  
   Connected the Nucleo board to the host PC via USB cable, powering the potentiometer voltage divider circuit and establishing ST-LINK debugging capability. 

---

### Key Technical Concepts 

* **Continuous Conversion Mode (`ADC_CR2_CONT`)**  
  Setting bit 1 (`CONT`) in `ADC_CR2` instructs the ADC peripheral to restart a new conversion cycle immediately after finishing the current one. Unlike Single Mode, CPU re-triggering is completely eliminated. 

* **Single-Shot Trigger Architecture (`ADC1_StartConversion`)**  
  Software trigger `SWSTART` (Bit 30 in `ADC_CR2`) is written exactly once during system initialization. The ADC peripheral runs continuously in the background on hardware autopilot. 

* **Non-Blocking DR Harvesting**  
  The CPU accesses `ADC1->DR` asynchronously inside the application loop. Reading `ADC_DR` extracts the latest 12-bit conversion value while the ADC pipeline concurrently processes the next analog sample. 

* **Zero-Overhead Hardware Pipeline**  
  Disabling repetitive software trigger injection removes software state machine delays, lowering CPU execution cycles while maintaining maximum signal sampling throughput. 

* **12-Bit Resolution Voltage Calculations**  
  The SAR ADC yields a digital quantization range of **0 to 4095** ($2^{12} - 1$). The dynamic voltage level is derived using floating-point hardware scaling: 

  `Voltage = (ADC_RAW / 4095.0f) * 3.3f`

---

### Task Breakdown Architecture 

| Module / Component | Operation Type | Primary Action | Hardware / Register |
| :--- | :--- | :--- | :--- |
| **GPIO DRIVER** | Pin Initialization | Configures PA0 as `GPIO_MODE_ANALOG` without pull-up/down | `GPIOA->MODER` |
| **RCC DRIVER** | Clock Gating | Enables APB2 peripheral clock for ADC1 (`ADC1EN`) | `RCC->APB2ENR` (Bit 8) |
| **ADC DRIVER (INIT)** | Continuous Setup | Sets `ADC_CR2_CONT` bit, maps Channel 0 to `SQ1`, and powers ON ADC | `ADC1->CR2`, `ADC1->SQR3` |
| **ADC DRIVER (START)** | One-Time Trigger | Asserts `SWSTART` bit once before main execution loop | `ADC1->CR2` (`SWSTART`) |
| **ADC DRIVER (READ)** | Data Harvesting | Polls `EOC` flag for latest hardware cycle and reads `ADC1->DR` | `ADC1->SR`, `ADC1->DR` |
| **TELEMETRY LOGIC** | Voltage Formatting | Converts raw 12-bit data to float voltage and formats string | `USART2->DR`, `snprintf` |

---

### Hardware Verification & Telemetry Results 

* **Autonomous Continuous Operation:** Verified that removing `SWSTART` re-triggers from `while(1)` maintains real-time, non-stop analog conversions driven purely by hardware. 
* **Linear Response Verification:** Rotating the potentiometer shaft provided smooth, monotonic transitions across the entire 12-bit range (**0 to 4095**). 
* **Stable Upper & Lower Boundaries:** 
  * **Fully CCW (GND):** Raw count = `1` -> `0.00 V` 
  * **Mid-Position:** Raw count = `2542` -> `2.04 V` 
  * **Fully CW (3.3V Rail):** Raw count = `4095` -> `3.29 V` 

* **UART Output Snapshot:** 

```text

=== Day 039: STM32F401 Bare-Metal ADC1 Continuous Mode Started === 
[ADC1 CH0] Raw:    1 | Voltage: 0.00 V 
[ADC1 CH0] Raw:  624 | Voltage: 0.50 V 
[ADC1 CH0] Raw: 1593 | Voltage: 1.28 V 
[ADC1 CH0] Raw: 2542 | Voltage: 2.04 V 
[ADC1 CH0] Raw: 3375 | Voltage: 2.71 V 
[ADC1 CH0] Raw: 4091 | Voltage: 3.29 V 
[ADC1 CH0] Raw: 4095 | Voltage: 3.29 V

```

### Conclusion

Day 039 successfully transitions the bare-metal ADC driver into an autonomous Continuous Conversion Mode. By configuring `ADC_CR2_CONT` and executing a single-shot `SWSTART` trigger, software polling overhead was significantly reduced, establishing an efficient background hardware acquisition pipeline verified via real-time UART telemetry.