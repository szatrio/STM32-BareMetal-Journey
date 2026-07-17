#include "stm32f401_gpio.h"

#define LED_PIN    5
#define BUTTON_PIN 13 // button pin on port C, so use GPIOC

int main(void)
{
    // Enable Clock GPIOA safely
	RCC_EnableGPIOClock();

	// Setup LED (PA5)
	GPIO_SetMode(&GPIOA_MODER, LED_PIN, GPIO_MODE_OUTPUT);

	// Setup Button (PC13)
	GPIO_SetMode(&GPIOC_MODER, BUTTON_PIN, GPIO_MODE_INPUT);

    // Set Pin 5 No Pull Up/Pull Down
    GPIOA_SetPullUpDown(LED_PIN, GPIO_PUPDR_NOPULLUPDOWN);

    // Set Pin 5 Otype to Push Pull
    GPIOA_SetOutputOtype(LED_PIN, GPIO_OTYPE_PUSHPULL);

    while(1)
    {
    	// Read Pin Voltage from GPIOC
    	if (GPIOC_ReadPin(BUTTON_PIN) == 0) {
			GPIOA_WritePin(LED_PIN, 1);
		} else {
			GPIOA_WritePin(LED_PIN, 0);
		}
    }
}
