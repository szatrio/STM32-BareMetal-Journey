#include "stm32f401_tim.h"
#include "stm32f401_gpio.h"

void TIM2_Timebase_Init(uint16_t psc, uint32_t arr) {
    // 1. Enable TIM2 peripheral clock on APB1 bus
	RCC_APB1ENR |= RCC_APB1ENR_TIM2EN;

    // 2. Load Prescaler and Auto-Reload values into registers
    TIM2->PSC = psc;
    TIM2->ARR = arr;

    // 3. Force an Update Generation (UG) event
    // Copies PSC and ARR values into hardware shadow registers immediately
    TIM2->EGR |= TIM_EGR_UG;

    // 4. Clear status register UIF flag caused by forced software update above
    TIM2->SR &= ~TIM_SR_UIF;

    // 5. Enable counter (Set CEN bit in Control Register 1)
    TIM2->CR1 |= TIM_CR1_CEN;
}

void TIM2_Timebase_IT_Init(uint16_t psc, uint32_t arr) {
    // 1. Enable TIM2 peripheral clock on APB1 bus
    RCC_APB1ENR |= RCC_APB1ENR_TIM2EN;

    // 2. Load Prescaler and Auto-Reload values (500 ms @ 16 MHz HSI)
    TIM2->PSC = psc;
    TIM2->ARR = arr;

    // 3. Force an Update Event to transfer PSC & ARR to shadow registers
    TIM2->EGR |= TIM_EGR_UG;

    // 4. Clear the UIF flag triggered by setting EGR_UG above
    TIM2->SR &= ~TIM_SR_UIF;

    // 5. Enable TIM2 Update Interrupt in the DIER register
    TIM2->DIER |= TIM_DIER_UIE;

    // 6. Enable TIM2 interrupt line in the Cortex-M NVIC using custom registers
    NVIC->ISER[TIM2_IRQn >> 5] |= (1U << (TIM2_IRQn & 0x1F));

    // 7. Enable TIM2 counter to start operation
    TIM2->CR1 |= TIM_CR1_CEN;
}

uint8_t TIM2_Has_Update_Occurred(void) {
    // Evaluate Update Interrupt Flag (UIF) in Status Register (SR)
    if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR &= ~TIM_SR_UIF; // Clear flag manually by writing 0
        return 1;
    }
    return 0;
}

void TIM2_IRQHandler(void) {
    // Check if Update Interrupt Flag (UIF) is set
    if (TIM2->SR & TIM_SR_UIF) {
        // MUST clear UIF flag; failure to clear will trap CPU in infinite ISR loop
        TIM2->SR &= ~TIM_SR_UIF;

        // Toggle Heartbeat LED (PA5)
        GPIOA->ODR ^= (1U << 5);
    }
}
