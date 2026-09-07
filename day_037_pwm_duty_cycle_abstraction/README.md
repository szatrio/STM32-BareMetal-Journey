# Day 037: Bare-Metal Cortex-M4F Hardware FPU Integration & Floating-Point PWM Duty Cycle Modulation

### Objective
Migrate the **TIM2 Hardware PWM Duty Cycle Modulation** logic from integer tick scaling (0–1000) to human-readable **floating-point percentages (0.0% to 100.0%)**. Safely enable the ARM Cortex-M4F **Hardware Floating Point Unit (FPU)** via the Coprocessor Access Control Register (`CPACR`) within the System Control Block (`SCB`), preventing CPU execution crashes (`HardFault`). Modularize core architecture by encapsulating FPU control into a dedicated system utility module (`stm32f401_system.h/c`) without polluting peripheral macro headers.

---

### System Architecture & Hardware FPU Execution Pipeline

```text
+-----------------------------------------------------------------------+
|                         STM32F401 Core Domain                         |
|                     ARM Cortex-M4F CPU Core @ 16 MHz                  |
+-----------------------------------------------------------------------+
                                   |
                                   v
+-----------------------------------------------------------------------+
|                    System Control Block Initialization                |
|  System Init Calls FPU_Enable() Before Any Math Operations            |
|  SCB->CPACR (0xE000ED88) Bits Set to 0b1111 (Full Access)     |
|  CP10 & CP11 Coprocessors Powered ON & Unlocked                       |
+-----------------------------------------------------------------------+
                                   |
                                   v
+-----------------------------------------------------------------------+
|                  Hardware Floating Point Unit (FPU)                   |
|  Hardware Single-Precision Math Execution (-mfloat-abi=hard)          |
|  Executes Native Assembly Instructions (e.g., vadd.f32, vmul.f32)     |
|  Calculates CCR1 Ticks: Ticks = (Duty / 100.0f) * ARR                 |
|  Execution Latency: 1 - 3 Clock Cycles per Calculation                |
+-----------------------------------------------------------------------+
                                   |
                                   v
+-----------------------------------------------------------------------+
|                     TIM2 Output Compare Unit & Pin                    |
|  Raw Integer CCR1 Value Updated (0 to ARR Max Limit)                  |
|  Signal Modulated to PA5 (TIM2_CH1) @ 1 kHz                           |
|  Glitch-Free Preloaded PWM Signal Driven to Onboard LED               |
+-----------------------------------------------------------------------+

```

### Key Technical Concepts

- **Hardware FPU Activation (`SCB->CPACR` Register):**
  - By default on power-on/reset, the Cortex-M4F FPU coprocessor is powered down to minimize power consumption. Executing single-precision hardware instructions without enabling it causes the core to treat them as invalid opcodes, triggering an immediate `UsageFault` / `HardFault`.
- **Coprocessor Access Privileges (`CP10` & `CP11`):**
  - Bits 20–21 (`CP10`) and 22–23 (`CP11`) of the `CPACR` register govern access rights to the floating-point extension. Writing `0b11` to both fields grants **Full Access** across privileged and unprivileged execution modes.
- **Architectural Separation & Modularization:**
  - Core Cortex-M system registers (`SCB`) are defined with strict byte padding inside `SCB_TypeDef` within `stm32f401_registers.h`. Execution logic (`FPU_Enable()`) is cleanly encapsulated in `stm32f401_system.c/h`, preserving header files for macro/struct definitions.
- **Floating-Point Duty Cycle Mapping & Boundary Clamping:**
  - Dynamic percentage values (`0.0f` to `100.0f`) are scaled to internal timer ticks ($CCR1 = \frac{\text{duty}}{100.0f} \times ARR$). Floating-point boundary clamping ensures values never exceed `100.0f` or drop below `0.0f`, protecting against timer overflow glitches.
- **Efficient Non-FP-Printf Formatting:**
  - Floating-point logging over UART uses integer extraction (`int_part` and `dec_part`) with `snprintf`, avoiding the heavy Flash/RAM footprint of full float linker flags (`-u _printf_float`).

---

### Task Breakdown Architecture

| Module / Component | Operation Type | Trigger / Condition | Primary Action | Hardware / Register |
| :--- | :--- | :--- | :--- | :--- |
| **SYSTEM CORE** | FPU Initialization | Main Entry Point | Powers ON CP10/CP11 coprocessors with Full Access (`0b11`) | `SCB->CPACR` (`0xE000ED88`) |
| **SYSTEM HEADERS** | Architecture Design | Compilation | Isolates register structs (`.h`) from execution functions (`.c`) | `Inc/stm32f401_system.h` |
| **SYSTEM LOGIC** | Core System Driver | Pre-peripheral setup | Executes register bit-wise OR to enable FPU hardware | `Src/stm32f401_system.c` |
| **PWM DRIVER** | Duty Modulator | Main Loop Execution | Clamps float percentage and computes target `CCR1` value | `TIM2->CCR1` |
| **UART LOGGING** | Low-Overhead Printing | Main Loop Execution | Formats float to string via integer decomposition for UART | `USART2->DR`, `snprintf` |

---

### Hardware Verification & Results

- **Zero-Crash Execution:** Verified smooth execution of hardware floating-point instructions (`-mfloat-abi=hard`) without triggering `HardFault` or system resets.
- **Smooth LED Modulation:** The onboard PA5 LED gracefully fades in and out using fractional floating-point steps (`duty += 1.0f`), mapped directly to target `TIM2->CCR1` registers.
- **Precise UART Telemetry:** Verified real-time serial logging outputs accurate percentage metrics (e.g., `[PWM] Duty Cycle: 45.0%`) alongside active PWM generation.

---

### Conclusion
Day 037 successfully integrates **hardware-accelerated floating-point operations into a bare-metal STM32 project architecture**. By configuring `SCB->CPACR` and separating system core initializations into dedicated modules, the application achieves high-precision dynamic PWM modulation with total system stability and professional codebase organization.