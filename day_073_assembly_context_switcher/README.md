# Day 073: Assembly Context Switcher (PendSV_Handler) Implementation

### Objective

Implement the core low-level context-switching mechanism using naked assembly functions within the custom RTOS kernel. This milestone enables safe register-level preservation (`R4-R11`) and stack pointer (`PSP`) swapping during the PendSV exception, bridging low-level hardware manipulation with higher-level task pointer tracking.

---

### Key Technical Implementations

- **Global Task Tracking Pointers (`rtos_task.c` / `rtos_task.h`)**: Declared and defined `pxCurrentTaskPtr` and `pxNextTaskPtr` to track the active running task and upcoming task targets, avoiding multi-definition compilation errors via `extern` linkage.
- **Naked Assembly `PendSV_Handler`**: 
  - Saved the active task's software-managed registers (`R4-R11`) onto its private stack using `STMDB` via the Process Stack Pointer (`PSP`).
  - Updated the active TCB's stack pointer reference (`pStackPointer`) with the modified PSP value.
  - Invoked the C helper function `RTOS_SwitchTaskPointer` to update active task tracking pointers.
  - Restored software-managed registers (`R4-R11`) from the next task's stack using `LDMIA`, updated the CPU `PSP` register, and safely returned to thread mode via `BX r14`.
- **`main.c` Integration**: Updated initialization sequence to assign the initial active task (`pxCurrentTaskPtr = &sample_task`) prior to enabling the system timer and exception handlers.

---

### Terminal Diagnostic Output

```text
==================================================
STM32F401 DAY 73: ASSEMBLY CONTEXT SWITCHER
==================================================
[SUCCESS] Assembly PendSV context switcher ready.
```

### Conclusion

Day 073 successfully establishes the beating heart of the custom RTOS kernel. By combining assembly-level precision for register context saving/restoring with modular C pointer handling, the system is now fully equipped to perform automated, multi-task switching in the upcoming integration phase.