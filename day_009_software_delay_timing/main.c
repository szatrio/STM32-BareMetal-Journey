#include "stm32f401_gpio.h"

#define LED_PIN    5

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
    	// Use atomic function (BSRR)
    	GPIO_TogglePin(LED_PIN);

    	Delay_Simple(5000000);
    }
}
