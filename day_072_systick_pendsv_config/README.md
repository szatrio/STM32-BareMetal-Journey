# Day 072: SysTick Timer & PendSV Exception Configuration

### Objective

Establish the core hardware timing and exception infrastructure required for real-time operating system task scheduling. This milestone configures the SysTick timer to generate a periodic 1ms system heartbeat and sets the PendSV exception priority to the lowest level to ensure safe, non-disruptive context switching.

---

### Key Technical Implementations

- **SysTick Timer Initialization (`stm32f401_systick.c`)**: Configured the processor's system timer to reload periodically and trigger a 1ms interrupt (`SysTick_Handler`), advancing the global system tick counter (`g_ticks`).
- **PendSV Priority Configuration (`cortex_m4_core.h`)**: Programmatically modified the System Handlers Priority Register 3 (`SHPR3`) via the `SCB` structure to force the PendSV exception priority to the lowest level (`0xFF`), preventing context switches from disrupting critical hardware interrupts.
- **Exception Handler Stubs**: Implemented safe stub routines for both `SysTick_Handler` and `PendSV_Handler` to prevent system crashes (`HardFault`) prior to full scheduler and assembly switcher integration.

---

### Terminal Diagnostic Output

```text
==================================================
STM32F401 DAY 72: SYSTICK & PENDSV CONFIG
==================================================
[SUCCESS] SysTick & PendSV configured successfully.
```

### Conclusion

Day 072 successfully installs the fundamental timing and exception machinery for our custom RTOS kernel. By arming the 1ms SysTick heartbeat and securing the lowest priority level for the PendSV supervisor, the processor is now fully equipped to safely execute automated task switching in the upcoming milestones.