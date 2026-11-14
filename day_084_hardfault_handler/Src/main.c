/**
 * @file main.c
 * @brief Day 84 Hard Fault Handler Test
 */

#include <stdint.h>
#include "fault_handler.h"

/* Function to intentionally trigger a Hard Fault for testing */
static void trigger_fault(void)
{
    /* Attempt to write to an invalid/reserved memory address */
    volatile uint32_t *invalid_addr = (volatile uint32_t *)0x00000000;
    *invalid_addr = 0xDEADBEEF;
}

int main(void)
{
    volatile uint32_t counter = 0;

    /* Uncomment the line below to test the Hard Fault Handler */
    // trigger_fault();

    while (1)
    {
        counter++;
    }
}
