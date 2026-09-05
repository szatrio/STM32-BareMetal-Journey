# Day 036: Bare-Metal TIM2 Hardware PWM Generation via CCMR1 & CCER Architecture

### Objective
Configure the **TIM2 General-Purpose Timer** on the STM32F401RE for **Hardware Pulse Width Modulation (PWM) Generation** on Channel 1 (`TIM2_CH1` via PA5). Manipulate the **Capture/Compare Mode Register 1 (`CCMR1`)** and **Capture/Compare Enable Register (`CCER`)** at the bare-metal register level to generate a **1 kHz PWM wave**. Dynamically modulate the duty cycle inside the foreground loop to achieve a smooth, zero-CPU-overhead **breathing LED effect**.

---

### System Architecture & PWM Waveform Logic

```text
+-----------------------------------------------------------------------+
|                         STM32F401 Clock Tree                          |
|                       HSI Clock Source (16 MHz)                       |
+-----------------------------------------------------------------------+
                                   |
                                   v
+-----------------------------------------------------------------------+
|                     TIM2 Timebase & Compare Unit                      |
|  Prescaler (PSC = 15)       -->  Tick Clock = 1,000,000 Hz (1 us)     |
|  Auto-Reload (ARR = 999)    -->  PWM Period = 1,000 us (1 kHz)        |
|  Compare Reg (CCR1 = Duty)  -->  Controls Output High Duration        |
+-----------------------------------------------------------------------+
                                   |
                                   v
+-----------------------------------------------------------------------+
|                  Output Compare Mode Unit (CCMR1)                     |
|  OC1M = 0b110 (PWM Mode 1): Output HIGH while CNT < CCR1              |
|  OC1PE = 1 (Preload Enable for Glitch-Free CCR1 Updating)             |
+-----------------------------------------------------------------------+
                                   |
                                   v
+-----------------------------------------------------------------------+
|                   Output Pin Routing & Enable (CCER)                  |
|  CC1E = 1 (Enable Channel 1 Signal Transmission to Pin)               |
|  GPIOA PA5 Set to Alternate Function 1 (AF01 = TIM2_CH1)              |
+-----------------------------------------------------------------------+
                                   |
                                   v
+-----------------------------------------------------------------------+
|                 Physical Pin Output (PA5 Onboard LED)                 |
|  High-Speed Switching Signal Creates Variable Brightness (Breathing)  |
|  CPU Overhead for Waveform Generation = 0%                            |
+-----------------------------------------------------------------------+

```
### Key Technical Concepts

- **Hardware Signal Generation (Zero CPU Load):**
  - Unlike software pin-toggling, the hardware timer automatically toggles the output state on PA5 based on internal counter-to-compare comparisons (`CNT` vs `CCR1`), freeing the CPU completely from generating signal timing.
- **Alternate Function Pin Multiplexing (`AFR` Register):**
  - Reconfiguring PA5 from Standard Output Mode (`01`) to **Alternate Function Mode (`10`)** and binding it to **AF01** routes control of the physical pin directly to `TIM2_CH1`.
- **PWM Mode 1 Configuration (`CCMR1` Register):**
  - Setting `OC1M = 0b110` in `TIM2->CCMR1` enforces PWM Mode 1: the output channel is driven **HIGH** as long as `TIM2->CNT < TIM2->CCR1`, and switches **LOW** when `CNT >= CCR1`.
- **Preload Register Synchronization (`OC1PE` Bit):**
  - Enabling Output Compare Preload (`OC1PE`) ensures that updates to the duty cycle (`CCR1`) are buffered and only transferred to the active register on an Update Event (`UEV`), preventing waveform glitches or partial pulse states.
- **Output Channel Activation (`CCER` Register):**
  - Asserting the `CC1E` bit in `TIM2->CCER` acts as the physical hardware switch, connecting the generated PWM pulse train from internal peripheral logic directly to the mapped GPIO output pad.

---

### Task Breakdown Architecture

| Module / Component | Operation Type | Trigger / Condition | Primary Action | Hardware / Register |
| :--- | :--- | :--- | :--- | :--- |
| **CLOCK ENABLE** | Setup | System Init | Enables APB1 (TIM2) & AHB1 (GPIOA) bus clocks | `RCC_APB1ENR`, `RCC_AHB1ENR` |
| **PIN MULTIPLEXING** | Hardware Routing | Setup | Configures PA5 as Alternate Function 1 (`TIM2_CH1`) | `GPIOA->MODER`, `GPIOA->AFR[0]` |
| **TIMEBASE SETUP** | Frequency Control | Continuous (16 MHz / PSC) | Sets PWM frequency to 1 kHz (Period = 1000 us) | `TIM2->PSC`, `TIM2->ARR` |
| **PWM MODE CONFIG** | Output Mode | Register Write | Sets PWM Mode 1 (`110`) & Preload Enable | `TIM2->CCMR1` (`OC1M`, `OC1PE`) |
| **CHANNEL ENABLE** | Physical Output | Gate Control | Connects internal compare signal to PA5 pin | `TIM2->CCER` (`CC1E`) |
| **DUTY MODULATION** | Application Logic | Main Loop Execution | Adjusts `CCR1` value dynamically (0 to 1000) | `TIM2->CCR1` |

---

### Hardware Verification & Results

- **Smooth Breathing Effect:** Verified via the onboard PA5 LED gracefully fading in and out between 0% and 100% duty cycle with 5 ms step delays driven by SysTick.
- **Exact Waveform Timing:** Prescaler value 15 (16 MHz / (15 + 1) = 1 MHz) and ARR value 999 (1000 ticks) produce a precise **1.00 kHz PWM frequency**.
- **Glitch-Free Register Updates:** Dynamic changes to `TIM2->CCR1` occurred seamlessly without logic spikes or LED flickering due to enabled preload registers (`OC1PE`).

---

### Conclusion
Day 036 successfully implements **hardware-driven PWM generation via bare-metal register manipulation**. By bypassing software pin toggling and using `TIM2_CH1` through `CCMR1` and `CCER`, precise power modulation is achieved with maximum performance and complete CPU independence.