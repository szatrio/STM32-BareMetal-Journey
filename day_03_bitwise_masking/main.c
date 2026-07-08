#include "stm32f401_registers.h"

int main(void)
{
    // Enable Clock GPIOA safely
    RCC_ENABLE_GPIOA();

    // Set Pin 5 as Output
    GPIOA_MODER &= ~(3 << 10);
    GPIOA_MODER |=  (1 << 10);

    while(1)
    {
        // Toggle Pin 5 (XOR operation)
        GPIOA_ODR ^= (1 << 5);

        // Simple delay
        for(volatile int i = 0; i < 500000; i++);
    }
}
