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

    // 3. Reset and disable I2C1 peripheral state machine
    I2C1->CR1 |= (1UL << 15);   // Set SWRST (Software Reset)
    I2C1->CR1 &= ~(1UL << 15);  // Clear SWRST
    I2C1->CR1 &= ~I2C_CR1_PE;   // Disable I2C1 peripheral

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
void I2C_Start(I2C_TypeDef *I2Cx) {
    // 1. Generate START condition
    I2Cx->CR1 |= I2C_CR1_START;

    // 2. Wait until Start Bit (SB) flag is set in SR1 register
    while (!(I2Cx->SR1 & I2C_SR1_SB));
}

// Generate STOP condition on I2C bus
void I2C_Stop(I2C_TypeDef *I2Cx) {
    // Generate STOP condition
    I2Cx->CR1 |= I2C_CR1_STOP;
}

/**
 * @brief  Write multiple bytes to a slave device on I2C bus
 */
void I2C_WriteData(I2C_TypeDef *I2Cx, uint8_t slave_addr, uint8_t *pData, uint32_t len)
{
    // 1. Send START condition
    I2C_Start(I2Cx);

    // 2. Send 7-bit slave address with Write bit (R/W = 0)
    I2Cx->DR = (slave_addr << 1) & ~(1 << 0);

    // 3. Wait until ADDR flag is set (Slave responded with ACK) or AF (NACK) occurs
	while (!(I2Cx->SR1 & (1 << 1)))
	{
		if (I2Cx->SR1 & (1 << 10))
		{
			I2Cx->SR1 &= ~(1 << 10);
			I2C_Stop(I2Cx);
			return;
		}
	}

    // 4. Clear ADDR flag by reading SR1 followed by SR2
    uint32_t dummy_read = I2Cx->SR1;
    dummy_read = I2Cx->SR2;
    (void)dummy_read; // Avoid unused variable warning

    // 5. Send data buffer byte-by-byte
    for (uint32_t i = 0; i < len; i++)
    {
        // Wait until TXE (Transmit Data Register Empty) flag is set
        while (!(I2Cx->SR1 & (1 << 7)));

        // Load data byte into Data Register
        I2Cx->DR = pData[i];
    }

    // 6. Wait until BTF (Byte Transfer Finished) flag is set
    while (!(I2Cx->SR1 & (1 << 2)));

    // 7. Send STOP condition
    I2C_Stop(I2Cx);
}

/**
 * @brief  Write a single byte to a slave device
 */
void I2C_WriteByte(I2C_TypeDef *I2Cx, uint8_t slave_addr, uint8_t data)
{
    I2C_WriteData(I2Cx, slave_addr, &data, 1);
}
