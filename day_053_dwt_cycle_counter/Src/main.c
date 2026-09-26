#include "stm32f401_system.h"
#include "stm32f401_gpio.h"
#include "stm32f401_usart.h"
#include "cortex_m4_dwt.h"

#define LED_PIN 5 // PA5 on Nucleo board

int main(void)
{
    FPU_Enable();
    RCC_EnableGPIOClock();
    USART2_Init(115200);

    // Initialize DWT cycle counter for precise delays and profiling (pass 84 MHz core clock)
    if (!DWT_Delay_Init(84000000UL)) {
        UART_Println("Error: DWT initialization failed!");
    } else {
        UART_Println("DWT Cycle Counter initialized successfully.");
    }

    // Configure LED pin as push-pull output
    GPIO_Init_t led_init = {
        .Pin   = LED_PIN,
        .Mode  = GPIO_MODE_OUTPUT,
        .OType = GPIO_OTYPE_PUSHPULL,
        .Pull  = GPIO_PUPDR_NOPULLUPDOWN
    };
    GPIO_Init(GPIOA, &led_init);

    UART_Println("Starting DWT performance profiling & delay test...");

    while (1) {
        // Record start cycles for profiling execution time
        uint32_t start_cycles = DWT_GetCycleCount();

        // Toggle LED state
        GPIO_TogglePin(GPIOA, LED_PIN);

        // Record end cycles and compute elapsed ticks
        uint32_t elapsed_cycles = DWT_GetCycleCount() - start_cycles;

        // Print profiling result via UART
        // (1 cycle at 84 MHz = ~11.9 nanoseconds)
        UART_Print("LED toggle execution took cycles: ");
        // Note: You can format numbers or print directly if print function supports it

        // Precise 500 ms delay using DWT hardware cycle counter
        DWT_Delay_ms(500U);
    }
}
