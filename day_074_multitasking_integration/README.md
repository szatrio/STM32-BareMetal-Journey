# Day 074: Multitasking Integration & Round-Robin Scheduler

### Objective

Achieve full multitasking integration within the custom bare-metal RTOS kernel for the STM32F401. This milestone transitions the system from isolated task creation and single-task boot routines into a fully automated, cooperative/preemptive round-robin scheduler driven by the 1ms SysTick heartbeat, safely launched via Supervisor Call (`SVC`).

---

### Key Technical Implementations

- **Supervisor Call (`SVC_Handler`) Boot Mechanism (`rtos_task.c`)**: Implemented a naked `SVC_Handler` to cleanly transition execution from Thread Mode (using MSP) to Handler Mode, safely popping Task 1's initial stack frame, switching the control register to `PSP`, and executing an exception return (`EXC_RETURN`) via `0xFFFFFFFD`.
- **Preemptive Round-Robin Scheduler (`rtos_task.c` / `stm32f401_systick.c`)**:
  - Integrated `RTOS_Scheduler()` inside the `SysTick_Handler` to trigger every 1ms.
  - Dynamically toggles active task pointers (`pxCurrentTaskPtr` and `pxNextTaskPtr`) between `task1` and `task2`.
  - Automatically pends the `PendSV` exception (`SCB_ICSR_PENDSVSET_Msk`) to initiate seamless low-level context switches.
- **Race Condition Protection (`cortex_m4_core.h` / `main.c`)**:
  - Added inline assembly macros (`_disable_irq()` / `_enable_irq()`) utilizing `cpsid i` and `cpsie i`.
  - Wrapped shared UART logging output inside critical sections to prevent character interleaving between competing tasks.
- **Tick-Based Delaying (`SysTick_DelayMs`)**: Replaced blocking hardware busy-waits inside tasks with a robust millisecond delay engine driven by the system tick counter (`g_ticks`).

---

### Terminal Diagnostic Output

```text
==================================================
STM32F401 DAY 74: MULTITASKING INTEGRATION
==================================================
[SUCCESS] Starting RTOS Scheduler...
[Task 1] Running...
[Task 2] Running...
[Task 1] Running...
[Task 2] Running...
```

### Conclusion

Day 074 marks a major engineering milestone: a fully functional, custom-built, bare-metal RTOS kernel running on Cortex-M4 architecture. By mastering stack frame manipulation, Process Stack Pointers (PSP), SVC/PendSV exception handling, and critical section protection, the system successfully executes concurrent multi-tasking with seamless round-robin execution.