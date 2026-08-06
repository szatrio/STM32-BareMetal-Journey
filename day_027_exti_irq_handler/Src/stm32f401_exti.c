#include "stm32f401_exti.h"
#include "stm32f401_gpio.h"

#define LED_PIN    		 5

void EXTI_PC13_Init(void) {
    // enable clock SYSCFG (APB2 Bus)
    RCC_APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    // connecting pin PC13 to EXTI Line 13 via SYSCFG_EXTICR4 (index [3])
    SYSCFG->EXTICR[3] &= ~(0xFU << 4); // Clear field EXTI13
    SYSCFG->EXTICR[3] |=  (0x2U << 4); // 0x2 = Port C

    // Set Falling Edge Trigger (PC13 Active LOW)
    EXTI->FTSR |= EXTI_FTSR_TR13;

    // open masking Interuption at Line 13
    EXTI->IMR |= EXTI_IMR_MR13;
}

/**
 * @brief ISR specific for EXTI Line 10 to 15 (PC13 include in this)
 */
void EXTI15_10_IRQHandler(void) {
    if (EXTI->PR & EXTI_PR_PR13) {
        GPIOA->ODR ^= (1U << LED_PIN);

        // Clear Pending Flag Line 13 (Write 1 to Clear)
        EXTI->PR = EXTI_PR_PR13;
    }
}
