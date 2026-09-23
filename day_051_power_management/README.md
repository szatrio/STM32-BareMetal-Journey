# Day 051: Power Management & Sleep Mode Wake-up via EXTI

### Objective

Implement bare-metal Power Management on the STM32F401RE by entering low-power Sleep Mode via the __WFI() instruction and configuring External Interrupts (EXTI Line 13) on pin PC13 (Nucleo Blue Button) to asynchronously wake up the CPU upon user interaction.

### System Architecture & Signaling Pipeline

```text
+-----------------------------------------------------------------------+
|                    Power Management & Wake Pipeline                   |
+-----------------------------------------------------------------------+
                                   |
                                   v
+-----------------------------------------------------------------------+
|                       1. Sleep Mode Entry                             |
|  - Enable APB1 Power Interface clock (PWR)                            |
|  - Clear SLEEPDEEP bit in SCB->SCR register                           |
|  - Execute WFI (Wait For Interrupt) instruction to halt CPU           |
+-----------------------------------------------------------------------+
                                   |
                                   v
+-----------------------------------------------------------------------+
|                    2. Asynchronous Wake-up Trigger                    |
|  - Physical Blue Button press pulls PC13 LOW (Falling Edge)           |
|  - SYSCFG routes Port C to EXTI Line 13                               |
|  - EXTI captures signal via FTSR and sets Pending Flag (PR13)         |
+-----------------------------------------------------------------------+
                                   |
                                   v
+-----------------------------------------------------------------------+
|                    3. ISR & Main Loop Resumption                      |
|  - CPU wakes up, jumps to EXTI15_10_IRQHandler                        |
|  - Clear pending flag and set g_btn_event_flag                        |
|  - Main loop resumes execution and triggers LED feedback              |
+-----------------------------------------------------------------------+
```

### Key Technical Implementations

* **Power Interface Clock & Sleep Mode Configuration**  
  Enabled the power interface clock via `RCC->APB1ENR` and cleared the `SLEEPDEEP` bit in the Cortex-M4 `SCB->SCR` register to ensure standard Sleep Mode entry when executing `__WFI()`.

* **SYSCFG Routing & EXTI Line Mapping**  
  Configured the system configuration controller (`SYSCFG`) and mapped `EXTICR4` (`EXTICR[3]`) to route EXTI Line 13 specifically to Port C (`GPIOC`), linking the physical user button to the interrupt subsystem.

* **Trigger Setup & NVIC Integration**  
  Configured `FTSR` for falling edge triggering (active-LOW button press) and enabled `IMR` for line 13. Activated the `EXTI15_10_IRQn` (IRQ 40) inside the ARM Nested Vectored Interrupt Controller (`NVIC->ISER[1]`) to handle low-latency wake-up events.

---

### Conclusion

Day 51 successfully integrates low-power system control with asynchronous hardware interrupts. By combining Sleep Mode execution with EXTI line 13 wake-up logic, the STM32F401RE can efficiently reduce dynamic power consumption while maintaining instant responsiveness to external user triggers.