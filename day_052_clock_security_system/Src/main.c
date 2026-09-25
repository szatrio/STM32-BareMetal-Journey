#include "stm32f401_system.h"
#include "stm32f401_gpio.h"
#include "stm32f401_css.h"

#define LED_PIN     5 // PA5
#define BUZZER_PIN  3 // PB3

void Play_Tone(uint32_t duration_cycles) {
    for (uint32_t i = 0; i < duration_cycles; i++) {
        GPIO_TogglePin(GPIOB, BUZZER_PIN);
        // Short inner delay to create an audible frequency tone (~1-2 kHz range)
        for (volatile uint32_t d = 0; d < 500; d++);
    }
}

int main(void)
{
    FPU_Enable();
    RCC_EnableGPIOClock();

    // Initialize LED Pin
    GPIO_Init_t led_init = {
        .Pin   = LED_PIN,
        .Mode  = GPIO_MODE_OUTPUT,
        .OType = GPIO_OTYPE_PUSHPULL,
        .Pull  = GPIO_PUPDR_NOPULLUPDOWN
    };
    GPIO_Init(GPIOA, &led_init);

    // Configure Buzzer Pin (PB3) as Push-Pull Output
	GPIO_Init_t buzzer_init = {
		.Pin   = BUZZER_PIN,
		.Mode  = GPIO_MODE_OUTPUT,
		.OType = GPIO_OTYPE_PUSHPULL,
		.Pull  = GPIO_PUPDR_NOPULLUPDOWN
	};
	GPIO_Init(GPIOB, &buzzer_init);

    // Try to initialize HSE and activate CSS
    bool css_active = CSS_Init();

    if (!css_active) {
        // Fallback or Error State: HSE failed to start / CSS couldn't be enabled.
        // The system remains running on HSI (Internal Clock) safely.
        // Here we blink the LED rapidly 5 times to signal a startup clock warning.
        for (int i = 0; i < 10; i++) {
            GPIO_TogglePin(GPIOA, LED_PIN);
            Delay_Simple(1000000);
        }
    }

    while (1){
    	if (css_active) {
			// =================================================================
			// NORMAL OPERATING MODE (HSE + CSS Operational)
			// =================================================================
			// - LED stays solidly ON (Solid state indicates stable external clock)
			// - Buzzer emits an active periodic status tone

			GPIO_WritePin(GPIOA, LED_PIN, 1); // Turn LED Solid ON

			Play_Tone(300);                   // Play status sound pulse
			Delay_Simple(1000000UL);          // Interval delay before next tone

		} else {
			// =================================================================
			// FALLBACK / ERROR WARNING MODE (HSI Mode Active due to CSS Fault)
			// =================================================================
			// - Buzzer is completely silenced (Muted)
			// - LED blinks continuously as a visual fault warning indicator

			GPIO_WritePin(GPIOB, BUZZER_PIN, 0); // Silence buzzer

			GPIO_TogglePin(GPIOA, LED_PIN);      // Blink warning LED
			Delay_Simple(200000UL);              // Medium blink interval
		}
    }
}
