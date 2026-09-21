#include "stm32f401_registers.h"
#include "stm32f401_system.h"
#include "stm32f401_i2c.h"

int main(void) {
    FPU_Enable();
    I2C1_Init();

    // Test START & STOP sequence
    I2C1_Start();
    I2C1_Stop();

    while (1) {
        __NOP();
    }
}
