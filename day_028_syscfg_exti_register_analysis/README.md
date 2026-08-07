# Day 028: Deep Dive EXTI Architecture & Hardware Behavior Validation

### Objective
Perform an in-depth register-level analysis and hardware validation of the STM32F4 External Interrupt (EXTI) and System Configuration Controller (`SYSCFG`) pipelines. Validate how individual peripheral controls—specifically `SYSCFG->EXTICR`, `EXTI->FTSR`/`RTSR`, and `EXTI->IMR`—dictate signal propagation from physical pin PC13 to the Cortex-M4 NVIC.

---

### Key Technical Deep Dives

- **SYSCFG Peripheral Multiplexing Mechanics:**
  - `EXTI Line 13` is shared across `PA13`, `PB13`, `PC13`, and `PD13`.
  - `SYSCFG->EXTICR[3]` (EXTICR4) maps EXTI lines 12 through 15.
  - Writing `0x2` to bits `[7:4]` routes Port C (PC13) to EXTI Line 13.
  - **Clock Dependency:** `SYSCFG` resides on the APB2 peripheral bus. Enabling `RCC_APB2ENR_SYSCFGEN` is strictly required before configuring `EXTICR`, otherwise bus writes are ignored.

- **Edge Trigger Detection (FTSR vs. RTSR):**
  - **FTSR (Falling Trigger Selection Register):** Captures HIGH-to-LOW voltage transitions (active state when pressing the Nucleo PC13 Active-LOW button).
  - **RTSR (Rising Trigger Selection Register):** Captures LOW-to-HIGH voltage transitions (releasing the button as voltage returns to 3.3V).

- **Interrupt Masking (IMR):**
  - `EXTI->IMR` acts as a hardware gating register.
  - If a bit in `IMR` is cleared (`0`), hardware edge detection still sets the pending flag in `EXTI->PR`, but signal transmission to the NVIC is blocked.

---

### Experimental Hardware Validation

To verify the hardware-level enforcement of the EXTI state machine, three isolated failure/edge-case tests were executed directly on the Nucleo-F401RE board:

#### 1. Test Case A: Disabled SYSCFG APB2 Clock
- **Modification:** Omitted `RCC_APB2ENR_SYSCFGEN` clock initialization.
- **Observed Behavior:** Pressing PC13 resulted in **zero system response**. Without APB2 clock energy, the `SYSCFG` block could not latch the `EXTICR[3]` write, leaving EXTI Line 13 disconnected from Port C.

#### 2. Test Case B: Trigger Inversion (`RTSR` Active)
- **Modification:** Swapped `EXTI->FTSR` for `EXTI->RTSR`.
- **Observed Behavior:** Pressing and holding PC13 yielded no LED toggle. The `EXTI15_10_IRQHandler` executed instantly upon **releasing** the button (rising edge transition back to 3.3V).

#### 3. Test Case C: Masked Line (`IMR = 0`)
- **Modification:** Omitted `EXTI->IMR |= EXTI_IMR_MR13`.
- **Observed Behavior:** Pressing PC13 resulted in **zero response**. Edge detection occurred internally in `EXTI->PR`, but output propagation to NVIC Vector 40 was blocked at the EXTI boundary.

---

### Summary Table

| Register | Offset | Value / Bit Mask | Functional Impact |
| :--- | :--- | :--- | :--- |
| `RCC->APB2ENR` | `0x44` | `RCC_APB2ENR_SYSCFGEN` (Bit 14) | Supplies clock energy to SYSCFG block |
| `SYSCFG->EXTICR[3]` | `0x14` | `(0x2U << 4)` | Routes Port C to EXTI Line 13 |
| `EXTI->FTSR` | `0x04` | `EXTI_FTSR_TR13` (Bit 13) | Enables Falling Edge detection |
| `EXTI->IMR` | `0x00` | `EXTI_IMR_MR13` (Bit 13) | Unmasks interrupt signal output to NVIC |
| `EXTI->PR` | `0x14` | `EXTI_PR_PR13` (Bit 13) | Clears pending interrupt flag inside ISR |

---

### Conclusion
Day 028 successfully established a concrete mental model of the STM32F4 EXTI peripheral pipeline. Validating register failure modes on real hardware confirmed the strict dependencies between clock gating, pin multiplexing, trigger edge selection, and interrupt mask propagation.