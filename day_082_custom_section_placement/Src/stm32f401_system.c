#include "stm32f401_registers.h"
#include "stm32f401_system.h"

void FPU_Enable(void) {
    SCB->CPACR |= (SCB_CPACR_CP10_FULL | SCB_CPACR_CP11_FULL);
}
