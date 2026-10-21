# Day 070: MSP & PSP Stack Pointer Analysis and Switching

###  Objective
Understand and empirically validate the dual stack pointer architecture of the ARM Cortex-M4 processor. This milestone establishes the foundation for custom RTOS kernel development by analyzing how the CPU switches execution context between the Main Stack Pointer (**MSP**) and the Process Stack Pointer (**PSP**).

---

###  Key Technical Implementations
- **Core Intrinsics Helper (`cortex_m4_core.h`)**: Implemented inline assembly functions (`__get_MSP`, `__set_MSP`, `__get_PSP`, `__set_PSP`, `__get_CONTROL`, and `__set_CONTROL`) to interface directly with core special registers.
- **Independent Stack Allocation**: Reserved a dedicated static memory array (`512 bytes`) to act as the task stack space for the Process Stack Pointer (PSP).
- **Live Register Manipulation**: Dynamically modified the `CONTROL` register (`SPSEL` bit) via inline assembly to force Thread Mode execution to transition from MSP to PSP at runtime.
- **UART Diagnostic Logging**: Configured USART2 (`115200` baud) to capture and stream real-time register states and stack pointer addresses before and after the transition.

---

###  Terminal Diagnostic Output
```text
==================================================
STM32F401 DAY 70: MSP & PSP STACK ANALYSIS
==================================================
[INFO] Initial state (Default Thread Mode -> MSP):
[INFO] MSP: 0x20017F40 | PSP: 0x00000000 | CONTROL: 0x00000000
[INFO] Switched to PSP via CONTROL register:
[INFO] MSP: 0x20017FE0 | PSP: 0x20017F40 | CONTROL: 0x00000002
```

### Conclusion

Day 070 successfully lays the foundational brick for custom RTOS kernel development by validating the dual stack pointer architecture of the ARM Cortex-M4. By empirically demonstrating the live transition from the Main Stack Pointer (MSP) to the Process Stack Pointer (PSP) via the CONTROL register manipulation, this milestone proves that application threads can be safely isolated in their own private stack frames.