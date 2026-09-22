#include "stm32f401_system.h"
#include "stm32f401_gpio.h"
#include "stm32f401_i2c.h"

#define BME280_I2C_ADDR   0x76  // Default I2C address for BME280
#define BME280_REG_ID     0xD0  // Chip ID register address

int main(void)
{
    // Enable Hardware FPU
    FPU_Enable();

    // Initialize I2C Peripheral
    I2C1_Init();

    // Send register address 0xD0 (Chip ID) to BME280
    uint8_t reg_addr = BME280_REG_ID;
    I2C_WriteByte(I2C1, BME280_I2C_ADDR, reg_addr);

    while (1)
    {
        // Idle loop
    }
}
