#include "stm32f401_registers.h"

#define LED_PIN    5
#define DELAY_VAL  1000000

int main(void)
{
    // Enable Clock GPIOA safely
	RCC_EnableGPIOClock();

    // Set Pin 5 as Output
    GPIOA_SetMode(LED_PIN, GPIO_MODE_OUTPUT);

    // Set Pin 5 No Pull Up/Pull Down
    GPIOA_SetPullUpDown(LED_PIN, GPIO_PUPDR_NOPULLUPDOWN);

    // Set Pin 5 Otype to Push Pull
    GPIOA_SetOutputOtype(LED_PIN, GPIO_OTYPE_PUSHPULL);

    while(1)
    {
        // Toggle Pin 5 (XOR operation)
    	GPIOA_TogglePin(LED_PIN);

        // Simple delay
        for(volatile uint32_t i = 0; i < DELAY_VAL; i++);
    }
}
