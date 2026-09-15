#include "stm32f401_registers.h"
#include "stm32f401_systick.h"
#include "stm32f401_system.h"
#include "stm32f401_dma.h"

int main(void) {
    // 1. Initialize system core
    FPU_Enable();
    SysTick_Init(16000);

    while (1) {
        SysTick_DelayMs(1000);
    }
}
