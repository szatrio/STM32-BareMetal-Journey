# Day 071: Task Control Block (TCB) & Independent Stack Allocation

### Objective

Design and implement the foundational data structures and stack initialization logic required for multi-threading. This milestone introduces the Task Control Block (TCB) and constructs an artificial hardware stack frame for ARM Cortex-M4 to prepare tasks for upcoming scheduler integration.

---

### Key Technical Implementations

- **Task Control Block (`rtos_task.h`)**: Defined the `TCB_t` structure containing tracking pointers for private stack memory, execution states (`TASK_READY`, `TASK_RUNNING`, etc.), and task priorities.
- **Static Stack Allocation**: Allocated a dedicated static memory array (`configSTACK_SIZE`) within each TCB instance to serve as the private stack workspace for individual tasks.
- **Artificial Stack Frame Initialization**: Programmatically populated the task's private stack with simulated hardware-saved registers (`xPSR`, `PC`, `LR`, `R12`, `R3`, `R2`, `R1`, `R0`) and software-saved registers (`R4-R11`), aligning precisely with the Cortex-M4 exception stacking architecture.
- **Diagnostic Validation (`main.c`)**: Verified task creation success and inspected initial stack pointer placement via USART2 diagnostic logs prior to runtime context switching.

---

### Terminal Diagnostic Output

```text
==================================================
STM32F401 DAY 71: TCB & STACK ALLOCATION
==================================================
[SUCCESS] Task TCB and stack frame initialized.
[INFO] TCB pStackPointer: 0x200003A8
```

### Conclusion

Day 071 establishes the core structural blueprint for thread management in our custom RTOS kernel. By encapsulating task execution context inside a TCB and pre-formatting its private stack memory with a valid Cortex-M4 stack frame, the system is now fully prepared for automated task scheduling and context switching via PendSV and SysTick exceptions.