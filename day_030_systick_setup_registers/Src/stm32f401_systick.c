#include "stm32f401_systick.h"

void SysTick_Init(uint32_t ticks) {
    // 1. Disable SysTick counter for safety initialization
    SysTick->CTRL = 0;

    // 2. Set the Reload Value (LOAD register)
	//    subtract 1 because the counter counts down from LOAD value to 0 (N cycles total)
	SysTick->LOAD = ticks - 1;

	// 3. Clear the Current Value Register (VAL)
	//    writing any value to CVR resets the counter and clears the COUNTFLAG
    SysTick->VAL = 0;

    // 4. Configure Control & Status Register (CTRL):
	//    - CLKSOURCE (Bit 2) = 1 (Use AHB Processor Clock / HSI 16MHz)
	//    - TICKINT   (Bit 1) = 1 (Enable SysTick exception request / IRQ)
	//    - ENABLE    (Bit 0) = 1 (Enable the down-counter)
    SysTick->CTRL = SYSTICK_CTRL_CLKSOURCE | SYSTICK_CTRL_TICKINT | SYSTICK_CTRL_ENABLE;
}
