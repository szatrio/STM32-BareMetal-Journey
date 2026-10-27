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

void TIM2_PWM_Init(uint8_t channel, uint16_t prescaler, uint32_t auto_reload) {
    // 1. Enable TIM2 peripheral clock
    RCC_APB1ENR |= RCC_APB1ENR_TIM2EN;

    // 2. Configure timer timebase parameters (Shared across channels)
    TIM2->PSC = prescaler - 1;
    TIM2->ARR = auto_reload - 1;

    // 3. Configure Output Compare Mode, Preload, and Enable based on target channel
    switch (channel) {
        case 1:
            // CCMR1: CH1 bits (OC1M at bit 6:4, OC1PE at bit 3)
            TIM2->CCMR1 &= ~(7UL << 4);
            TIM2->CCMR1 |= TIM_CCMR1_OC1M_PWM1; // PWM Mode 1 (110)
            TIM2->CCMR1 |= TIM_CCMR1_OC1PE;     // Enable Preload
            TIM2->CCER  |= TIM_CCER_CC1E;       // Enable CC1 output
            break;

        case 2:
            // CCMR1: CH2 bits (OC2M at bit 14:12, OC2PE at bit 11)
            TIM2->CCMR1 &= ~(7UL << 12);
            TIM2->CCMR1 |=  (6UL << 12);        // PWM Mode 1 for CH2 (110)
            TIM2->CCMR1 |=  (1UL << 11);        // Enable Preload for CH2
            TIM2->CCER  |=  (1UL << 4);         // Enable CC2 output (CC2E)
            break;

        case 3:
            // CCMR2: CH3 bits (OC3M at bit 6:4, OC3PE at bit 3)
            TIM2->CCMR2 &= ~(7UL << 4);
            TIM2->CCMR2 |=  (6UL << 4);         // PWM Mode 1 for CH3 (110)
            TIM2->CCMR2 |=  (1UL << 3);         // Enable Preload for CH3
            TIM2->CCER  |=  (1UL << 8);         // Enable CC3 output (CC3E)
            break;

        case 4:
            // CCMR2: CH4 bits (OC4M at bit 14:12, OC4PE at bit 11)
            TIM2->CCMR2 &= ~(7UL << 12);
            TIM2->CCMR2 |=  (6UL << 12);        // PWM Mode 1 for CH4 (110)
            TIM2->CCMR2 |=  (1UL << 11);        // Enable Preload for CH4
            TIM2->CCER  |=  (1UL << 12);        // Enable CC4 output (CC4E)
            break;

        default:
            return; // Invalid channel
    }

    // 4. Force an update event to reload Prescaler and ARR values immediately
    TIM2->EGR |= TIM_EGR_UG;

    // 5. Start counter
    TIM2->CR1 |= TIM_CR1_CEN;
}

void TIM2_PWM_SetDutyCycle(uint8_t channel, float percent) {
    // 1. Clamp input percentage to safe operating bounds (0.0% to 100.0%)
    if (percent < 0.0f) {
        percent = 0.0f;
    } else if (percent > 100.0f) {
        percent = 100.0f;
    }

    // 2. Retrieve active Auto-Reload Register (ARR) value from hardware
    uint32_t arr_val = TIM2->ARR;

    // 3. Compute target Compare Register value based on percentage formula
    uint32_t ccr_val = (uint32_t)((percent * (float)arr_val) / 100.0f);

    // 4. Update the respective Capture/Compare Register based on the channel
    switch (channel) {
        case 1: TIM2->CCR1 = ccr_val; break;
        case 2: TIM2->CCR2 = ccr_val; break;
        case 3: TIM2->CCR3 = ccr_val; break;
        case 4: TIM2->CCR4 = ccr_val; break;
        default: break;
    }
}
