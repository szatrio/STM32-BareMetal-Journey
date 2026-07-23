# Day 015: Bare-Metal UART Driver Implementation & Polling Transmit Pipeline

### Objective
Implement a bare-metal UART transmission driver for the STM32F401 microcontroller from scratch using direct register access. Develop the low-level transmit polling mechanism (`UART_SendChar`) based on status register flag evaluation, integrate physical serial output with the debounced push-button FSM, and validate real-time serial telemetry on a PC host via a terminal console.

---

### Key Learnings

- **USART Peripheral Data Flow:** Understood the dual-register hardware architecture comprising the Transmit Data Register (`TDR`) and the internal Output Shift Register, enabling asynchronous parallel-to-serial conversion.
- **Polling / Blocking Telemetry:** Mastered the polling technique using hardware flag evaluation to synchronize CPU data execution with peripheral hardware transmission cycles.
- **Status Register Masking & Logic Inversion:** Analyzed bitwise AND operations combined with logical NOT (`!`) against the `USART_SR_TXE` flag to halt CPU execution until the hardware transmit buffer is safely cleared.
- **Direct Assignment vs Bitwise Mutators:** Deciphered hardware register semantics from the STM32 Reference Manual, establishing why direct assignment (`=`) is required for `USART_DR` to prevent byte corruption from stale buffer values.
- **Physical Serial Debugging:** Configured serial connection properties (Baud Rate: 115200, 8 Data bits, No Parity, 1 Stop bit) and observed physical hardware reset behavior, transient power-on glitches, and character stream outputs using PuTTY.

---

### Technical Implementation

- **Hardware Register Mapping & Clock Initialization:**
  Enabled APB1 peripheral bus clock for `USART2` (`RCC_APB1ENR`) and mapped PA2 (`TX`) and PA3 (`RX`) to `AF7` (USART2 Alternate Function) using the Day 14 dynamic bit-field driver API.

- **Baud Rate & Transmitter Configuration:**
  Programmed the fractional Baud Rate Register (`USART2->BRR`) for $115200\text{ bps}$ execution and enabled the hardware transmitter engine by setting the Transmitter Enable (`TE`) and USART Enable (`UE`) bits in `USART2->CR1`.

- **Register-Level Character Transmission Protocol:**
  Engineered `UART_SendChar(char ch)` to poll Bit 7 (`TXE`) of `USART2->SR`. Implemented logic inversion `while (!(USART2->SR & USART_SR_TXE))` to block CPU execution until the Data Register is empty, then performed a clean 8-bit masked assignment to `USART2->DR`.

- **Event-Driven Telemetry Integration:**
  Embedded character transmission calls into the main application loop, triggering instantaneous serial character output (`UART_SendChar('B')`) in tandem with LED state toggles (`PA5`) upon valid debounced button presses (`PC13`).

---

### Hardware/Registers Involved

- **Registers:**
  - `USART2->SR`: Status Register — specifically Bit 7 (`TXE` / Transmit Data Register Empty) used to evaluate buffer readiness.
  - `USART2->DR`: Data Register — low 9-bit field (`DR[8:0]`) written directly to load payload data into the transmit queue.
  - `USART2->BRR`: Baud Rate Register — programmed to divide peripheral clock for exact bit-rate timing.
  - `USART2->CR1`: Control Register 1 — configured to enable USART peripheral hardware (`UE`) and transmitter control (`TE`).
  - `GPIOA->MODER` & `GPIOA->AFR[0]`: Mapped PA2/PA3 to Alternate Function 7 (`AF7`).

---

### Verification & Hardware Results

- **Serial Terminal Validation (PuTTY):**
  - Established a stable $115200\text{ baud}$ serial session over the ST-LINK Virtual COM Port.
  - Verified system startup character (`'A'`) transmission triggered precisely on hardware reset/reboot.
  - Confirmed instantaneous, deterministic transmission of payload characters (`'B'`) upon each debounced push-button event.
- **Physical Layer Observation:**
  - Observed transient line noise (`Ξ`) on physical reset caused by ST-LINK line voltage drops during CPU power-cycling, verifying normal UART physical layer behavior.
- **GPIO System Coexistence:**
  - Confirmed that real-time UART blocking loops do not starve or corrupt the non-blocking button FSM debouncer or LED toggling routines.

---

### Conclusion
Day 15 successfully delivers a fully functional, bare-metal USART2 transmit driver built directly from reference manual specifications. By mastering low-level register polling, bitwise status evaluation, and physical serial terminal debugging, the driver architecture now possesses a solid hardware-level logging and telemetry mechanism for embedded development.