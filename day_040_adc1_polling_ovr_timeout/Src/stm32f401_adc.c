#include "stm32f401_adc.h"

void ADC1_Init(uint8_t channel) {
    // 1. Enable Clock ADC1 in APB2 bus
	RCC_APB2ENR |= RCC_APB2ENR_ADC1EN;

    // 2. Set first squence convertion (SQ1) in register SQR3
    // Reset bit 0-4 first, then channel number
    ADC1->SQR3 &= ~(0x1F << ADC_SQR3_SQ1_POS);
    ADC1->SQR3 |= ((channel & 0x1F) << ADC_SQR3_SQ1_POS);

    // 3. CR2 Configuration: Enable Continuous Mode & Power ON ADC
    ADC1->CR2 |= ADC_CR2_CONT;

    ADC1->CR2 |= ADC_CR2_EOCS;

    ADC1->CR2 |= ADC_CR2_ADON;
}

void ADC1_StartConversion(void) {
    ADC1->CR2 |= ADC_CR2_SWSTART;
}

uint16_t ADC1_Read(void) {
    // 1. Polling flag EOC (End of Conversion) in SR until value is 1
    while (!(ADC1->SR & ADC_SR_EOC));

    // 2. Return data convertion result 12-bit from DR
    // (Reading DR is automacally clean EOC flag in hardware)
    return (uint16_t)(ADC1->DR & 0xFFF);
}

ADC_Status_t ADC1_ReadPollForConversion(uint16_t *data_out, uint32_t timeout_loops) {
    uint32_t counter = 0;

    // 1. checking overrun flag from previous conversion
    if (ADC1->SR & ADC_SR_OVR) {
        ADC1_ClearOVR();
        return ADC_ERROR_OVERRUN;
    }

    // 2. Polling EOC with Timeout Mechanism (Preventing CPU Lockup)
    while (!(ADC1->SR & ADC_SR_EOC)) {
        counter++;
        if (counter >= timeout_loops) {
            return ADC_ERROR_TIMEOUT;
        }
    }

    // 3. getting 12-bit data (reading DR is automatically reset EOC flag in hardware)
    if (data_out != 0) {
        *data_out = (uint16_t)(ADC1->DR & 0xFFF);
    }

    return ADC_OK;
}

void ADC1_ClearOVR(void) {
    // Write 0 to OVR bit in Status Register (SR) to clear the overrun condition
    ADC1->SR &= ~ADC_SR_OVR;
}
