#include "stm32f401_gpio.h"
#include "stm32f401_nvic.h"

#define LED_PIN         5   // PA5
#define EXTI0_IRQ_NUM   6   // Vector position for EXTI0

int main(void) {
    // 1. Enable GPIOA Clock & Initialize LED Pin
    RCC_EnableGPIOClock();

    GPIO_Init_t led_cfg = {
        .Pin   = LED_PIN,
        .Mode  = GPIO_MODE_OUTPUT,
        .OType = GPIO_OTYPE_PUSHPULL,
        .Pull  = GPIO_PUPDR_NOPULLUPDOWN
    };
    GPIO_Init(GPIOA, &led_cfg);

    // Keep LED OFF initially
    GPIO_WritePin(GPIOA, LED_PIN, 0);
    Delay_Simple(2000000);

    // --- TEST 1: Enable IRQ via ISER ---
    NVIC_EnableIRQ_Custom(EXTI0_IRQ_NUM);
    if (NVIC_GetEnableIRQ_Custom(EXTI0_IRQ_NUM) == 1) {
        // Turn ON LED for a duration if ISER set successfully
        GPIO_WritePin(GPIOA, LED_PIN, 1);
        Delay_Simple(2000000);
    }

    // --- TEST 2: Prove 'Write 0 has no effect' on ISER ---
    NVIC->ISER[0] &= ~(1U << EXTI0_IRQ_NUM); // Attempt to write bit 0
    if (NVIC_GetEnableIRQ_Custom(EXTI0_IRQ_NUM) == 1) {
        // Blink LED 3 times quickly: proves bit 0 writing did NOT disable IRQ
        for (uint8_t i = 0; i < 3; i++) {
            GPIO_WritePin(GPIOA, LED_PIN, 0);
            Delay_Simple(500000);
            GPIO_WritePin(GPIOA, LED_PIN, 1);
            Delay_Simple(500000);
        }
    }

    // --- TEST 3: Disable IRQ via ICER ---
    NVIC_DisableIRQ_Custom(EXTI0_IRQ_NUM);
    if (NVIC_GetEnableIRQ_Custom(EXTI0_IRQ_NUM) == 0) {
        // Turn OFF LED: proves ICER successfully disabled IRQ
        GPIO_WritePin(GPIOA, LED_PIN, 0);
    }

    while (1) {
        // Main loop idle - execution complete
    }
}
