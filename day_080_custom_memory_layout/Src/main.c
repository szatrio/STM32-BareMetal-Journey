/**
 * =================================================================
 * STM32F401RE Custom Linker Script Test - Day 80
 * =================================================================
 */

#include <stdint.h>

int main(void) {
    /* Simple infinite loop running on custom memory boundaries */
    volatile uint32_t counter = 0;

    while (1) {
        counter++;
    }
}
