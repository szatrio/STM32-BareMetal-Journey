#include "stm32f401_tim.h"

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

uint8_t TIM2_Has_Update_Occurred(void) {
    // Evaluate Update Interrupt Flag (UIF) in Status Register (SR)
    if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR &= ~TIM_SR_UIF; // Clear flag manually by writing 0
        return 1;
    }
    return 0;
}
