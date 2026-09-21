#include "stm32f401_i2c.h"

void I2C1_Init(void) {
    // 1. Enable clock for GPIOB and I2C1 peripherals
    RCC_AHB1ENR |= RCC_GPIOB_EN;
    RCC_APB1ENR |= RCC_APB1ENR_I2C1EN;

    // 2. Configure PB6 (SCL) and PB7 (SDA) pins
    // Set mode to Alternate Function
    GPIOB->MODER &= ~((3UL << (6 * 2)) | (3UL << (7 * 2)));
    GPIOB->MODER |=  ((2UL << (6 * 2)) | (2UL << (7 * 2)));

    // Output type: Open-Drain (Mandatory for I2C)
    GPIOB->OTYPER |= (1UL << 6) | (1UL << 7);

    // Enable internal pull-up resistors
    GPIOB->PUPDR &= ~((3UL << (6 * 2)) | (3UL << (7 * 2)));
    GPIOB->PUPDR |=  ((1UL << (6 * 2)) | (1UL << (7 * 2)));

    // Output speed: High Speed
    GPIOB->OSPEEDR |= ((2UL << (6 * 2)) | (2UL << (7 * 2)));

    // Select Alternate Function AF4 (I2C1) in AFRL register (Pins 0-7)
    GPIOB->AFR[0] &= ~((0xFUL << (6 * 4)) | (0xFUL << (7 * 4)));
    GPIOB->AFR[0] |=  ((4UL << (6 * 4)) | (4UL << (7 * 4)));

    // 3. Reset and disable I2C1 peripheral before configuration
    I2C1->CR1 &= ~I2C_CR1_PE;

    // 4. Set Peripheral Clock Frequency in CR2 (HSI = 16 MHz)
    I2C1->CR2 &= ~I2C_CR2_FREQ_MASK;
    I2C1->CR2 |= 16U; // 16 MHz

    // 5. Set Clock Control Register (CCR) for Standard Mode (100 kHz)
    // CCR = 16 MHz / (2 * 100 kHz) = 80
    I2C1->CCR &= ~I2C_CCR_FS; // 0 = Standard Mode
    I2C1->CCR |= 80U;

    // 6. Set Maximum Rise Time (TRISE)
    // TRISE = (1000 ns / 62.5 ns) + 1 = 17
    I2C1->TRISE = 17U;

    // 7. Set Own Address Register 1 (OAR1)
    // Bit 14 must always be kept as 1 according to STM32 Reference Manual
    I2C1->OAR1 = (1UL << 14);

    // 8. Enable I2C1 peripheral
    I2C1->CR1 |= I2C_CR1_PE;
}

// Generate START condition on I2C bus
void I2C1_Start(void) {
    // 1. Generate START condition
    I2C1->CR1 |= I2C_CR1_START;

    // 2. Wait until Start Bit (SB) flag is set in SR1 register
    while (!(I2C1->SR1 & I2C_SR1_SB));
}

// Generate STOP condition on I2C bus
void I2C1_Stop(void) {
    // Generate STOP condition
    I2C1->CR1 |= I2C_CR1_STOP;
}
