#include "stm32f401_adc.h"

void ADC1_Init(uint8_t channel) {
    // 1. Enable Clock ADC1 in APB2 bus
	RCC_APB2ENR |= RCC_APB2ENR_ADC1EN;

    // 2. Set first squence convertion (SQ1) in register SQR3
    // Reset bit 0-4 first, then channel number
    ADC1->SQR3 &= ~(0x1F << ADC_SQR3_SQ1_POS);
    ADC1->SQR3 |= ((channel & 0x1F) << ADC_SQR3_SQ1_POS);

    // 3. ADC1 (Power ON) via bit ADON at CR2
    ADC1->CR2 |= ADC_CR2_ADON;
}

uint16_t ADC1_Read(void) {
    // 1. Trigger convertion via software (SWSTART)
    ADC1->CR2 |= ADC_CR2_SWSTART;

    // 2. Polling flag EOC (End of Conversion) in SR until value is 1
    while (!(ADC1->SR & ADC_SR_EOC));

    // 3. Return data convertion result 12-bit from DR
    // (Reading DR is automacally clean EOC flag in hardware)
    return (uint16_t)(ADC1->DR & 0xFFF);
}
