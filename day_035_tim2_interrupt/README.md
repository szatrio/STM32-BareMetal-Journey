# Day 035: Bare-Metal TIM2 Hardware Interrupt Architecture via ARM Cortex-M NVIC

### Objective
Migrate the **TIM2 32-bit General-Purpose Timer** from synchronous status polling to a fully **asynchronous hardware-driven interrupt architecture** on the STM32F401RE. Configure the **Nested Vectored Interrupt Controller (NVIC)** at the register level to handle timer update events (`UIF`), delegating heartbeat LED toggling asynchronously to `TIM2_IRQHandler` while leaving the main execution loop entirely non-blocking.

---

### System Architecture & Interrupt Execution Flow

```text
+-----------------------------------------------------------------------+
|                         STM32F401 Clock Tree                          |
|                       HSI Clock Source (16 MHz)                       |
+-----------------------------------------------------------------------+
                                   |
                                   v
+-----------------------------------------------------------------------+
|                      TIM2 Hardware Peripheral                         |
|  Prescaler (PSC = 15999)    -->  TIM2 Tick Clock = 1,000 Hz (1 ms)    |
|  Auto-Reload (ARR = 499)    -->  Overflow Period = 500 ms             |
|  [Counter: 0 -> 499] ----> Asserts Hardware IRQ Line (DIER_UIE)       |
+-----------------------------------------------------------------------+
                                   |
                                   v 
                        (Hardware IRQ Line 28)
+-----------------------------------------------------------------------+
|                      ARM Cortex-M NVIC Controller                     |
|  Enabled via NVIC ISER[0] Register                                    |
|  Preempts CPU, Pushes Context to Stack & Branches to Vector Table     |
+-----------------------------------------------------------------------+
                                   |
                                   v
+-----------------------------------------------------------------------+
|                     TIM2_IRQHandler() Execution                       |
|  1. Verify & Clear UIF Status Flag in TIM2_SR (Prevents Trapping)     |
|  2. Toggle Heartbeat LED (PA5) via GPIOA_ODR                          |
|  3. Restore CPU Context & Resume Main Foreground Task                 |
+-----------------------------------------------------------------------+
```

### Key Technical Concepts

- **Asynchronous Event Preemption:**
  - The CPU no longer monitors timer registers in `main()`. Instead, hardware NVIC interrupts the core only upon counter overflow, executing the handler and returning to the foreground task seamlessly.
- **Peripheral IRQ Activation (`DIER` Register):**
  - Setting the Update Interrupt Enable (`UIE`) bit in `TIM2->DIER` permits the peripheral hardware to signal the NVIC upon hitting Auto-Reload values.
- **Bare-Metal NVIC Configuration (`ISER` Array):**
  - IRQ 28 (TIM2) is enabled by manipulating `NVIC->ISER[0]` using bit-shift logic (`1U << 28`), bypassing HAL and CMSIS library functions.
- **Vector Table Binding & Weak Symbol Override:**
  - Defining `TIM2_IRQHandler` in driver code overrides the default weak handler in `startup_stm32f401retx.s`, binding the ISR directly into the hardware vector table.
- **Status Flag Maintenance:**
  - Explicitly clearing `TIM2_SR_UIF` within the ISR is mandatory to de-assert the hardware IRQ line and avoid continuous re-entry loops.

---

### Task Breakdown Architecture

| Module / Component | Operation Type | Trigger / Condition | Primary Action | Hardware / Register |
| :--- | :--- | :--- | :--- | :--- |
| **CLOCK ENABLE** | Setup | System Init | Enables APB1 bus clock for TIM2 | `RCC_APB1ENR` |
| **TIMEBASE GENERATION** | Hardware Counter | Continuous (16 MHz / PSC) | Increments counter from 0 to ARR (499) | `TIM2->PSC`, `TIM2->ARR` |
| **PERIPHERAL IRQ** | Signal Trigger | Overflow (`CNT == ARR`) | Sends interrupt signal to NVIC | `TIM2->DIER` (`UIE`) |
| **NVIC CONTROLLER** | Core Gateway | IRQ Line 28 Asserted | Pauses CPU & branches to vector address | `NVIC->ISER[0]` |
| **ISR HANDLER** | Asynchronous Task | Vector Entry (`TIM2_IRQHandler`) | Clears `UIF` flag & toggles PA5 LED | `TIM2->SR`, `GPIOA->ODR` |
| **MAIN LOOP** | Foreground Task | Unrestricted CPU Access | Processes heavy/blocking user application tasks | CPU Core / `main.c` |

---

### Hardware Verification & Results

- **Asynchronous Execution Under Heavy Load:** Verified via onboard PA5 LED toggling steadily at **500 ms intervals** even while the CPU is stalled in an artificial 50-second blocking delay inside `main()`.
- **Minimal CPU Overhead:** Context switching and ISR execution complete within **~2 microseconds**, leaving over **99.99%** of CPU capacity available for application logic.
- **Telemetry Independence:** Serial logs confirm UART operations execute concurrently without disrupting background hardware timing.

---

### Conclusion
Day 035 completes the transition to a **bare-metal interrupt-driven timing architecture**. By delegating time-critical execution to hardware-triggered ISRs, system determinism and CPU efficiency are maximized for complex embedded firmware tasks.