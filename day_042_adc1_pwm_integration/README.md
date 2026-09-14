# Day 042: STM32F4 Timer 2 PWM Architecture & ADC-to-PWM Closed-Loop Telemetry Integration

### Objective
Integrate the bare-metal **ADC1 driver** (potentiometer input on PA0) with a newly refactored, multi-channel scalable **Timer 2 (TIM2) PWM driver** (output on PA1 / Channel 2) for the STM32F401. Implement dynamic duty cycle scaling, resolve hardware alternate function mapping requirements (`GPIO_SetAltFunction` via AF1), and verify closed-loop control via real-time UART telemetry.

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
|                  Data Extraction & Scaling Engine                     |
|  1. Re-aligned 12-Bit Reconstruction: (DR >> 4) & 0x0FFF              |
|  2. Dynamic Voltage Mapping: (12Bit_Value / 4095.0f) * 3.3f           |
|  3. Duty Cycle Mapping: (12Bit_Value / 4095.0f) * 100.0f              |
|  4. OVR Guard & Auto-Recovery: Clear OVR & SWSTART on Delay Stalls    |
+-----------------------------------------------------------------------+
                                   |
                                   v
+-----------------------------------------------------------------------+
|                   TIM2 PWM Output Domain (PA1 / CH2)                  |
|  1. Enable APB1 Clock: RCC_APB1ENR |= RCC_APB1ENR_TIM2EN              |
|  2. Timebase Load: PSC = prescaler - 1, ARR = auto_reload - 1         |
|  3. Mode Selection: CCMR1 configured to PWM Mode 1 (OC2M = 110)       |
|  4. Alternate Function Mapping: GPIO_SetAltFunction(GPIOA, 1, AF1)    |
|  5. Hardware Duty Update: CCR2 = (percent * ARR) / 100.0f             |
+-----------------------------------------------------------------------+
```

### Physical Circuit Setup & Breadboard Wiring

#### Hardware Components Used
- **MCU Board:** STM32F401RE Nucleo-64
- **Input Sensor:** 10K Mono Potentiometer
- **Output Actuator:** LED with 330Ω Current-Limiting Resistor
- **Prototyping:** Half-Size Breadboard
- **Interconnects:** Male-to-Male Jumper Wires

#### Wiring Schematic Table

| Component / Pin | STM32 Nucleo Connection | Function / Signal Type |
| :--- | :--- | :--- |
| **Potentiometer Left Pin (1)** | **GND** | Ground Reference (0V) |
| **Potentiometer Middle Pin (2)** | **PA0** | Analog Input (`ADC1_IN0`) |
| **Potentiometer Right Pin (3)** | **3V3** | Supply Power (+3.3V) |
| **LED Anode (+) via 330Ω** | **PA1** | PWM Output (`TIM2_CH2`) |
| **LED Cathode (-)** | **GND** | Ground Reference (0V) |

---

### Key Technical Insights & Troubleshooting

* **Alternate Function Mapping Requirement (`GPIO_SetAltFunction`)**  
  Configuring a pin via `GPIO_Init` with `GPIO_MODE_ALT` sets the hardware mode bits in `MODER`, but leaves the alternate function selection registers (`AFR[0]` and `AFR[1]`) at `0x00` (AF0). Explicitly mapping pin 1 to **AF1** (`GPIO_SetAltFunction(GPIOA, 1, 1);`) connects the internal TIM2_CH2 output to physical pin PA1. Without this explicit mapping, timer signals remain disconnected internally.

* **Hardware Diagnostic Workflow (GPIO Health Check)**  
  When troubleshooting output issues, isolating physical connections via a temporary digital output blink test (`GPIO_MODE_OUTPUT`) confirms whether the wiring, breadboard layout, and LED polarity are fully functional before validating complex timer peripheral channels.

* **PWM Frequency & Resolution Calculation**  
  With a timer clock source of 16 MHz, a prescaler of `16` (`PSC = 15`), and an auto-reload register value of `1000` (`ARR = 999`), the output PWM frequency is established precisely at $1\,\text{kHz}$, offering 1000 discrete duty cycle steps for smooth LED dimming performance.

---


### Hardware Verification & Telemetry Output

Testing was performed by sweeping the 10K potentiometer wiper connected to **PA0** to drive the LED connected to **PA1** via Timer 2 Channel 2 PWM, while streaming real-time diagnostic values over USART2.

* **UART Output Snapshot (PuTTY Telemetry):**

```text
[ADC:  669 | 0.53V] ---> [PWM Duty: 16.33%]
[ADC:  669 | 0.53V] ---> [PWM Duty: 16.33%]
[ADC:  664 | 0.53V] ---> [PWM Duty: 16.21%]
[ADC:  669 | 0.53V] ---> [PWM Duty: 16.33%]
[ADC:  668 | 0.53V] ---> [PWM Duty: 16.31%]
[ADC:  662 | 0.53V] ---> [PWM Duty: 16.16%]
[ADC:  669 | 0.53V] ---> [PWM Duty: 16.33%]
[ADC:  669 | 0.53V] ---> [PWM Duty: 16.33%]
[ADC:  669 | 0.53V] ---> [PWM Duty: 16.33%]
[ADC:  669 | 0.53V] ---> [PWM Duty: 16.33%]
[ADC:  669 | 0.53V] ---> [PWM Duty: 16.33%]
[ADC:  669 | 0.53V] ---> [PWM Duty: 16.33%]
[ADC:  668 | 0.53V] ---> [PWM Duty: 16.31%]
[ADC:  669 | 0.53V] ---> [PWM Duty: 16.33%]
[ADC:  667 | 0.53V] ---> [PWM Duty: 16.28%]
```

### Conclusion

Day 042 successfully establishes closed-loop ADC-to-PWM control on the STM32F401. By combining a modular multi-channel timer architecture with correct alternate function routing (`AF1`), potentiometer adjustments translate seamlessly into real-time hardware LED brightness variations alongside continuous UART telemetry logging.