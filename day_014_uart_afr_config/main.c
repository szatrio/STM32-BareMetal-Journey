#include "stm32f401_gpio.h"

#define LED_PIN    5
#define BUTTON_PIN 13 // button pin 13 on port C, so use GPIOC

// PIN UART2
#define UART2_TX_PIN 2  // PA2
#define UART2_RX_PIN 3  // PA3

static void System_Init(void)
{
    // 1. Enable Clock GPIOA & GPIOC
    RCC_EnableGPIOClock();

    // 2. Setup LED (PA5)
    GPIO_Init_t led_config = {
        .Pin   = LED_PIN,
        .Mode  = GPIO_MODE_OUTPUT,
        .OType = GPIO_OTYPE_PUSHPULL,
        .Pull  = GPIO_PUPDR_NOPULLUPDOWN
    };
    GPIO_Init(GPIOA, &led_config);

    // 3. Setup Button (PC13)
    GPIO_Init_t button_config = {
        .Pin   = BUTTON_PIN,
        .Mode  = GPIO_MODE_INPUT,
        .OType = GPIO_OTYPE_PUSHPULL,
        .Pull  = GPIO_PUPDR_NOPULLUPDOWN
    };
    GPIO_Init(GPIOC, &button_config);

    // a. Set Mode PA2 & PA3 ke Alternate Function (GPIO_MODE_ALT)
	GPIO_SetMode(&GPIOA->MODER, UART2_TX_PIN, GPIO_MODE_ALT);
	GPIO_SetMode(&GPIOA->MODER, UART2_RX_PIN, GPIO_MODE_ALT);

	// b. Set Alternate Function ke AF7 (USART2)
	GPIO_SetAltFunction(GPIOA, UART2_TX_PIN, 7); // PA2 -> AF7 (USART2_TX)
	GPIO_SetAltFunction(GPIOA, UART2_RX_PIN, 7); // PA3 -> AF7 (USART2_RX)
}

int main(void)
{
	System_Init();

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
        Delay_Simple(2000);
    }
}
