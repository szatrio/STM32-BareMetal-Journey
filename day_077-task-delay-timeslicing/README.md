# Day 077: Non-Blocking Task Delay & Time Slicing Architecture

### Objective

Upgrade the custom bare-metal RTOS kernel to support non-blocking task delays. This milestone introduces the `TASK_BLOCKED` execution state and a master task registry, replacing CPU-wasting busy-wait loops with kernel-managed tick countdowns and clean queue rotation.

---

### Key Technical Implementations

- **Extended TCB & Blocked State (`rtos_task.c` / `rtos_task.h`)**: 
  - Added `xTicksToDelay` field to the `TCB_t` structure and introduced `TASK_BLOCKED` to the `TaskState_t` enum.
- **`RTOS_Task_Delay()` API**: 
  - Allows the running task to yield control voluntarily, setting its delay ticks, moving its state to `TASK_BLOCKED`, and temporarily removing it from the ready ring buffer tail.
- **Tick Management & Master Task Registry (`RTOS_IncTick`)**: 
  - Implemented a system-wide task registry (`pxTaskRegistry`) to track all created tasks. The SysTick 1ms heartbeats decrement delay counters and automatically transition expired tasks back to `TASK_READY`.
- **Seamless Context Resumption**: 
  - Woken tasks are safely pushed back into the active ring buffer ready queue without interrupting system stability.

---

### Terminal Diagnostic Output

```text
==================================================
STM32F401 DAY 77: TASK DELAY & BLOCKED STATE
==================================================
[SUCCESS] Starting RTOS with Blocked State...
[Task 1] Running...
[Task 2] Running...
[Task 1] Running...
[Task 2] Running...
```

### Conclusion

Day 077 successfully transforms the RTOS scheduler into an event-aware, CPU-efficient operating system kernel. By managing blocked states and timed wakeups through hardware ticks, the system prevents wasted processing cycles and paves the way for advanced priority-based preemption in the next milestones.