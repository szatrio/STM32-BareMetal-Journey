#include "stm32f401_gpio.h"

/**
 * @brief Validates if the given GPIO pin number is within the valid range (0-15).
 */
bool validatePin(uint8_t pin) {
    return (pin <= 15);
}

/**
 * @brief Helper function to set a 2-bit field in a GPIO register safely.
 */
void GPIO_Set2BitField(volatile uint32_t *reg, uint8_t pin, uint32_t value) {
    if (!validatePin(pin)) return;

    uint32_t mask = (3UL << (pin * 2UL));
    *reg &= ~mask;
    *reg |= ((value & 0x03UL) << (pin * 2UL));
}

/**
 * @brief Enables AHB1 clock for GPIOA and GPIOC peripherals.
 */
void RCC_EnableGPIOClock(void) {
    RCC_AHB1ENR |= RCC_GPIOA_EN | RCC_GPIOC_EN;
}

/**
 * @brief Initializes a GPIO pin according to the specified configuration structure.
 */
void GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_Init_t *init) {
    if (!validatePin(init->Pin)) return;

    // 1. Configure Pin Mode (Input, Output, Alternate Function, Analog)
    GPIO_Set2BitField(&GPIOx->MODER, init->Pin, init->Mode);

    // 2. Configure Output Type (Push-Pull / Open-Drain)
    if (init->Mode == GPIO_MODE_OUTPUT) {
        GPIOx->OTYPER &= ~(1UL << init->Pin);
        GPIOx->OTYPER |= ((init->OType & 0x01UL) << init->Pin);
    }

    // 3. Configure Pull-Up / Pull-Down Resistors
    GPIO_Set2BitField(&GPIOx->PUPDR, init->Pin, init->Pull);
}

/**
 * @brief Writes a high (1) or low (0) state to a specific GPIO pin using BSRR.
 */
void GPIO_WritePin(GPIO_TypeDef *GPIOx, uint8_t pin, uint8_t state) {
    if (!validatePin(pin)) return;

    if (state) {
        GPIOx->BSRR = (1UL << pin);
    } else {
        GPIOx->BSRR = (1UL << (pin + 16UL));
    }
}

/**
 * @brief Toggles the current output state of a specific GPIO pin.
 */
void GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint8_t pin) {
    if (!validatePin(pin)) return;

    if (GPIOx->ODR & (1UL << pin)) {
        GPIOx->BSRR = (1UL << (pin + 16UL));
    } else {
        GPIOx->BSRR = (1UL << pin);
    }
}

/**
 * @brief Reads the logical level of a specific GPIO input pin.
 * @note  Generic implementation compatible with any GPIO port (GPIOA, GPIOB, GPIOC, etc.)
 */
uint8_t GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint8_t pin) {
    if (!validatePin(pin)) return 0;

    return ((GPIOx->IDR & (1UL << pin)) != 0) ? 1 : 0;
}

/**
 * @brief Configures the Alternate Function (AF) selection for a specific GPIO pin.
 */
void GPIO_SetAltFunction(GPIO_TypeDef *GPIOx, uint8_t pin, uint8_t af_value) {
    if (!validatePin(pin)) return;

    uint8_t afr_index = pin / 8;
    uint8_t bit_shift = (pin % 8) * 4;

    GPIOx->AFR[afr_index] &= ~(0x0FUL << bit_shift);
    GPIOx->AFR[afr_index] |= ((uint32_t)(af_value & 0x0FUL) << bit_shift);
}

/**
 * @brief  Updates the button debounce FSM using a 8-bit Shift Register history filter.
 * @param  current_pin_level: Raw logical level read from the pin (0 = Active LOW pressed).
 * @retval true if a clean, debounced press event (falling edge) is detected, false otherwise.
 */
bool Button_Update_FSM(uint8_t current_pin_level) {
    static uint8_t history = 0xFF;

    // Shift in the new sample bit
    history = (history << 1) | (current_pin_level & 0x01);

    // 0xF0 = 4 High samples followed by 4 Low samples (Debounced falling edge)
    if (history == 0xF0) {
        history = 0x00; // Reset history to prevent false multi-triggers
        return true;
    }

    return false;
}

/**
 * @brief Simple software blocking delay loop.
 */
void Delay_Simple(uint32_t count) {
    for (volatile uint32_t i = 0; i < count; i++);
}
