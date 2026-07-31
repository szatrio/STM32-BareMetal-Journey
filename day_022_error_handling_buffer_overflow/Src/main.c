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

//    // Verify initial state of Head and Tail
//    if (rx_buffer.head == 0 && rx_buffer.tail == 0) {
//        UART_Println("[SUCCESS] Ring Buffer Initialized (head=0, tail=0)");
//    } else {
//        UART_Println("[ERROR] Ring Buffer Initialization Failed!");
//    }
//
//    UART_Println("\r\n--- Testing RingBuffer Push and Pop ---");
//
//	// Testing push chars
//	RingBuffer_Push(&rx_buffer, 'A');
//	RingBuffer_Push(&rx_buffer, 'B');
//	RingBuffer_Push(&rx_buffer, 'C');
//	RingBuffer_Push(&rx_buffer, 'D');
//	RingBuffer_Push(&rx_buffer, 'E');
//	UART_Println("[SUCCESS] Pushed ABCDE to Ring Buffer");
//
//	uint8_t popped_char = 0;
//
//	if (RingBuffer_Pop(&rx_buffer, &popped_char) && popped_char == 'A') {
//		UART_Println("[SUCCESS] 1st Pop returned 'A' (First-In, First-Out Passed!)");
//	} else {
//		UART_Println("[ERROR] 1st Pop Failed!");
//	}
//
//	if (RingBuffer_Pop(&rx_buffer, &popped_char) && popped_char == 'B') {
//		UART_Println("[SUCCESS] 2nd Pop returned 'B'");
//	}
//
//	if (RingBuffer_Pop(&rx_buffer, &popped_char) && popped_char == 'C') {
//		UART_Println("[SUCCESS] 3rd Pop returned 'C'");
//	}
//
//	if (RingBuffer_Pop(&rx_buffer, &popped_char) && popped_char == 'D') {
//			UART_Println("[SUCCESS] 4th Pop returned 'D'");
//		}
//
//	if (RingBuffer_Pop(&rx_buffer, &popped_char) && popped_char == 'E') {
//		UART_Println("[SUCCESS] 5th Pop returned 'E'");
//	}
//
//	UART_Println("\r\n--- Test Underflow Protection ---");
//
//	// Trying to pop on empty buffer (head == tail)
//	if (!RingBuffer_Pop(&rx_buffer, &popped_char)) {
//		UART_Println("[SUCCESS] Pop rejected! Buffer is empty (Underflow Protected).");
//	} else {
//		UART_Println("[ERROR] Pop succeeded on empty buffer!");
//	}

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
