#include "stm32f401_gpio.h"

#define LED_PIN    5
#define BUTTON_PIN 13 // button pin 13 on port C, so use GPIOC

ButtonState_t is_button_pressed_debounced(uint8_t pin) {
    if (get_button_state(pin) == BUTTON_PRESSED) {
        Delay_Simple(20000); // Debouncing delay ~20ms
        if (get_button_state(pin) == BUTTON_PRESSED) {
            return BUTTON_PRESSED;
        }
    }
    return BUTTON_RELEASED;
}

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
    	// Read Button pin current status
    	uint8_t current_pin_val = GPIOC_ReadPin(BUTTON_PIN);

		// Using FSM Debounce (Non-blocking)
		if (Button_Update_FSM(current_pin_val)) {
			// Action runs ONLY on a single valid button press
			GPIO_TogglePin(LED_PIN);
		}

		// Very short sampling delay for the FSM tick (e.g., ~1ms - 5ms)
		// This keeps the CPU free and prevents utilization from endless looping
		Delay_Simple(2000);
    }
}
