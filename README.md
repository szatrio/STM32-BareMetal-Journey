# STM32 Bare-Metal Journey 🚀

A disciplined, hands-on journey mastering **bare-metal firmware development** on the **STM32F401RE (ARM Cortex-M4)** microcontroller. This repository documents a step-by-step progress from raw register manipulation to drivers, hardware interrupts, ring buffers, and SysTick timekeeping.

---

##  Hardware & Toolchain Specifications

- **Microcontroller:** STM32F401RET6 (ARM Cortex-M4 core with FPU, 84 MHz)
- **Development Board:** NUCLEO-F401RE
- **Compiler:** `arm-none-eabi-gcc` (GNU Arm Embedded Toolchain)
- **Build System / IDE:** STM32CubeIDE / GNU Make
- **Debugger:** ST-LINK/V2-1

---

##  Table of Contents & Learning Path

Click on any day's link to explore the source code, implementation details, and dedicated technical documentation.

### Phase 1: GPIO & Register Foundations

| Day | Folder Hyperlink | Title |
| :---: | :--- | :--- |
| **Day 001** | [day_001_direct_register_access](./day_001_direct_register_access) | Direct Register Access (Bare-Metal LED Blink) |
| **Day 002** | [day_002_register_abstraction](./day_002_register_abstraction) | Modular Header Abstraction (Register Offset Definition) |
| **Day 003** | [day_003_bitwise_masking](./day_003_bitwise_masking) | Bitwise Masking & Helper Macro Abstraction |
| **Day 004** | [day_004_gpio_mode_configuration](./day_004_gpio_mode_configuration) | Modular GPIO Mode Configuration & Static Inline Functions |
| **Day 005** | [day_005_gpio_otyper_configuration](./day_005_gpio_otyper_configuration) | GPIO Output Type Configuration (OTYPER Driver Extension) |
| **Day 006** | [day_006_gpio_pupdr_configuration](./day_006_gpio_pupdr_configuration) | GPIO Pull-Up/Pull-Down Configuration (DRY Driver Refactoring) |
| **Day 007** | [day_007_GPIO_Output_Control](./day_007_GPIO_Output_Control) | Explicit GPIO Output Control (State-Based Pin Write API) |
| **Day 008** | [day_008_bsrr_atomic_toggle](./day_008_bsrr_atomic_toggle) | Atomic Pin State Operations via BSRR (Thread-Safe Bit Operations) |
| **Day 009** | [day_009_software_delay_timing](./day_009_software_delay_timing) | Software Delay and Compiler Optimization |
| **Day 010** | [day_010_gpio_Input_idr](./day_010_gpio_Input_idr) | Bare-Metal Peripheral Control (GPIO Input & Clock Gating) |
| **Day 011** | [day_011_gpio_input_debouncing](./day_011_gpio_input_debouncing) | Debouncing Logic & Toggle Switch Implementation |
| **Day 012** | [day_012_non_blocking_fsm_debounce](./day_012_non_blocking_fsm_debounce) | Non-Blocking Debouncing via Finite State Machine (FSM) |
| **Day 013** | [day_013_struct_based_mapping_and_modularization](./day_013_struct_based_mapping_and_modularization) | Struct-Based Register Mapping & Driver Modularization |

---

### Phase 2: Serial Communication (USART)

| Day | Folder Hyperlink | Title |
| :---: | :--- | :--- |
| **Day 014** | [day_014_uart_afr_config](./day_014_uart_afr_config) | Alternate Function Mapping & Dynamic Bitwise Field Configuration |
| **Day 015** | [day_015_uart_txe_polling](./day_015_uart_txe_polling) | Bare-Metal UART Driver Implementation & Polling Transmit Pipeline |
| **Day 016** | [day_016_uart_rx_polling](./day_016_uart_rx_polling) | Bare-Metal UART Receiver Driver & RXNE Polling Pipeline |
| **Day 017** | [day_017_uart_print_string](./day_017_uart_print_string) | UART String Transmission Engine & Generic GPIO Driver Refactoring |
| **Day 018** | [day_018_uart_non_blocking_rx](./day_018_uart_non_blocking_rx) | Non-Blocking RX Engine & Register Flag Polling Abstraction |
| **Day 019** | [day_019_ring_buffer_design](./day_019_ring_buffer_design) | Ring Buffer Design & Circular Indexing Simulation |
| **Day 020** | [day_020_ring_buffer_atomic_push](./day_020_ring_buffer_atomic_push) | Atomic Push Operation & Overflow Protection |
| **Day 021** | [day_021_ring_buffer_atomic_pop](./day_021_ring_buffer_atomic_pop) | Atomic Pop Operation & Underflow Protection |
| **Day 022** | [day_022_error_handling_buffer_overflow](./day_022_error_handling_buffer_overflow) | Buffer Overflow Telemetry & Error Handling Strategy |
| **Day 023** | [day_023_uart_ring_buffer_tx](./day_023_uart_ring_buffer_tx) | Non-Blocking Buffered UART Transmit (TX) Engine |
| **Day 024** | [day_024_watchdog_timer](./day_024_watchdog_timer) | Hardware Watchdog Timer (IWDG) Integration & System Resilience |
| **Day 025** | [day_025_uart_stress_test](./day_025_uart_stress_test) | UART Stress Testing & High-Throughput System Stability |

---

### Phase 3: Core Architecture (NVIC, EXTI & SysTick)

| Day | Folder Hyperlink | Title |
| :---: | :--- | :--- |
| **Day 026** | [day_026_nvic_enable_disable](./day_026_nvic_enable_disable) | Nested Vectored Interrupt Controller (NVIC) Enable/Disable Register Mechanics |
| **Day 027** | [day_027_exti_irq_handler](./day_027_exti_irq_handler) | EXTI Hardware Initialization & Shared IRQ Handler Integration (PC13 to PA5 Toggle) |
| **Day 028** | [day_028_syscfg_exti_register_analysis](./day_028_syscfg_exti_register_analysis) | Deep Dive EXTI Architecture & Hardware Behavior Validation |
| **Day 029** | [day_029_exti_non_blocking_flag_event](./day_029_exti_non_blocking_flag_event) | Non-Blocking Event-Driven EXTI Interrupt Architecture |
| **Day 030** | [day_030_systick_setup_registers](./day_030_systick_setup_registers) | SysTick Setup & Core Timer Registers (ARM Cortex-M) |
| **Day 031** | [day_031_systick_handler_global_tick](./day_031_systick_handler_global_tick) | SysTick Exception Handler & Non-Blocking Delay Integration |
| **Day 032** | [day_032_systick_delay_ms](./day_032_systick_delay_ms) | High-Precision SysTick Blocking Delay Driver |

---

## License

![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)

© 2026 Satrio Utomo. Released under the **MIT License**.