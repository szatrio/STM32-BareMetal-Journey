#include "stm32f401_system.h"
#include "stm32f401_gpio.h"
#include "stm32f401_css.h"

#define LED_PIN     5 // PA5)

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

    // Try to initialize HSE and activate CSS
    bool css_active = CSS_Init();

    if (!css_active) {
        // Fallback or Error State: HSE failed to start / CSS couldn't be enabled.
        // The system remains running on HSI (Internal Clock) safely.
        // Here we blink the LED rapidly 5 times to signal a startup clock warning.
        for (int i = 0; i < 5; i++) {
            GPIO_TogglePin(GPIOA, LED_PIN);
            Delay_Simple(100000);
        }
    }

    while (1){
        // Normal Operation Loop
        // If css_active is true, system runs on HSE guarded by CSS.
        // If css_active is false, system runs safely on HSI fallback.
        GPIO_TogglePin(GPIOA, LED_PIN);

        // Adjust delay depending on whether HSE or HSI is active
        Delay_Simple(css_active ? 2000000UL : 500000UL);
    }
}
