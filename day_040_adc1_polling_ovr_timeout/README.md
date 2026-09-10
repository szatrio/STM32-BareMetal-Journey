# Day 040: STM32F4 Bare-Metal ADC Overrun (OVR) & Timeout Guard Mechanism

### Objective
Enhance the bare-metal **ADC1 driver** from continuous acquisition to a production-grade, fault-tolerant sampling architecture. Implement robust error handling mechanisms including an **Overrun (OVR) Detection & Recovery Engine** (`ADC_SR_OVR`) using `ADC_CR2_EOCS` and a non-blocking **Timeout Guard Counter** (`ADC_TIMEOUT_LOOPS`) to prevent CPU lockups during hardware stalls. Verify hardware fault management on pin **PA0** by exposing simulation hooks for data overwrite and EOC polling timeout, ensuring autonomous recovery and continuous streaming of raw analog telemetry over UART.

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
|                 STM32F4 ADC1 Autonomous Engine & Guards               |
|  1. Enable Clock & SQR Selection: SQR3 (SQ1[4:0] = Channel 0)         |
|  2. Continuous & EOCS Setup: CR2 |= ADC_CR2_CONT | ADC_CR2_EOCS       |
|  3. Power ON & Single SWSTART: CR2 |= ADC_CR2_ADON | ADC_CR2_SWSTART  |
|  4. Hardware OVR Check: SR & ADC_SR_OVR -> Clear Bit & Auto-Restart   |
|  5. Guarded EOC Polling: Bounded loop against ADC_TIMEOUT_LOOPS       |
+-----------------------------------------------------------------------+
                                   |
                                   v
+-----------------------------------------------------------------------+
|                      Data Harvesting & Status Dispatcher              |
|  1. ADC_OK: Extract 12-Bit DR, Convert Voltage & Stream UART Log      |
|  2. ADC_ERROR_OVERRUN: Clear OVR Flag, Trigger Recovery & Log Warning |
|  3. ADC_ERROR_TIMEOUT: Re-arm Hardware Pacer & Log Stalled Fault      |
+-----------------------------------------------------------------------+
```

---

### Physical Circuit Setup & Breadboard Wiring 

To test and verify the ADC error handling mechanisms on physical hardware, a 10K Ohm potentiometer was interfaced using a standard breadboard and male-to-male jumper wires. 

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

* **End of Conversion Selection (`ADC_CR2_EOCS`)**  
  Explicitly enabling bit `EOCS` in `ADC_CR2` configures the hardware to assert the `EOC` (End of Conversion) flag after each individual regular conversion. Crucially, this setting allows hardware assertion of the `OVR` (Overrun) bit in `ADC1->SR` when new data overwrites unread DR register content.

* **Hardware Overrun Detection (`ADC_SR_OVR`)**  
  Triggered when a conversion completes before the CPU reads `ADC1->DR`. Evaluated at the start of `ADC1_ReadPollForConversion()` to intercept corrupted telemetry pipeline states.

* **OVR Hardware Recovery Sequence**  
  Clearing the `OVR` bit requires writing `0` to bit 5 of `ADC1->SR` via `ADC1_ClearOVR()`. Since an overrun condition halts continuous acquisition hardware, the application logic executes `ADC1_StartConversion()` upon catching `ADC_ERROR_OVERRUN` to resume continuous sampling.

* **Bounded EOC Polling (Timeout Guard)**  
  Replaces infinite blocking `while(!(ADC1->SR & ADC_SR_EOC))` polling with a bounded counter loop against `timeout_loops`. Returns `ADC_ERROR_TIMEOUT` if the counter expires, safeguarding the CPU core from lockups during clock or peripheral failures.

* **12-Bit Resolution Voltage Calculations**  
  Quantizes input signal range (0.0V–3.3V) into 4096 discrete steps ($2^{12}$). Telemetry voltage scaling formula:

  `Voltage = (ADC_RAW / 4095.0f) * 3.3f`

---

### Task Breakdown Architecture 

| Module / Component | Operation Type | Primary Action | Hardware / Register |
| :--- | :--- | :--- | :--- |
| **GPIO DRIVER** | Pin Initialization | Configures PA0 as `GPIO_MODE_ANALOG` without pull-up/down | `GPIOA->MODER` |
| **RCC DRIVER** | Clock Gating | Enables APB2 peripheral clock for ADC1 (`ADC1EN`) | `RCC->APB2ENR` (Bit 8) |
| **ADC DRIVER (INIT)** | Fault-Aware Setup | Configures `ADC_CR2_CONT` and `ADC_CR2_EOCS` for OVR handling | `ADC1->CR2`, `ADC1->SQR3` |
| **ADC DRIVER (POLL)** | Guarded Harvesting | Checks `ADC_SR_OVR`, polls `ADC_SR_EOC` with timeout, extracts `DR` | `ADC1->SR`, `ADC1->DR` |
| **ADC DRIVER (RECOVERY)**| Fault Clearing | Clears `OVR` flag in `SR` register and auto-re-arms `SWSTART` | `ADC1->SR`, `ADC1->CR2` |
| **TELEMETRY LOGIC** | Error & Data Logging | Processes `ADC_Status_t` enumeration and streams telemetry via UART | `USART2->DR`, `snprintf` |

---

### Hardware Verification & Telemetry Results 

* **Normal Autonomous Operation:** Verified non-stop 12-bit dynamic sampling across the full 0.00V–3.30V range during nominal execution. 
* **Overrun (OVR) Simulation & Recovery:** Injecting artificial delays (`Simple_Delay(10000)`) before register polling caused data loss. The driver successfully detected `ADC_SR_OVR`, invoked `ADC1_ClearOVR()`, re-triggered `ADC1_StartConversion()`, and restored normal acquisition without system crash. 
* **Timeout Protection Verification:** Simulating a polling threshold limit of 1 loop successfully triggered `ADC_ERROR_TIMEOUT`, prevented unbounded blocking, and allowed user telemetry logic to handle the stalled condition gracefully. 

* **UART Output Snapshot:** 

```text
=== STM32F401 Bare-Metal ADC1 Polling, OVR & Timeout Handling === 
[ADC1 CH0] Raw: 2048 | Voltage: 1.65 V 
[ADC1 CH0] Raw: 2050 | Voltage: 1.65 V 
[ADC WARN] Hardware Overrun (OVR) detected and recovered! 
[ADC1 CH0] Raw: 2045 | Voltage: 1.64 V 
[ADC ERROR] Conversion timed out! Hardware stalled. 
[ADC1 CH0] Raw: 2048 | Voltage: 1.65 V 
```

---

### Conclusion

Day 040 successfully upgrades the bare-metal ADC driver into a fault-tolerant, production-ready peripheral interface. By leveraging `ADC_CR2_EOCS` alongside `ADC_SR_OVR` clearing and a deterministic timeout guard loop, the driver effectively mitigates memory overwrite loss and CPU lockup conditions, ensuring robust long-term reliability for embedded real-time systems.