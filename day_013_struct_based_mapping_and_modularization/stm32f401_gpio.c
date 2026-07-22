#include "stm32f401_gpio.h"

static uint16_t gpio_status_cache = 0UL;

void validatePin(uint8_t pin) {
    if (pin > 15) return;
}

void GPIO_Set2BitField(volatile uint32_t *reg, uint8_t pin, uint32_t value) {
    uint32_t mask = (3UL << (pin * 2UL));
    *reg &= ~mask;
    *reg |= (value << (pin * 2UL));
}

void RCC_EnableGPIOClock(void) {
    RCC_AHB1ENR |= RCC_GPIOA_EN | RCC_GPIOC_EN;
}

void GPIO_SetMode(volatile uint32_t *MODER_reg, uint8_t pin, uint32_t mode) {
    GPIO_Set2BitField(MODER_reg, pin, mode);
}

void GPIOA_SetOutputOtype(uint8_t pin, uint32_t type) {
    validatePin(pin);
    GPIOA->OTYPER &= ~(1UL << pin);
    GPIOA->OTYPER |= ((type & 0x01UL) << pin);
}

void GPIOA_SetPullUpDown(uint8_t pin, uint32_t state) {
    GPIO_Set2BitField(&GPIOA->PUPDR, pin, state);
}

void GPIOA_WritePin(uint8_t pin, uint8_t state) {
    validatePin(pin);
    if (state) {
        GPIOA->ODR |= (1UL << pin);
    } else {
        GPIOA->ODR &= ~(1UL << pin);
    }
}

void GPIO_TogglePin(uint8_t pin) {
    validatePin(pin);
    if (gpio_status_cache & (1UL << pin)) {
        GPIOA->BSRR = (1UL << (pin + 16UL)); // Atomic Reset (Upper 16 bits) using struct pointer
        gpio_status_cache &= ~(1UL << pin);
    } else {
        GPIOA->BSRR = (1UL << pin);          // Atomic Set (Lower 16 bits) using struct pointer
        gpio_status_cache |= (1UL << pin);
    }
}

void Delay_Simple(uint32_t count) {
    for(volatile uint32_t i = 0; i < count; i++);
}

uint8_t GPIOC_ReadPin(uint8_t pin) {
    validatePin(pin);
    if ((GPIOC->IDR & (1UL << pin)) != 0) {
        return 1;
    } else {
        return 0;
    }
}

ButtonState_t get_button_state(uint8_t pin) {
    validatePin(pin);
    if (GPIOC_ReadPin(pin) == 0) {
        return BUTTON_PRESSED;
    }
    return BUTTON_RELEASED;
}

bool Button_Update_FSM(uint8_t current_pin_level) {
    static ButtonState_t button_state = BUTTON_RELEASED;
    static uint32_t debounce_counter = 0;

    bool press_event = false;

    switch (button_state) {
        case BUTTON_RELEASED:
            if (current_pin_level == 0) {
                debounce_counter = 0;
                button_state = BUTTON_DEBOUNCING_DOWN;
            }
            break;

        case BUTTON_DEBOUNCING_DOWN:
            if (current_pin_level == 0) {
                debounce_counter++;
                if (debounce_counter >= DEBOUNCE_THRESHOLD_TICKS) {
                    button_state = BUTTON_PRESSED;
                    press_event = true;
                }
            } else {
                button_state = BUTTON_RELEASED;
            }
            break;

        case BUTTON_PRESSED:
            if (current_pin_level == 1) {
                debounce_counter = 0;
                button_state = BUTTON_DEBOUNCING_UP;
            }
            break;

        case BUTTON_DEBOUNCING_UP:
            if (current_pin_level == 1) {
                debounce_counter++;
                if (debounce_counter >= DEBOUNCE_THRESHOLD_TICKS) {
                    button_state = BUTTON_RELEASED;
                }
            } else {
                button_state = BUTTON_PRESSED;
            }
            break;
    }

    return press_event;
}

void GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_Init_t *init) {
    validatePin(init->Pin);

    // 1. Set Mode
    GPIO_Set2BitField(&GPIOx->MODER, init->Pin, init->Mode);

    // 2. Set Output Type
    if (init->Mode == GPIO_MODE_OUTPUT) {
        GPIOx->OTYPER &= ~(1UL << init->Pin);
        GPIOx->OTYPER |= ((init->OType & 0x01UL) << init->Pin);
    }

    // 3. Set Pull-Up / Pull-Down
    GPIO_Set2BitField(&GPIOx->PUPDR, init->Pin, init->Pull);
}
