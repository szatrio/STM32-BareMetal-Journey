#include "stm32f401_gpio.h"
#include "stm32f401_nvic.h"
#include "stm32f401_exti.h"
#include "stm32f401_systick.h"

#define LED_PIN          5   // PA5
#define BUTTON_PIN       13  // PC13
#define EXTI15_10_IRQn   40  // IRQ Position for EXTI Line 10-15

int main(void) {
    // 1. Clock & Init LED (PA5)
    RCC_EnableGPIOClock();

    GPIO_Init_t led_cfg = {
        .Pin   = LED_PIN,
        .Mode  = GPIO_MODE_OUTPUT,
        .OType = GPIO_OTYPE_PUSHPULL,
        .Pull  = GPIO_PUPDR_NOPULLUPDOWN
    };
    GPIO_Init(GPIOA, &led_cfg);

    // Init button as input (PC13)
    GPIO_Init_t btn_cfg = {
        .Pin   = BUTTON_PIN,
        .Mode  = GPIO_MODE_INPUT,
        .Pull  = GPIO_PUPDR_NOPULLUPDOWN
    };
    GPIO_Init(GPIOC, &btn_cfg);

    // Initialize EXTI hardware registers (SYSCFG, FTSR, IMR)
    EXTI_PC13_Init();


    // 2. Initialize SysTick Timer for 1ms ticks (16000 cycles @ 16MHz HSI Clock)
    SysTick_Init(16000);

    // Enable IRQ 40 (EXTI15_10) in NVIC
    NVIC_EnableIRQ_Custom(EXTI15_10_IRQn);

    // Main event loop
    while (1) {
        if (g_btn_event_flag) {
            g_btn_event_flag = 0;          // Reset flag
            GPIOA->ODR ^= (1U << LED_PIN); // Toggle LED
        }

        // blocking timer: toggle led automatically every 500ms via SysTick
        GPIOA->ODR ^= (1U << LED_PIN);
		SysTick_DelayMs(500);
    }
}
