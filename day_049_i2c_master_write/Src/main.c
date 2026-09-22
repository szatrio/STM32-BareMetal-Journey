#include "stm32f401_system.h"
#include "stm32f401_gpio.h"
#include "stm32f401_i2c.h"

#define BME280_I2C_ADDR   0x77
#define BME280_REG_ID     0xD0

int main(void)
{
    FPU_Enable();
    I2C1_Init();

    uint8_t reg_addr = BME280_REG_ID;

    while (1)
    {
        // Send data repeatedly every ~200ms
        I2C_WriteByte(I2C1, BME280_I2C_ADDR, reg_addr);

        // Simple software delay
        for (volatile uint32_t i = 0; i < 200000; i++);
    }
}
