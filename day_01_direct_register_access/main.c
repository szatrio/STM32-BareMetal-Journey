#include <stdint.h>

// Base Addresses
#define RCC_BASE      0x40023800
#define GPIOA_BASE    0x40020000

// Register Addresses
#define RCC_AHB1ENR   (*(volatile uint32_t *)(RCC_BASE + 0x30))
#define GPIOA_MODER   (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_ODR     (*(volatile uint32_t *)(GPIOA_BASE + 0x14))

int main(void) {
    // 1. Enable Clock GPIOA (Bit 0)
    RCC_AHB1ENR |= (1 << 0);

    // 2. Set PA5 as Output
    // Clear bit 10 & 11 (masking)
    GPIOA_MODER &= ~(3 << 10);
    // Set bit 10 to 1 (01 = Output)
    GPIOA_MODER |= (1 << 10);

    while(1) {
        // 3. Toggle PA5 (Bit 5)
        GPIOA_ODR ^= (1 << 5);

        // Simple delay for blink
        for(int i = 0; i < 500000; i++);
    }
}
