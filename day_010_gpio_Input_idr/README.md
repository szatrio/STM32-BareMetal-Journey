# Day 009: Software Delay and Compiler Optimization

### Objective
Experimenting with software-based delays (for-loop) to understand their limitations and observing how compiler optimization levels impact execution timing.

### Key Learnings
- **Non-deterministic Timing:** Software delays are highly unstable. They depend on CPU clock speed and compiler configuration, making them unreliable for time-critical tasks.
- **Compiler's Logic:** Changing optimization levels (e.g., -O0 to -Os) alters the assembly output, causing significant fluctuations in execution time even if the for-loop count remains the same.
- **The Lesson:** This experiment confirmed why Hardware Timers (SysTick/TIMx) are the industry standard for firmware, rather than relying on CPU-cycle-dependent loops.

### Experiment Results

| Optimization Level | Iteration Count | Measured Time (s) |
| :--- | :--- | :--- |
| None (-O0) | 5,000,000 | 3.38s |
| Optimize for Size (-Os) | 5,000,000 | 2.88s |

### Hardware/Registers Involved
- **Registers:** N/A (Focus was strictly on compiler logic and CPU instruction execution).

### Conclusion
For-loop delays are unreliable and should be avoided in production firmware. Future projects will transition to using Hardware Timers for precise timing control.