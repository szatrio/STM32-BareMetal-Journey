#include "stm32f401_gpio.h"

static uint16_t gpio_status_cache = 0UL;

void GPIO_TogglePin(uint8_t pin) {
    validatePin(pin);
    if (gpio_status_cache & (1UL << pin)) {
        GPIOA_BSRR = (1UL << (pin + 16UL)); // using '=' because it's atomic
        gpio_status_cache &= ~(1UL << pin);
    } else {
        GPIOA_BSRR = (1UL << pin);
        gpio_status_cache |= (1UL << pin);
    }
}
