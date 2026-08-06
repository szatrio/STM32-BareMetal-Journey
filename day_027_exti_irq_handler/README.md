# Day 027: EXTI Hardware Initialization & Shared IRQ Handler Integration (PC13 to PA5 Toggle)

### Objective
Examine, implement, and physically verify the complete bare-metal External Interrupt (EXTI) pipeline on the STM32F401RE Nucleo board without using HAL libraries. The primary goal is to configure the core registers—including `SYSCFG`, `EXTI` (FTSR, IMR, PR), and Cortex-M4 `NVIC`—to capture hardware button events on PC13 (Active LOW) and execute an instant ISR response (`EXTI15_10_IRQHandler`) to toggle the onboard PA5 LED.

---

### Key Learnings

- **EXTI Architecture & Peripheral Dependencies:**
  - **SYSCFG Routing:** Understood that GPIO pins are not directly connected to EXTI lines. The `SYSCFG->EXTICR` registers act as a multiplexer to route specific GPIO ports (e.g., Port C for PC13) to their corresponding EXTI line (EXTI Line 13).
  - **Clock Enable Order:** Learned that `RCC_APB2ENR_SYSCFGEN` must be enabled before configuring `SYSCFG_EXTICR`, otherwise register writes are ignored.
- **EXTI Trigger & Mask Mechanics:**
  - **FTSR (Falling Trigger Selection Register):** Configured for Active-LOW button detection (transition from HIGH to LOW when pressed).
  - **IMR (Interrupt Mask Register):** Unmasked EXTI Line 13 to allow hardware event signals to propagate to the NVIC.
  - **PR (Pending Register):** Verified that EXTI pending flags are cleared by writing `1` to the target bit position (`rc_w1` - Read / Clear by writing 1). Failing to clear `PR` results in infinite re-triggering of the ISR.
- **Shared IRQ Vectoring:** Handled vector position 40 (`EXTI15_10_IRQn`), which is shared across EXTI Lines 10 through 15. Implemented pending-bit checking inside the ISR to ensure line isolation before executing action logic.

---

### Technical Implementation

- **Core Register Mapping (`Inc/stm32f401_registers.h`):**
  - Mapped `SYSCFG_BASE` (`0x40013800UL`) and created `SYSCFG_TypeDef` to access `EXTICR[4]`.
  - Added bit masks: `RCC_APB2ENR_SYSCFGEN`, `EXTI_IMR_MR13`, `EXTI_FTSR_TR13`, and `EXTI_PR_PR13`.

- **EXTI Abstraction Driver (`Inc/stm32f401_exti.h` & `Src/stm32f401_exti.c`):**
  - Implemented `EXTI_PC13_Init()`:
    1. Enables APB2 clock for `SYSCFG`.
    2. Maps PC13 to EXTI Line 13 via `SYSCFG->EXTICR[3]` (Index 3 for EXTICR4).
    3. Configures falling edge trigger in `EXTI->FTSR`.
    4. Unmasks line 13 in `EXTI->IMR`.
  - Implemented `EXTI15_10_IRQHandler()`:
    1. Checks if `EXTI->PR & EXTI_PR_PR13` is active.
    2. Toggles PA5 Output Data Register (`GPIOA->ODR ^= (1U << 5)`).
    3. Clears pending status via `EXTI->PR = EXTI_PR_PR13`.

- **Main Application (`Src/main.c`):**
  - Initialized GPIOA (PA5 LED) as Output and GPIOC (PC13 Button) as Input.
  - Initialized EXTI registers via `EXTI_PC13_Init()`.
  - Enabled IRQ 40 in Cortex-M4 NVIC via `NVIC_EnableIRQ_Custom(40)`.
  - Continuous foreground loop executes background LED blinking while awaiting asynchronous interrupt preemption.

---

### Hardware/Registers Involved

- **Peripherals & System Core:**
  - `GPIOA` (Pin 5): User LED output indicator.
  - `GPIOC` (Pin 13): User Blue Push Button input source.
  - `SYSCFG`: Muxes PC13 to EXTI Line 13 (`EXTICR[3]`, bits [7:4] = `0x2`).
  - `EXTI`: Manages edge trigger, interrupt mask, and pending flag (`FTSR`, `IMR`, `PR`).
  - `NVIC`: Cortex-M4 Core Interrupt Controller (Vector position 40).

- **Registers Manipulated:**
  - `RCC->APB2ENR`: Set bit 14 (`SYSCFGEN`).
  - `SYSCFG->EXTICR[3]`: Set bits `[7:4]` to `0b0010` (Port C).
  - `EXTI->FTSR`: Set bit 13 (`TR13`).
  - `EXTI->IMR`: Set bit 13 (`MR13`).
  - `EXTI->PR`: Write `1` to bit 13 (`PR13`) inside ISR to acknowledge and clear.

---

### Verification & Hardware Results

- **Background Execution:**
  - PA5 LED blinks rhythmically in the main loop (`while(1)`), proving active CPU execution.
- **Asynchronous EXTI Trigger:**
  - Pressing the Blue User Button (PC13) immediately halts the background loop.
  - CPU jumps to `EXTI15_10_IRQHandler()`, toggles PA5 LED state instantly, clears `PR13`, and seamlessly resumes the main loop execution.
- **Pending Flag Integrity:**
  - Confirmed that clearing `PR13` inside the ISR prevents lockups and allows subsequent button presses to be captured accurately.

---

### Conclusion
Day 027 successfully closes the loop on bare-metal interrupt processing for STM32F4. By manually linking GPIO routing (`SYSCFG`), edge triggers (`EXTI`), and vector enable masks (`NVIC`), the system achieves instantaneous, event-driven hardware response capabilities required for real-time embedded system engineering.