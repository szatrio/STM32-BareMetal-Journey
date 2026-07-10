#include "stm32f401_registers.h"

int main(void)
{
    // Enable Clock GPIOA safely
	RCC_EnableGPIOClock();

    // Set Pin 5 as Output
    GPIOA_SetMode(5, GPIO_MODE_OUTPUT);

    while(1)
    {
        // Toggle Pin 5 (XOR operation)
    	GPIOA_TogglePin(5);

        // Simple delay
        for(volatile uint32_t i = 0; i < 10000000; i++);
    }
}
