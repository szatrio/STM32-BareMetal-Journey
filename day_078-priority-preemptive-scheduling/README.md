# Day 078: Priority-Based Preemptive Scheduling

### Objective

Upgrade the custom bare-metal RTOS kernel from cooperative round-robin scheduling to a robust **Priority-Based Preemptive Scheduling** architecture. This milestone ensures that urgent, high-priority tasks can immediately interrupt lower-priority tasks the moment they transition from a blocked state to ready.

---

### Key Technical Implementations

- **Priority-Aware Task Creation (`rtos_task.c` / `rtos_task.h`)**: 
  - Updated `RTOS_Task_Create()` to accept a custom `priority` parameter, assigning explicit urgency levels (where lower numerical values represent higher execution priority) inside the task control block (`TCB_t`).
- **Instant Preemption Logic (`RTOS_IncTick`)**: 
  - Enhanced the tick interrupt handler to evaluate task priorities upon wake-up. If a newly unblocked task holds a higher priority than the currently running task (`pxCurrentTaskPtr`), the kernel flags an immediate context switch.
- **Forced Context Switch Trigger (`PendSV`)**: 
  - Leveraged the `SCB->ICSR` register to programmatically pend a `PendSV` exception, allowing high-priority tasks to preempt lower-priority routines instantly without waiting for time-slice expiration.

---

### Terminal Diagnostic Output

```text
==================================================
STM32F401 DAY 78: PRIORITY-BASED PREEMPTION
==================================================
[SUCCESS] Starting Preemptive RTOS Scheduler...
[Task 1] Running...
[Task 2] Running...
[Task 1] Running...
[Task 2] Running...
```

### Conclusion

Day 078 successfully establishes true real-time determinism in the custom RTOS kernel. By integrating priority levels and immediate preemption into the tick management workflow, the system can now guarantee that critical tasks take precedence over standard background routines instantaneously.