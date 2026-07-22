# Day 013: Struct-Based Register Mapping & Driver Modularization

### Objective
Refactor the bare-metal firmware architecture by transitioning low-level register management from raw macros to structured pointers (`GPIO_TypeDef`), implementing robust `GPIO_Init_t` configuration blueprints, and modularizing the driver source files (`.c` and `.h`) following professional embedded systems standards.

### Key Learnings

- **Struct-Based Register Mapping:** Replaced scattered raw pointer macros with standard CMSIS-style `GPIO_TypeDef` structures, aligning memory offsets precisely with the STM32F401 hardware datasheet (including proper alignment padding for unused registers like `OSPEEDR`).
- **Configuration Blueprints (`GPIO_Init_t`):** Implemented generic pin configuration data templates to handle pin modes, output types, and pull-up/pull-down settings cleanly before registration.
- **Driver Modularization (`.c` / `.h` Separation):** Decoupled interface declarations from hardware-specific logic by moving function implementations out of header files and into dedicated source modules (`stm32f401_gpio.c`).
- **File-Level Encapsulation:** Utilized `static` storage specifiers for internal state variables (such as `gpio_status_cache`) to prevent unauthorized cross-module access and maintain clean separation of concerns.

### Technical Implementation

- **Precision Struct Offset Management:** Defined a clean hardware mapping structure to ensure correct memory-mapped I/O execution:
    ```c
    typedef struct {
        volatile uint32_t MODER;    // 0x00
        volatile uint32_t OTYPER;   // 0x04
        volatile uint32_t OSPEEDR;  // 0x08 (Reserved/Padding slot)
        volatile uint32_t PUPDR;    // 0x0C
        volatile uint32_t IDR;      // 0x10
        volatile uint32_t ODR;      // 0x14
        volatile uint32_t BSRR;     // 0x18
    } GPIO_TypeDef;
    ```
- **Unified Initialization Interface:** Created a centralized `GPIO_Init()` routine to parse user-defined configurations and map them dynamically to any target port (`GPIOA`, `GPIOC`, etc.).
- **Atomic Bit Operations & Caching:** Maintained fast, non-blocking toggle states using local memory caching combined with the STM32 Bit Set/Reset Register (`BSRR`).

### Hardware/Registers Involved

- **Registers:**
    - `GPIO_TypeDef->MODER`: Port mode configuration (Input/Output).
    - `GPIO_TypeDef->OTYPER`: Output type selection (Push-Pull / Open-Drain).
    - `GPIO_TypeDef->PUPDR`: Internal pull-up and pull-down resistor configuration.
    - `GPIO_TypeDef->BSRR`: Atomic pin set and reset manipulation.

### Conclusion
Day 13 establishes professional code organization standards. By evolving from messy inline header functions into clean, decoupled `.c` drivers with structured initialization blueprints, the firmware architecture now mirrors industrial-grade hardware abstraction layers (HAL), making the codebase highly scalable and maintainable.