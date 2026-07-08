#include "stm32f401_registers.h"

int main(void)
{
    // 1. Enable Clock GPIOA (RCC_AHB1ENR bit 0)
    RCC_AHB1ENR |= (1 << 0);

    // 2. Set Pin 5 as Output (MODER5 bit 10:11)
    GPIOA_MODER &= ~(3 << 10); // Clear bit 10 & 11
    GPIOA_MODER |=  (1 << 10); // Set bit 10 ke 1 (01 = Output)

    while(1)
    {
        // 3. Toggle LED (Pin 5)
        GPIOA_ODR ^= (1 << 5);

        // Simple delay
        for(int i = 0; i < 1000000; i++);
    }
}
