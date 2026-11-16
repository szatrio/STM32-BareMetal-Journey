#include "stm32f401_pwr.h"

void PWR_EnterSleepMode(void)
{
    // 1. Enable clock
    RCC_APB1ENR |= RCC_APB1ENR_PWREN;

    // 2. Clear the SLEEPDEEP bit in the SCB system control register
	//    to ensure entry into standard Sleep Mode (rather than Stop or Standby)
	SCB->SCR &= ~SCB_SCR_SLEEPDEEP;

	// 3. Execute the Wait For Interrupt (WFI) instruction
	//    The CPU will halt execution and enter sleep until an external interrupt occurs
	__asm__ volatile ("wfi");
}
