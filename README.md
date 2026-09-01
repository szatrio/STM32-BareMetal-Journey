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

| Day | Module / Hyperlink | Key Focus Areas |
| :---: | :--- | :--- |
| **Day 001** | [day_001_direct_register_access](./day_001_direct_register_access) | Memory-mapped I/O, `volatile` pointers, RCC clock gating |
| **Day 002** | [day_002_register_abstraction](./day_002_register_abstraction) | Base addresses, register offsets, preprocessor guards |
| **Day 003** | [day_003_bitwise_masking](./day_003_bitwise_masking) | Safe Read-Modify-Write (`&= ~`, `|=`), bitwise shift operations |
| **Day 004** | [day_004_gpio_mode_configuration](./day_004_gpio_mode_configuration) | Dynamic 2-bit field bitmasking (`pin * 2`), static inline utilities |
| **Day 005** | [day_005_gpio_otyper_configuration](./day_005_gpio_otyper_configuration) | Push-Pull vs Open-Drain mechanics, strongly-typed enums |
| **Day 006** | [day_006_gpio_pupdr_configuration](./day_006_gpio_pupdr_configuration) | Internal Pull-Up/Pull-Down, DRY architecture refactoring |
| **Day 007** | [day_007_GPIO_Output_Control](./day_007_GPIO_Output_Control) | State-based pin write API, explicit HIGH/LOW output driving |
| **Day 008** | [day_008_bsrr_atomic_toggle](./day_008_bsrr_atomic_toggle) | Atomic bit operations via `BSRR`, eliminating RMW race conditions |
| **Day 009** | [day_009_software_delay_timing](./day_009_software_delay_timing) | Dummy loop delay limitations & clock frequency dependencies |
| **Day 010** | [day_010_gpio_Input_idr](./day_010_gpio_Input_idr) | Digital pin reading via Input Data Register (`IDR`) |
| **Day 011** | [day_011_gpio_input_debouncing](./day_011_gpio_input_debouncing) | Software switch debouncing & state toggle logic |
| **Day 012** | [day_012_non_blocking_fsm_debounce](./day_012_non_blocking_fsm_debounce) | Non-blocking state machines for noise filtration |
| **Day 013** | [day_013_struct_based_mapping_and_modularization](./day_013_struct_based_mapping_and_modularization) | C `struct` register overlays (`GPIO_TypeDef` alignment) |

---

### Phase 2: Serial Communication (USART)

| Day | Module / Hyperlink | Key Focus Areas |
| :---: | :--- | :--- |
| **Day 014** | [day_014_uart_afr_config](./day_014_uart_afr_config) | Alternate Function Register (`AFRH`/`AFRL`) pin muxing |
| **Day 015** | [day_015_uart_txe_polling](./day_015_uart_txe_polling) | Polling-based character transmission via `TXE` flag |
| **Day 016** | [day_016_uart_rx_polling](./day_016_uart_rx_polling) | Polling-based character reception via `RXNE` flag |
| **Day 017** | [day_017_uart_print_string](./day_017_uart_print_string) | String transmission drivers & `printf` redirection support |
| **Day 018** | [day_018_uart_non_blocking_rx](./day_018_uart_non_blocking_rx) | Non-blocking UART reception state polling |
| **Day 019** | [day_019_ring_buffer_design](./day_019_ring_buffer_design) | Circular ring buffer data structure design & index arithmetic |
| **Day 020** | [day_020_ring_buffer_atomic_push](./day_020_ring_buffer_atomic_push) | Safe push operations & buffer overflow handling |
| **Day 021** | [day_021_ring_buffer_atomic_pop](./day_021_ring_buffer_atomic_pop) | Safe pop operations & full-cycle FIFO throughput testing |
| **Day 022** | [day_022_error_handling_buffer_overflow](./day_022_error_handling_buffer_overflow) | UART overrun detection & error recovery mechanisms |
| **Day 023** | [day_023_uart_ring_buffer_tx](./day_023_uart_ring_buffer_tx) | Buffered UART transmission with interrupt-driven queues |
| **Day 024** | [day_024_watchdog_timer](./day_024_watchdog_timer) | Independent Watchdog (IWDG) system reset & timeout management |
| **Day 025** | [day_025_uart_stress_test](./day_025_uart_stress_test) | High-baudrate stress testing & ring buffer stability verification |

---

### Phase 3: Core Architecture (NVIC, EXTI & SysTick)

| Day | Module / Hyperlink | Key Focus Areas |
| :---: | :--- | :--- |
| **Day 026** | [day_026_nvic_enable_disable](./day_026_nvic_enable_disable) | Nested Vectored Interrupt Controller (NVIC) configuration |
| **Day 027** | [day_027_exti_irq_handler](./day_027_exti_irq_handler) | External Interrupt (EXTI) configuration & vector table routing |
| **Day 028** | [day_028_syscfg_exti_register_analysis](./day_028_syscfg_exti_register_analysis) | System Configuration Controller (`SYSCFG`) line mapping |
| **Day 029** | [day_029_exti_non_blocking_flag_event](./day_029_exti_non_blocking_flag_event) | Asynchronous interrupt event flags & ISR pending bit clearing |
| **Day 030** | [day_030_systick_setup_registers](./day_030_systick_setup_registers) | SysTick core hardware timer register configuration |
| **Day 031** | [day_031_systick_handler_global_tick](./day_031_systick_handler_global_tick) | `SysTick_Handler` ISR, `volatile g_ticks`, non-blocking timekeeping |
| **Day 032** | [day_032_systick_delay_ms](./day_032_systick_delay_ms) | Hardware-backed precise `SysTick_DelayMs()` driver |

---

## License

![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)

© 2026 Satrio Utomo. Released under the **MIT License**.