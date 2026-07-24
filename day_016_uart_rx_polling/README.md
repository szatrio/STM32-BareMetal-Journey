# Day 016: Bare-Metal UART Receiver Driver & RXNE Polling Pipeline

### Objective
Expand the bare-metal USART driver to support full-duplex communication by implementing a low-level reception engine for the STM32F401 microcontroller. Develop a polling-based character read algorithm (`UART_ReceiveChar`) using status register flag evaluation, integrate an interactive PC-to-MCU command-and-control interface via serial terminal, and resolve physical layer flow control constraints during live system testing.

---

### Key Learnings

- **USART Reception Data Pipeline:** Analyzed hardware data flow from physical pin PA3 (RX) through the internal Input Shift Register into the Transmit/Receive Data Register (`USART_DR`).
- **Hardware Flag Lifecycle & Auto-Reset:** Understood the operational behavior of Bit 5 (`RXNE` - Read Data Register Not Empty) in `USART_SR`, noting how hardware automatically sets `RXNE` upon byte reception and clears it when `USART_DR` is read by the CPU.
- **Polling / Blocking Reception Pattern:** Modeled CPU blocking logic using status bit mask evaluation to stall execution flow until incoming payload bytes are fully assembled in memory.
- **Receiver Engine Enable Dynamics:** Mastered the control sequence in `USART_CR1`, recognizing that Receiver Enable (`RE`, Bit 2) must be asserted alongside `TE` and `UE` to activate the internal RX sampling clocks.
- **Serial Terminal Flow Control & Local Echo:** Identified host-side UART transmission behaviors in PuTTY, diagnosing software-level stalls caused by hardware flow control settings and implementing local character echo back to render typed inputs.

---

### Technical Implementation

- **Receiver Hardware Enablement:**
  Updated `USART2_Init()` to set Bit 2 (`RE`) in `USART2->CR1`, powering on the internal sampling hardware on pin PA3 (mapped to `AF7` Alternate Function).

- **Register-Level Reception Protocol (`UART_ReceiveChar`):**
  Engineered a blocking reception driver that polls Bit 5 (`RXNE`) of `USART2->SR`. Applied inverted bitwise masking `while (!(USART2->SR & USART_SR_RXNE))` to halt execution until data is available, then performed a masked byte read `(USART2->DR & 0xFF)` which atomically resets `RXNE`.

- **Interactive Command & Echo Loop:**
  Implemented an interactive terminal pipeline in `main.c`. Processed incoming ASCII control bytes to control hardware state (`'1'` for LED ON, `'0'` for LED OFF) and transmitted immediate echo responses via `UART_SendChar` to provide visual feedback on the PC console.

---

### Hardware/Registers Involved

- **Registers:**
  - `USART2->SR`: Status Register — Bit 5 (`RXNE` / Read Data Register Not Empty) evaluated to detect pending incoming bytes.
  - `USART2->DR`: Data Register — low 8 bits read to retrieve received character payload and automatically clear `RXNE`.
  - `USART2->CR1`: Control Register 1 — modified to enable Receiver Engine (`RE`, Bit 2), Transmitter Enable (`TE`), and USART Hardware (`UE`).
  - `GPIOA->MODER` & `GPIOA->AFR[0]`: Configured PA3 as Alternate Function 7 (`AF7` / `USART2_RX`).

---

### Verification & Hardware Results

- **Interactive Terminal Command Testing (PuTTY):**
  - Established 115200 baud serial session over ST-LINK Virtual COM Port with **Flow Control set to None**.
  - Confirmed receipt of initial initialization string (`"RX OK\r\n"`) upon CPU reset.
  - Verified real-time local echo of typed characters on the PuTTY console screen.
  - Evaluated character command execution: pressing `'1'` triggered onboard LED (`PA5`) HIGH state, while pressing `'0'` drove `PA5` LOW state instantly.
- **Flow Control & Diagnostic Resolution:**
  - Resolved initial reception blocking issues by disabling PuTTY software/hardware flow control lines, enabling host-to-target character transmission.

---

### Conclusion
Day 16 successfully achieves full-duplex bare-metal UART communication. By establishing a robust RX polling mechanism, understanding automatic register clear behaviors, and creating an interactive command interpreter, the driver architecture now supports bidirectional telemetry and host control.