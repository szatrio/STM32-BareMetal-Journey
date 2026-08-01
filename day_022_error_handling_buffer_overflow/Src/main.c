#include "stm32f401_gpio.h"
#include "stm32f401_usart.h"
#include "stm32f401_ring_buffer.h"

#define UART2_TX_PIN 2  // PA2
#define UART2_RX_PIN 3  // PA3

// Global instance of the ring buffer
RingBuffer_t rx_buffer;

static void System_Init(void)
{
    // 1. Enable GPIOA Peripheral Clock
    RCC_EnableGPIOClock();

    // 2. Configure Alternate Function (AF7) for UART2 Pins (PA2 & PA3)
    GPIO_SetMode(&GPIOA->MODER, UART2_TX_PIN, GPIO_MODE_ALT);
    GPIO_SetMode(&GPIOA->MODER, UART2_RX_PIN, GPIO_MODE_ALT);

    GPIO_SetAltFunction(GPIOA, UART2_TX_PIN, 7); // PA2 -> AF7 (USART2_TX)
    GPIO_SetAltFunction(GPIOA, UART2_RX_PIN, 7); // PA3 -> AF7 (USART2_RX)

    // 3. Initialize USART2 with 115200 Baud Rate
    USART2_Init(115200);

    // 4. Initialize Ring Buffer pointers (head = 0, tail = 0)
    RingBuffer_Init(&rx_buffer);
}

int main(void)
{
    System_Init();

    UART_Println("=== Buffer Overflow Error Handling Test ===");

    UART_Println("Pushing 63 bytes to fill buffer exactly...");
	for (int i = 0; i < 63; i++) {
		RingBuffer_Push(&rx_buffer, 'A' + (i % 26)); // Push A-Z chars repeatedly
	}

	if (!RingBuffer_HasOverflowed(&rx_buffer)) {
		UART_Println("[SUCCESS] Buffer full (63 bytes) without overflow error.");
	}

	UART_Println("\nIntentionally push additional 5 bytes");
	for (int i = 0; i < 5; i++) {
		RingBuffer_Push(&rx_buffer, 'X');
	}

	if (RingBuffer_HasOverflowed(&rx_buffer)) {
		UART_Println("[ALERT] Buffer Overflow Detected!");
	} else {
		UART_Println("[ERROR] Overflow Detection Failed!");
	}

	if (RingBuffer_GetOverflowCount(&rx_buffer) == 5) {
		UART_Println("[SUCCESS] Overflow Count accurate (5 bytes dropped).");
	} else {
		UART_Println("[ERROR] Overflow Count inaccurate!");
	}

	RingBuffer_ClearOverflow(&rx_buffer);
	if (!RingBuffer_HasOverflowed(&rx_buffer) && RingBuffer_GetOverflowCount(&rx_buffer) == 0) {
		UART_Println("[SUCCESS] Overflow Flags Cleared Successfully.");
	}

    while (1) {
        // Main loop idle
    }
}
