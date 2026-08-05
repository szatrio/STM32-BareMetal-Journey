# Day 026: Nested Vectored Interrupt Controller (NVIC) Enable/Disable Register Mechanics

### Objective
Examine and verify the hardware behavior of the Cortex-M4 Nested Vectored Interrupt Controller (NVIC), specifically focusing on the Interrupt Set-Enable Register (`ISER`) and Interrupt Clear-Enable Register (`ICER`). Implement direct register manipulation routines to validate hardware-enforced bit behavior, such as the "Write 0 Has No Effect" policy on `ISER`, using physical visual feedback via PA5 LED.

---

### Key Learnings

- **Cortex-M4 Core Architecture vs. Vendor Peripherals:** Understood the separation between STMicroelectronics peripherals (mapped at `0x40000000`) and ARM Cortex-M4 Core peripherals (mapped in the System Control Space / PPB starting at `0xE000E000`).
- **NVIC Register Mechanics (`ISER` vs. `ICER`):**
  - **ISER (Interrupt Set-Enable Register):** Writing `1` enables the target interrupt line; writing `0` is explicitly ignored by hardware (*Write 0 Has No Effect*).
  - **ICER (Interrupt Clear-Enable Register):** Writing `1` disables the target interrupt line; writing `0` is ignored by hardware.
- **Hardware-Level Bit Bitwise Safety:** Experienced how ARM's atomic write-1-to-set / write-1-to-clear architecture eliminates the need for read-modify-write operations on interrupt enable bits, preventing race conditions.
- **Physical Verification State Machine:** Designed a sequential visual verification protocol using the onboard PA5 LED to prove register bit states directly on hardware without requiring step-by-step debugging.

---

### Technical Implementation

- **Core Register Mapping (`Inc/stm32f401_registers.h`):**
  Mapped the Cortex-M4 NVIC base address (`0xE000E100UL`) and defined `NVIC_TypeDef` representing `ISER`, `ICER`, `ISPR`, `ICPR`, `IABR`, and `IPR` register arrays.

- **Custom NVIC Driver Abstraction (`Inc/stm32f401_nvic.h` & `Src/stm32f401_nvic.c`):**
  Implemented direct register bit-manipulation helpers:
  - `NVIC_EnableIRQ_Custom(IRQn_Type irq_num)`: Sets the target bit in `ISER[irq_num >> 5]`.
  - `NVIC_DisableIRQ_Custom(IRQn_Type irq_num)`: Sets the target bit in `ICER[irq_num >> 5]`.
  - `NVIC_GetEnableIRQ_Custom(IRQn_Type irq_num)`: Reads bit status from `ISER[irq_num >> 5]`.

- **Test Suite (`Src/main.c`):**
  - Tested bit operations using `EXTI0_IRQn` (IRQ Vector Position 6).
  - Implemented a 3-stage validation sequence backed by `Delay_Simple()` delays and PA5 LED states.

---

### Hardware/Registers Involved

- **Peripherals & Core Drivers:**
  - `NVIC` (Nested Vectored Interrupt Controller): Target core peripheral at `0xE000E100UL`.
  - `GPIOA` (PA5 Green LED Output): Serves as the real-time visual indicator for NVIC enable status verification.

- **Registers Manipulated:**
  - `NVIC->ISER[0]`: Tested bit set operation and verified that attempting `ISER[0] &= ~(1U << 6)` fails to clear the enable bit.
  - `NVIC->ICER[0]`: Tested bit clear operation by writing `1U << 6` to force the bit to zero.

---

### Verification & Hardware Results

- **Test 1 — Enable via ISER:**
  - `NVIC_EnableIRQ_Custom(6)` executed successfully.
  - `NVIC_GetEnableIRQ_Custom(6)` returned `1`.
  - **LED Response:** PA5 LED turned **ON solid for ~2 seconds**.
- **Test 2 — Prove "Write 0 Has No Effect" on ISER:**
  - Executed direct zero-write attempt: `NVIC->ISER[0] &= ~(1U << 6)`.
  - `NVIC_GetEnableIRQ_Custom(6)` confirmed bit remained `1`.
  - **LED Response:** PA5 LED **blinked rapidly 3 times**, proving that writing `0` to `ISER` is safely ignored by ARM hardware.
- **Test 3 — Disable via ICER:**
  - `NVIC_DisableIRQ_Custom(6)` executed successfully.
  - `NVIC_GetEnableIRQ_Custom(6)` returned `0`.
  - **LED Response:** PA5 LED turned **OFF permanently**.

---

### Conclusion
Day 026 successfully isolates and verifies the core mechanics of the Cortex-M4 NVIC registers (`ISER` and `ICER`). By building a custom driver from raw register definitions and confirming hardware behaviors directly on the Nucleo board, the system establishes a solid, race-condition-free foundation for handling hardware interrupts in upcoming peripheral implementations.