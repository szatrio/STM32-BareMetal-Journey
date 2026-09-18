# Day 046: Non-Blocking Background Sensor Processing

### Objective

Refactor sensor telemetry processing by structuring raw ADC data conversion into calibrated physical units (Voltage and Position Percentage) using a non-blocking, event-driven architecture. By leveraging system calibration macros (`VREF`, `ADC_MAX_VAL`) and DMA status flags, this completes the transition from low-level DMA setup to application-level background sensor telemetry.

### System Architecture & Execution Pipeline

```text
+-----------------------------------------------------------------------+
|                 Hardware Layer (Background Activity)                  |
|  ADC1 (Raw: 0-4095) ---> DMA2 Stream 0 ---> SRAM (adc_dma_buffer)     |
+-----------------------------------------------------------------------+
                                   |
                                   v (Hardware sets TCIF0 Flag)
+-----------------------------------------------------------------------+
|               Application Layer (Event-Driven Main Loop)              |
|  1. Event Check: if (DMA_GetTCFlag(DMA2, 0))                          |
|  2. Structured Scaling: Voltage (0.0V - 3.3V) & Position (0% - 100%)  |
|  3. Telemetry Output: Format and transmit via UART                    |
|  4. Hardware ACK: DMA_ClearTCFlag(DMA2, 0)                            |
|                                                                       |
|  * CPU remains 100% free to execute other tasks if TCIF0 == 0         |
+-----------------------------------------------------------------------+
```
### Key Technical Implementations

* **Event-Driven Application Architecture**  
  Separated hardware data acquisition from software data processing. The CPU exclusively executes telemetry calculations and logging when a valid DMA transfer is explicitly flagged by the hardware, eliminating polling delays.

* **Structured Physical Data Abstraction**  
  Refactored hardcoded inline voltage math into system calibration macros (`VREF`, `ADC_MAX_VAL`) and integrated physical position percentage scaling (0% - 100%) to represent continuous sensor displacement.

* **Zero-Blocking Execution Validation**  
  Validated that the CPU execution remains entirely unblocked while waiting for the ADC-DMA pipeline to complete. The `while(1)` loop continuously cycles at maximum speed, allowing for seamless integration of future concurrent tasks.

---

### Comparative Analysis: Day 045 vs. Day 046

| Metric / Feature | Day 045 (Hardware Handshake) | Day 046 (Sensor Processing) |
| :--- | :--- | :--- |
| **Primary Focus** | Infrastructure & Register Verification | Application Layer & Telemetry Abstraction |
| **Data Scaling** | Raw ADC & Basic Voltage | Voltage & Position Percentage (0% - 100%) |
| **Code Abstraction** | Hardcoded Inline Math (`3.3f`, `4095.0f`) | Structured Macros (`VREF`, `ADC_MAX_VAL`) |
| **Execution Paradigm** | Validating safe SRAM access | Executing event-driven sensor telemetry |

---

### Conclusion

Day 046 finalizes the robust ADC-to-DMA telemetry architecture. By leveraging the hardware handshakes established in previous days, the firmware now processes and logs physical sensor data in the background with zero execution blocking. This marks the completion of the DMA data acquisition pipeline.