#ifndef STM32F401_GPIO_H
#define STM32F401_GPIO_H

#include "stm32f401_registers.h"


static inline void GPIO_Set2BitField(volatile uint32_t *reg, uint8_t pin, uint32_t value) {
    uint32_t mask = (3UL << (pin * 2UL));
    *reg &= ~mask;
    *reg |= (value << (pin * 2UL));
}

static inline void validatePin(uint8_t pin){
	if (pin > 15) return;
}

static inline void RCC_EnableGPIOClock(void) {
    RCC_AHB1ENR |= RCC_GPIOA_EN | RCC_GPIOC_EN;
}

static inline void GPIO_SetMode(volatile uint32_t *MODER_reg, uint8_t pin, uint32_t mode) {
    GPIO_Set2BitField(MODER_reg, pin, mode);
}

static inline void GPIOA_SetOutputOtype(uint8_t pin, uint32_t type){
	validatePin(pin);

//	Bits 15:0 OTy: Port x configuration bits (y = 0..15)
//	These bits are written by software to configure the output type of the I/O port.
//	0: Output push-pull (reset state)
//	1: Output open-drain

	GPIOA_OTYPER &= ~(1UL << pin);
	GPIOA_OTYPER |= ((type & 0x01UL) << pin);

}

static inline void GPIOA_SetPullUpDown(uint8_t pin, uint32_t state) {
    GPIO_Set2BitField(&GPIOA_PUPDR, pin, state);
}

static inline void GPIOA_WritePin(uint8_t pin, uint8_t state) {
    validatePin(pin);
    if (state) {
        GPIOA_ODR |= (1UL << pin);
    } else {
        GPIOA_ODR &= ~(1UL << pin);
    }
}

void GPIO_TogglePin(uint8_t pin);

static inline void Delay_Simple(uint32_t count) {
    for(volatile uint32_t i = 0; i < count; i++);
}

static inline uint8_t GPIOC_ReadPin(uint8_t pin){
	validatePin(pin);
	if((GPIOC_IDR & (1UL << pin)) != 0){
		return 1;
	}else{
		return 0;
	}
}

#endif
