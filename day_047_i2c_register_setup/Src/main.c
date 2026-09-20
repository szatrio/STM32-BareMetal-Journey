#include "stm32f401_registers.h"
#include "stm32f401_system.h"
#include "stm32f401_i2c.h"

int main(void) {
    // 1. Initialize System Core
    FPU_Enable();

    // 2. Initialize I2C1 Peripheral (PB6: SCL, PB7: SDA @ 100 kHz)
    I2C1_Init();

    // 3. Main Loop
    while (1) {
        __NOP();
    }
}
