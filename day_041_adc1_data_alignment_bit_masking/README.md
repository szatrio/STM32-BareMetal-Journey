# Day 041: STM32F4 Bare-Metal ADC Data Alignment & Fast Bit Masking Architecture

### Objective
Implement register-level **Data Alignment control (`ADC_CR2_ALIGN`)** on the bare-metal **ADC1 driver** for the STM32F401, comparing **Left Alignment** (`ADC_ALIGN_LEFT`) and **Right Alignment** (`ADC_ALIGN_RIGHT`). Explore the architectural advantages of Left Alignment for zero-CPU-overhead 8-bit MSB extraction (`ADC1_Read8BitFast()`) alongside standard 12-bit voltage reconstruction. Ensure continuous streaming telemetry over USART2 with hardware **Overrun (OVR) recovery** and atomic `ADC_CR2` configuration sequence handling.

---

### System Architecture & Execution Pipeline

```text
+-----------------------------------------------------------------------+
|                      External Hardware Domain                         |
|  10K Mono Potentiometer Wiring (VCC: 3.3V, GND: GND, Wiper: PA0)      |
+-----------------------------------------------------------------------+
                                   | (Analog Input: 0.0V - 3.3V)
                                   v
+-----------------------------------------------------------------------+
|                     GPIO Port A Domain (PA0 / IN0)                    |
|  GPIO_MODE_ANALOG (MODER = 0b11) Disables Digital Schmitt Trigger     |
|  Eliminates Switching Noise & Analog Input Leakage Currents           |
+-----------------------------------------------------------------------+
                                   |
                                   v
+-----------------------------------------------------------------------+
|             STM32F4 ADC1 Peripheral Engine & CR2 Setup                |
|  1. Enable Clock: RCC_APB2ENR |= RCC_APB2ENR_ADC1EN                   |
|  2. Select Channel 0: SQR3 (SQ1[4:0] = 0)                             |
|  3. Power ON ADC: CR2 |= ADC_CR2_ADON + t_STAB Wakeup Delay           |
|  4. CR2 Alignment Setup: Set/Clear ALIGN (Bit 11)                     |
|  5. Mode Configuration: CR2 |= (ADC_CR2_CONT | ADC_CR2_EOCS)          |
|  6. SWSTART Trigger: CR2 |= ADC_CR2_SWSTART                           |
+-----------------------------------------------------------------------+
                                   |
                                   v
+-----------------------------------------------------------------------+
|                   Data Extraction & Bit Masking Engine                |
|  1. Fast 8-Bit Extraction: High Byte Read -> (DR >> 8) [Zero Div]     |
|  2. Re-aligned 12-Bit Reconstruction: (DR >> 4) & 0x0FFF              |
|  3. Dynamic Voltage Mapping: (12Bit_Value / 4095.0f) * 3.3f           |
|  4. OVR Guard & Auto-Recovery: Clear OVR & SWSTART on Delay Stalls    |
+-----------------------------------------------------------------------+
```

### Data Alignment Mechanics (16-Bit DR Register Mapping)

The STM32F4 ADC hardware features a 12-bit successive-approximation ADC. Results are stored in the 16-bit **ADC Data Register (`ADC_DR`)**. The positioning of the 12-bit conversion result inside `ADC_DR[15:0]` is controlled by the `ALIGN` bit in `ADC_CR2`.

#### 1. Right Alignment (`ADC_ALIGN_RIGHT` / `ALIGN = 0`)
The 12-bit conversion result occupies bits `[11:0]`. Bits `[15:12]` are padded with zeros.

| Bit | 15 | 14 | 13 | 12 | 11 | 10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| **DR** | 0 | 0 | 0 | 0 | D11 | D10 | D9 | D8 | D7 | D6 | D5 | D4 | D3 | D2 | D1 | D0 |

- **Use Case:** Standard high-precision 12-bit integer arithmetic (0–4095).
- **Extraction:** Mask directly using `DR & 0x0FFF`.

#### 2. Left Alignment (`ADC_ALIGN_LEFT` / `ALIGN = 1`)
The 12-bit conversion result is shifted left to occupy bits `[15:4]`. Lower bits `[3:0]` are padded with zeros.

| Bit | 15 | 14 | 13 | 12 | 11 | 10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| **DR** | D11 | D10 | D9 | D8 | D7 | D6 | D5 | D4 | D3 | D2 | D1 | D0 | 0 | 0 | 0 | 0 |

- **Use Case 1 (Fast 8-bit):** Shift right by 8 (`DR >> 8`) to extract the top 8 MSB bits (`D11..D4`) into a `uint8_t` (range 0–255). Avoids costly bitwise shift and division operations for fast DSP, PWM duty cycle updates, or display rendering.
- **Use Case 2 (Realigned 12-bit):** Shift right by 4 and mask with `0x0FFF` (`(DR >> 4) & 0x0FFF`) to retrieve the exact 12-bit value (range 0–4095).

---

### Physical Circuit Setup & Breadboard Wiring

#### Hardware Components Used
- **MCU Board:** STM32F401RE Nucleo-64
- **Input Sensor:** 10K Mono Potentiometer
- **Prototyping:** Half-Size Breadboard
- **Interconnects:** 3x Male-to-Male Jumper Wires

#### Wiring Schematic Table

