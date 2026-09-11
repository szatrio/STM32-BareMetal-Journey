#include "stm32f401_adc.h"

void ADC1_Init(uint8_t channel) {
    // 1. Enable ADC1 peripheral clock on APB2 bus
    RCC_APB2ENR |= RCC_APB2ENR_ADC1EN;

    // 2. Configure sequence 1 (SQ1) channel in SQR3 register
    ADC1->SQR3 &= ~(0x1F << ADC_SQR3_SQ1_POS);
    ADC1->SQR3 |= ((channel & 0x1F) << ADC_SQR3_SQ1_POS);

    // 3. STEP A: Power ON ADC first (Set ADON)
    ADC1->CR2 |= ADC_CR2_ADON;

    // STEP B: Stabilization delay for ADC hardware wakeup time (tSTAB ~3-10us)
    for (volatile int i = 0; i < 1000; i++) {
        __NOP();
    }

    // 4. STEP C: Configure CONT and EOCS bits while ADC is already powered ON
    uint32_t cr2_temp = ADC1->CR2;
    cr2_temp |= (ADC_CR2_CONT | ADC_CR2_EOCS);

    // Write back configuration to CR2
    ADC1->CR2 = cr2_temp;
}

void ADC1_StartConversion(void) {
    // Start conversion of regular channels via software trigger
    ADC1->CR2 |= ADC_CR2_SWSTART;
}

uint16_t ADC1_Read(void) {
    // 1. Poll EOC (End of Conversion) flag in SR register until conversion completes
    while (!(ADC1->SR & ADC_SR_EOC));

    // 2. Return 12-bit conversion result from DR register
    // (Reading DR automatically clears the EOC flag in hardware)
    return (uint16_t)(ADC1->DR & 0xFFF);
}

ADC_Status_t ADC1_ReadPollForConversion(uint16_t *data_out, uint32_t timeout_loops) {
    uint32_t counter = 0;

    // 1. Check for overrun condition from previous conversions
    if (ADC1->SR & ADC_SR_OVR) {
        ADC1_ClearOVR();
        return ADC_ERROR_OVERRUN;
    }

    // 2. Poll EOC flag with timeout mechanism to prevent CPU lockup
    while (!(ADC1->SR & ADC_SR_EOC)) {
        counter++;
        if (counter >= timeout_loops) {
            return ADC_ERROR_TIMEOUT;
        }
    }

    // 3. Retrieve raw data (Reading DR automatically clears EOC flag in hardware)
    if (data_out != 0) {
        *data_out = (uint16_t)ADC1->DR;
    }

    return ADC_OK;
}

void ADC1_ClearOVR(void) {
    // Clear Overrun (OVR) flag in Status Register by writing 0
    ADC1->SR &= ~ADC_SR_OVR;
}
