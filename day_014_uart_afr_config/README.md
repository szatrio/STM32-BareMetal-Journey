# Day 014: Alternate Function Mapping & Dynamic Bitwise Field Configuration

### Objective
Expand the bare-metal GPIO driver architecture to support hardware peripheral multiplexing. This involves mapping GPIO pins to internal hardware peripherals (specifically UART2) by implementing dynamic bit-field manipulation algorithms over the STM32 Alternate Function Registers (`AFRL` and `AFRH`), verified via real-time register debugging.

---

### Key Learnings

- **Peripheral Pin Multiplexing:** Understood how STM32 hardware routing decouples standard GPIO logic to connect physical pins directly to internal peripherals (UART, SPI, I2C, Timers) via Alternate Function registers.
- **Array-Based Register Abstraction:** Modeled adjacent hardware registers (`AFRL` at offset `0x20` and `AFRH` at offset `0x24`) into a unified 2-element array within the peripheral structure, enabling indexed register selection via integer arithmetic.
- **Dynamic Field Masking & Insertion:** Applied multi-bit bitwise isolation techniques to safely clear and write 4-bit configuration fields without corrupting adjacent pin settings inside the target register.
- **Hardware-Level Debugging:** Verified physical memory mapping by halting execution at runtime and inspecting live SFR (Special Function Register) values inside STM32CubeIDE to confirm exact bit pattern matching (`0x7700`).

---

### Technical Implementation
- **Struct Offset Alignment & Array Abstraction:**
  Updated `GPIO_TypeDef` with `LCKR` padding and a 2-element array `AFR[2]` to cleanly map `AFRL` (`AFR[0]` for pins 0–7) and `AFRH` (`AFR[1]` for pins 8–15) at exact memory offsets.

- **Dynamic Register & Bit Shift Indexing:**
  Used `pin / 8` to automatically select the target `AFR` register index and `(pin % 8) * 4` to determine the exact bit-shift offset for 4-bit field configurations.

- **Bitwise Masking & Insertion:**
  Implemented a safe two-stage bit manipulation process: clear the target 4-bit field using an inverted shifted mask (`~(0xF << shift)`), then merge the new AF value using bitwise OR.

- **GPIO Pin Multiplexing Routine:**
  Built `GPIO_SetAltFunction()` helper to route pins PA2 (`USART2_TX`) and PA3 (`USART2_RX`) to Alternate Function Mode (`AF7`).
---

### Hardware/Registers Involved

- **Registers:**
  - `GPIO_TypeDef->MODER`: Set to Alternate Function mode (`0x02`) for targeted pins (PA2 and PA3).
  - `GPIO_TypeDef->AFR[0]` (`AFRL`): Alternate Function Low Register (Offset `0x20`), specifically configuring Bit 8–11 (`AFRL2`) and Bit 12–15 (`AFRL3`).
  - `GPIO_TypeDef->AFR[1]` (`AFRH`): Alternate Function High Register (Offset `0x24`), mapped for high-byte pins (PA8 to PA15).

---

### Verification & Hardware Results

- **Live Debugger Inspection (SFRs Tab):**
  - Paused execution after `System_Init()` execution to evaluate memory addresses in real time.
  - Verified `GPIOA->AFRL` bit patterns transformed successfully from `0x0000` to **`0x7700`**.
  - Confirmed `AFRL2` (PA2) holds value **`0x7`** (`AF7` / `USART2_TX`).
  - Confirmed `AFRL3` (PA3) holds value **`0x7`** (`AF7` / `USART2_RX`).
- **GPIO System Coexistence:**
  - Verified onboard LED (PA5) toggling and push-button (PC13) debounce FSM logic remain fully operational without bit-collision from `AFR` register writes.

---

### Conclusion
Day 14 successfully completes low-level pin multiplexing and dynamic register masking. By successfully routing physical pins PA2 and PA3 to the USART2 peripheral at the register level, the driver infrastructure is fully prepared for implementing UART transmission and reception protocols.