| Potentiometer Pin | Breadboard Placement | STM32 Nucleo Connection | Function / Signal Type |
| :--- | :--- | :--- | :--- |
| **Left Pin (Pin 1)** | Row 10 | **GND** | Ground Reference (0V) |
| **Middle Pin (Pin 2 / Wiper)** | Row 11 | **PA0** | Analog Output (`ADC1_IN0`) |
| **Right Pin (Pin 3)** | Row 12 | **3V3** | Supply Power (+3.3V) |

---

### Key Technical Concepts & Fixes

* **Power-On Stabilization Delay ($t_{STAB}$)**  
  When toggling `ADON` from `0` to `1` in `ADC_CR2`, the internal analog circuitry requires a startup stabilization delay ($t_{STAB} \approx 3\text{--}10\,\mu\text{s}$). Enabling `ADON`, `CONT`, and `ALIGN` simultaneously in a single bitwise write can cause the ADC continuous engine to fail or perform only a single conversion. Resolving this requires setting `ADON` first, waiting via a `__NOP()` loop, and then configuring the operational parameters.

* **Atomic CR2 Register Operations**  
  Modifying register flags (e.g., clearing/setting `ADC_CR2_ALIGN`) using separate read-modify-write statements can inadvertently corrupt hardware flags if performed while `ADON` is active. Using a local temporary variable (`cr2_temp`) prepares the full register state before committing a single write to `ADC1->CR2`.

* **Continuous Overrun (OVR) Recovery in Polled Loops**  
  In continuous conversion mode, the ADC hardware converts samples continuously in the background. Adding a loop delay (e.g., `SysTick_DelayMs(200)`) causes unread conversion data to trigger the Overrun flag (`ADC_SR_OVR`), halting further conversions. Adding an inline check to clear `OVR` and re-trigger `SWSTART` ensures continuous telemetry streams without stalling.

---

### Task Breakdown Architecture

| Module / Component | Operation Type | Primary Action | Hardware / Register |
| :--- | :--- | :--- | :--- |
| **GPIO DRIVER** | Pin Initialization | Configures PA0 as `GPIO_MODE_ANALOG` | `GPIOA->MODER` |
| **RCC DRIVER** | Peripheral Clock | Enables APB2 clock for ADC1 (`ADC1EN`) | `RCC->APB2ENR` |
| **ADC DRIVER** | Power-On Sequence | Sets `ADON`, executes $t_{STAB}$ delay | `ADC1->CR2` |
| **ADC DRIVER** | Alignment Control | Configures Left/Right alignment via `cr2_temp` | `ADC1->CR2` (Bit 11) |
| **ADC DRIVER** | Fast 8-Bit Read | Returns `(uint8_t)(DR >> 8)` for Left Alignment | `ADC1->DR` |
| **MAIN APP** | OVR Guard & Print | Clears `OVR`, reconstructs 12-bit & voltage, streams UART | `ADC1->SR`, `USART2->DR` |

---

### Hardware Verification & Telemetry Output

Testing was performed by sweeping the 10K potentiometer wiper connected to **PA0** while operating in **Left Alignment Mode (`ADC_ALIGN_LEFT`)**.

* **UART Output Snapshot (PuTTY Telemetry):**

```text
=== STM32F4 ADC DATA ALIGNMENT & BIT MASKING ===
[DR: 0x98A0] | 12-Bit: 2442 (1.96 V) | 8-Bit Fast: 152
[DR: 0xA370] | 12-Bit: 2615 (2.10 V) | 8-Bit Fast: 163
[DR: 0xAB10] | 12-Bit: 2737 (2.20 V) | 8-Bit Fast: 171
[DR: 0xB3B0] | 12-Bit: 2875 (2.31 V) | 8-Bit Fast: 179
[DR: 0xBBB0] | 12-Bit: 3003 (2.42 V) | 8-Bit Fast: 187
[DR: 0xC270] | 12-Bit: 3111 (2.50 V) | 8-Bit Fast: 194
[DR: 0xC830] | 12-Bit: 3203 (2.58 V) | 8-Bit Fast: 200
[DR: 0xCE30] | 12-Bit: 3299 (2.65 V) | 8-Bit Fast: 206
[DR: 0xD4B0] | 12-Bit: 3403 (2.74 V) | 8-Bit Fast: 212
[DR: 0xDA70] | 12-Bit: 3495 (2.81 V) | 8-Bit Fast: 218
[DR: 0xDF70] | 12-Bit: 3575 (2.88 V) | 8-Bit Fast: 223
[DR: 0xE010] | 12-Bit: 3585 (2.88 V) | 8-Bit Fast: 224
```

**Data Verification Example (`DR = 0xE010`):**
  - **Raw 16-Bit Register:** `0xE010`
  - **Fast 8-Bit Extract (`0xE010 >> 8`):** `0xE0` = `224` (matches output)
  - **Realigned 12-Bit Extract (`(0xE010 >> 4) & 0x0FFF`):** `0x0E01` = `3585`
  - **Voltage Calculation:** $(3585 / 4095.0) \times 3.3\text{V} = 2.889\text{V} \approx 2.88\text{V}$

---

### Conclusion

Day 041 completes the bare-metal alignment and bit-masking architecture for the STM32F4 ADC driver. By mastering Left Alignment mechanics, zero-overhead 8-bit sampling is achieved for resource-constrained DSP routines, while maintaining full 12-bit precision through explicit bit manipulation. Hardware-level $t_{STAB}$ initialization and active `OVR` recovery guarantee continuous telemetry streaming across arbitrary polling interval delays.