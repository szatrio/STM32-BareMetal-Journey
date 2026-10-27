# Day 075: Scalable Round-Robin Task Scheduler Architecture

### Objective

Upgrade the custom bare-metal RTOS kernel from hardcoded task-switching logic to a scalable, array-based Round-Robin task scheduler. This milestone abstracts task tracking into a dynamic ready task registry (`pxReadyTasks`), allowing the scheduler to manage multiple tasks through index rotation.

---

### Key Technical Implementations

- **Dynamic Task Registry (`rtos_task.c` / `rtos_task.h`)**: 
  - Introduced `pxReadyTasks[configMAX_TASKS]` array and `uxTaskRegistrySize` to hold registered tasks dynamically.
  - Added the `RTOS_Task_AddToList()` API to register created tasks into the scheduler queue before launching.
- **Index-Based Round-Robin Scheduler (`rtos_task.c`)**:
  - Replaced rigid `if-else` task toggles with a modulo-based index rotation (`uxCurrentTaskIndex = (uxCurrentTaskIndex + 1U) % uxTaskRegistrySize`).
  - Seamlessly updates `pxNextTaskPtr` to point to the next task in the queue upon every `SysTick` heartbeat.
- **`main.c` Refactoring**:
  - Decoupled task creation from scheduler execution by explicitly registering `task1` and `task2` via `RTOS_Task_AddToList()`.

---

### Terminal Diagnostic Output

```text
==================================================
STM32F401 DAY 75: ROUND-ROBIN SCHEDULER
==================================================
[SUCCESS] Starting Scalable RTOS Scheduler...
[Task 1] Running...
[Task 2] Running...
[Task 1] Running...
[Task 2] Running...
```

### Conclusion

Day 075 successfully transitions the custom RTOS kernel into a scalable operating system architecture. By decoupling task registration from execution logic, the kernel is now fully prepared to handle extended task counts, time-slicing states, and advanced scheduling algorithms in upcoming milestones.