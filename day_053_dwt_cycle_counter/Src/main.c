#include "stm32f401_system.h"
#include "stm32f401_gpio.h"
#include "stm32f401_usart.h"
#include "cortex_m4_dwt.h"

#define LED_PIN 5 // PA5 on Nucleo board

// Helper function to print an unsigned 32-bit number via UART
static void UART_PrintNumber(uint32_t num) {
    char buf[16];
    int i = 0;

    if (num == 0) {
        UART_SendChar('0');
        return;
    }

    while (num > 0) {
        buf[i++] = (char)('0' + (num % 10));
        num /= 10;
    }

    while (i > 0) {
        UART_SendChar(buf[--i]);
    }
}

int main(void)
{
    // Enable FPU and initialize system clock (84 MHz)
    FPU_Enable();
    RCC_EnableGPIOClock();

    // Initialize USART2 for debugging logs (115200 baud)
    USART2_Init(115200);

    // Initialize DWT cycle counter
    if (!DWT_Delay_Init(84000000UL)) {
        UART_Println("Error: DWT initialization failed!");
    } else {
        UART_Println("========================================");
        UART_Println("  STM32F401 DWT PERFORMANCE BENCHMARK   ");
        UART_Println("========================================");
    }

    // Configure LED pin as push-pull output
    GPIO_Init_t led_init = {
        .Pin   = LED_PIN,
        .Mode  = GPIO_MODE_OUTPUT,
        .OType = GPIO_OTYPE_PUSHPULL,
        .Pull  = GPIO_PUPDR_NOPULLUPDOWN
    };
    GPIO_Init(GPIOA, &led_init);

    // --- RUN BENCHMARK ONCE UPON BOOT ---
    uint32_t start_cycles;
    uint32_t cycles_wrapper, cycles_direct, cycles_int, cycles_float;

    // 1. Benchmark Function Wrapper (GPIO_TogglePin)
    start_cycles = DWT_GetCycleCount();
    GPIO_TogglePin(GPIOA, LED_PIN);
    cycles_wrapper = DWT_GetCycleCount() - start_cycles;

    // 2. Benchmark Direct Register Access (BSRR)
    start_cycles = DWT_GetCycleCount();
    GPIOA->BSRR = (1UL << LED_PIN);         // Set pin HIGH
    GPIOA->BSRR = (1UL << (LED_PIN + 16));  // Set pin LOW
    cycles_direct = DWT_GetCycleCount() - start_cycles;

    // 3. Benchmark Integer Multiplication (100 iterations)
    start_cycles = DWT_GetCycleCount();
    volatile int32_t a = 123, b = 456, res = 0;
    for (int i = 0; i < 100; i++) {
        res += a * b;
    }
    (void)res;
    cycles_int = DWT_GetCycleCount() - start_cycles;

    // 4. Benchmark Float Multiplication (100 iterations with FPU)
    start_cycles = DWT_GetCycleCount();
    volatile float fa = 123.45f, fb = 678.90f, fres = 0.0f;
    for (int i = 0; i < 100; i++) {
        fres += fa * fb;
    }
    (void)fres;
    cycles_float = DWT_GetCycleCount() - start_cycles;

    // Print all benchmark metrics to PuTTY once
    UART_Print("1. GPIO Wrapper Toggle : "); UART_PrintNumber(cycles_wrapper); UART_Println(" cycles");
    UART_Print("2. Direct Register BSRR: "); UART_PrintNumber(cycles_direct); UART_Println(" cycles");
    UART_Print("3. Integer Math (100x) : "); UART_PrintNumber(cycles_int); UART_Println(" cycles");
    UART_Print("4. Float Math (100x)   : "); UART_PrintNumber(cycles_float); UART_Println(" cycles");
    UART_Println("========================================");
    UART_Println("System entering normal operation loop...");

    while (1) {
        // Toggle LED and delay safely without spamming serial terminal
        GPIO_TogglePin(GPIOA, LED_PIN);
        DWT_Delay_ms(500U);
    }
}
