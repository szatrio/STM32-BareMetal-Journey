#include "stm32f401_system.h"
#include "stm32f401_gpio.h"
#include "stm32f401_pwr.h"
#include "stm32f401_exti.h"

// Define LED pin (PA5 on Nucleo Board) and Button pin (PC13 on Nucleo Board)
#define LED_PIN     5
#define BTN_PIN     13

int main(void)
{
    FPU_Enable();
    RCC_EnableGPIOClock();

    // 1. Initialize LED Pin (PA5) as Push-Pull Output
    GPIO_Init_t led_init = {
        .Pin   = LED_PIN,
        .Mode  = GPIO_MODE_OUTPUT,
        .OType = GPIO_OTYPE_PUSHPULL,
        .Pull  = GPIO_PUPDR_NOPULLUPDOWN
    };
    GPIO_Init(GPIOA, &led_init);

    // 2. Initialize Button Pin (PC13) as Input (Active LOW with internal pull-up if needed,
    //    assuming physical button pulls low when pressed)
    GPIO_Init_t btn_init = {
		.Pin   = BTN_PIN,
		.Mode  = GPIO_MODE_INPUT,
		.OType = GPIO_OTYPE_PUSHPULL,
		.Pull  = GPIO_PUPDR_NOPULLUPDOWN
	};
	GPIO_Init(GPIOC, &btn_init);

	// 3. Initialize EXTI for User Button (PC13)
	EXTI_PC13_Init();

	while (1){
		// Turn ON LED to indicate the MCU is awake and running
		GPIO_WritePin(GPIOA, LED_PIN, 1);

		// Delay to keep the LED visible for a brief moment
		Delay_Simple(8000000);

		// Turn OFF LED before entering sleep mode
		GPIO_WritePin(GPIOA, LED_PIN, 0);

		// Enter Sleep Mode (CPU halts via WFI, power consumption drops)
		PWR_EnterSleepMode();

		// Once awakened by the button interrupt, the CPU resumes execution here
		if (g_btn_event_flag) {
			g_btn_event_flag = 0; // Reset the flag

			// Give visual feedback (e.g., fast flashes) confirming wake-up
			for (int i = 0; i < 3; i++) {
				GPIO_TogglePin(GPIOA, LED_PIN);
				Delay_Simple(500000);
			}
		}
	}
}
