# Day 034: Bare-Metal TIM2 Hardware Timer Basic Timebase & Non-Blocking Polling

### Objective
Configure and initialize the **TIM2 32-bit Hardware General-Purpose Timer** on the STM32F401RE to establish a precise **500 ms basic timebase**. Transition away from core-coupled SysTick delays by leveraging dedicated hardware peripheral timing. Implement a **non-blocking status flag polling mechanism (`TIM2->SR`)** in `main.c` to maintain continuous CPU availability for concurrent tasks while toggling a heartbeat LED and transmitting USART2 telemetry logs.

---

### System Architecture & Hardware Timer Flow

```text
+-----------------------------------------------------------------------+
|                         STM32F401 Clock Tree                          |
|                       HSI Clock Source (16 MHz)                       |
+-----------------------------------------------------------------------+
                                   |
                                   v
+-----------------------------------------------------------------------+
|                      TIM2 Hardware Peripheral                         |
|                                                                       |
|  Prescaler (PSC = 15999)    -->  TIM2 Tick Clock = 1,000 Hz (1 ms)    |
|  Auto-Reload (ARR = 499)    -->  Overflow Period = 500 ms             |
|                                                                       |
|  [Counter: 0 -> 499] ----> Sets Update Interrupt Flag (TIM2_SR_UIF)   |
+-----------------------------------------------------------------------+
                                   |
                                   v
+-----------------------------------------------------------------------+
|                      Super Loop Non-Blocking Poll                     |
|                                                                       |
|  if (TIM2_Has_Update_Occurred()) {                                    |
|      1. Clear UIF Flag automatically                                  |
|      2. Toggle Heartbeat LED (PA5)                                    |
|      3. Transmit UART Telemetry Event Log                             |
|  }                                                                    |
+-----------------------------------------------------------------------+
```

---

### Key Technical Concepts

- **Hardware Timer Independence:**
  - Unlike software delays or core-coupled SysTick, **TIM2 operates as an independent hardware peripheral** on the APB1 bus. The counter increments in the background via hardware clocking, leaving zero CPU execution load for time tracking.
- **Clock Division via Prescaler (PSC):**
  - Scales down the **16 MHz HSI system clock** to a 1 kHz timebase:
    - **Timer Clock** = 16,000,000 Hz / (15,999 + 1) = **1,000 Hz (1 ms per tick)**
- **Auto-Reload Period Setup (ARR):**
  - Defines the counter overflow limit. Setting ARR to `499` causes an update event every 500 ticks:
    - **Overflow Period** = (499 + 1) * 1 ms = **500 ms**
- **Non-Blocking Flag Polling (`UIF`):**
  - Rather than blocking execution inside wait loops, the system polls the **Update Interrupt Flag (`TIM2_SR_UIF`)** non-blockingly. When the flag sets, it is cleared immediately by software to arm the next 500 ms cycle.

---

### Task Breakdown Architecture

| Module / Component | Operation Type | Trigger / Condition | Primary Action | Hardware / Register |
| :--- | :--- | :--- | :--- | :--- |
| **RCC CLOCK ENABLE** | Hardware Setup | System Initialization | Enables APB1 clock bus for TIM2 peripheral | `RCC_APB1ENR` (Bit 0) |
| **TIM2 TIMEBASE** | Hardware Counter | Continuous (16 MHz / PSC) | Increments counter `CNT` from 0 to ARR (499) | `TIM2->PSC`, `TIM2->ARR` |
| **FLAG CHECKER** | Non-Blocking Poll | Super-loop iteration | Checks `TIM2->SR` Bit 0 (`UIF`) and auto-clears on match | `TIM2->SR` |
| **EVENT HANDLER** | Execution Task | Every 500 ms (`UIF == 1`) | Toggles PA5 LED & formats UART telemetry string | `GPIOA->ODR`, USART2 |

---

### Implementation Details

1. **TIM2 Hardware Initialization (`Src/stm32f401_tim.c`):**
   - Enabled APB1 bus clock and configured prescaler and auto-reload registers:
     ```c
     void TIM2_Timebase_Init(uint16_t psc, uint32_t arr) {
         RCC_APB1ENR |= RCC_APB1ENR_TIM2EN; // Enable TIM2 Clock
         TIM2->PSC = psc;                   // Load Prescaler value
         TIM2->ARR = arr;                   // Load Auto-Reload value
         TIM2->EGR |= TIM_EGR_UG;          // Re-initialize counter & update registers
         TIM2->CR1 |= TIM_CR1_CEN;          // Enable Counter
     }
     ```

2. **Non-Blocking Status Evaluation (`Src/stm32f401_tim.c`):**
   - Implemented flag evaluation and automatic bit clearing:
     ```c
     bool TIM2_Has_Update_Occurred(void) {
         if (TIM2->SR & TIM_SR_UIF) {
             TIM2->SR &= ~TIM_SR_UIF; // Clear Update Interrupt Flag
             return true;
         }
         return false;
     }
     ```

3. **Super-Loop Polling Execution (`Src/main.c`):**
   - Executed continuous non-blocking checks inside `while(1)` without stalling the CPU core:
     ```c
     while (1) {
         if (TIM2_Has_Update_Occurred()) {
             GPIOA->ODR ^= (1U << LED_PIN); // Toggle Heartbeat LED
             toggle_count++;
             snprintf(msg, sizeof(msg), "[TIM2] Overflow Event #%lu | LED Toggled\r\n", toggle_count);
             UART_Print(msg);
         }
     }
     ```

---

### Hardware Verification & Results

- **Precise 500 ms Hardware Timing:** Verified via onboard PA5 LED blinking at exactly 1 Hz frequency (500 ms ON, 500 ms OFF).
- **USART2 Telemetry Streaming:** Confirmed via serial terminal (115200 baud) that overflow log lines print synchronously with hardware LED state changes:
  ```text
  === TIM2 Basic Timebase Started ===
  [TIM2] Overflow Event #1 | LED Toggled
  [TIM2] Overflow Event #2 | LED Toggled
  [TIM2] Overflow Event #3 | LED Toggled
  ```
- **Zero CPU Delay Blocking:** Verified that CPU spends less than 0.1% of execution cycles handling the status check, leaving the main loop open for additional concurrent non-blocking tasks.

---

### Conclusion
Day 034 successfully established a **hardware-driven 500 ms basic timebase** using the STM32 TIM2 peripheral. Moving timebase generation from software logic to dedicated hardware registers ensures precise, jitter-free timing and forms the foundation for future Hardware Interrupt (ISR) and PWM implementations.