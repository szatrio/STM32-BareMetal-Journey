#include "stm32f401_adc.h"

void ADC1_Init(uint8_t channel) {
    // 1. Enable ADC1 peripheral clock on APB2 bus
    RCC_APB2ENR |= RCC_APB2ENR_ADC1EN;

    // 2. Configure sequence 1 (SQ1) channel in SQR3 register
    ADC1->SQR3 &= ~(0x1F << ADC_SQR3_SQ1_POS);
    ADC1->SQR3 |= ((channel & 0x1F) << ADC_SQR3_SQ1_POS);

    // 3. Prepare atomic write for CR2 register
    uint32_t cr2_temp = ADC1->CR2;

    // Enable Continuous Mode (CONT), End of Conversion Selection (EOCS), and Power ON (ADON)
    cr2_temp |= (ADC_CR2_CONT | ADC_CR2_EOCS | ADC_CR2_ADON);

    // Write back combined configuration to CR2
    ADC1->CR2 = cr2_temp;

    // 4. Short stabilization delay for ADC hardware wakeup time (tSTAB)
    for (volatile int i = 0; i < 500; i++) {
        __NOP();
    }
}

void ADC1_StartConversion(void) {
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

    // 3. Retrieve raw data (Reading DR automatically clears EOC flag)
    if (data_out != 0) {
        *data_out = (uint16_t)ADC1->DR;
    }

    return ADC_OK;
}

void ADC1_ClearOVR(void) {
    // Clear Overrun (OVR) flag in Status Register by writing 0
    ADC1->SR &= ~ADC_SR_OVR;
}

void ADC1_InitWithAlignment(uint8_t channel, ADC_Align_t alignment) {
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

    // 4. STEP C: Configure CONT, EOCS, and ALIGN while ADC is powered ON
    uint32_t cr2_temp = ADC1->CR2;

    // Set or clear ALIGN bit (Bit 11) based on parameter
    if (alignment == ADC_ALIGN_LEFT) {
        cr2_temp |= ADC_CR2_ALIGN;
    } else {
        cr2_temp &= ~ADC_CR2_ALIGN;
    }

    // Enable Continuous Mode (CONT) and End of Conversion Selection (EOCS)
    cr2_temp |= (ADC_CR2_CONT | ADC_CR2_EOCS);

    // Write back combined configuration to CR2
    ADC1->CR2 = cr2_temp;
}

uint8_t ADC1_Read8BitFast(void) {
    // Extract High Byte [15:8] for instant 8-bit resolution (Left Alignment advantage)
    return (uint8_t)(ADC1->DR >> 8);
}
