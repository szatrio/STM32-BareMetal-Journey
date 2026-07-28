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

    UART_Println("=== Ring Buffer Structure Design ===");

    // Verify initial state of Head and Tail
    if (rx_buffer.head == 0 && rx_buffer.tail == 0) {
        UART_Println("[SUCCESS] Ring Buffer Initialized (head=0, tail=0)");
    } else {
        UART_Println("[ERROR] Ring Buffer Initialization Failed!");
    }

    UART_Println("\r\n--- Testing RingBuffer_Push ---");

	// Testing push data 'A' char
	if (RingBuffer_Push(&rx_buffer, 'A')) {
		UART_Println("[SUCCESS] Pushed 'A' to Ring Buffer");
	} else {
		UART_Println("[ERROR] Buffer Full, Push Failed!");
	}

	// Trying push data 'B' char
	if (RingBuffer_Push(&rx_buffer, 'B')) {
		UART_Println("[SUCCESS] Pushed 'B' to Ring Buffer");
	} else {
		UART_Println("[ERROR] Buffer Full, Push Failed!");
	}

	// Reset buffer to 0
	RingBuffer_Init(&rx_buffer);

    // Circular Indexing Simulation (Modulo Test)
    UART_Println("\r\n--- Simulating Head Increment & Wrap-Around ---");

    for (int i = 0; i < RING_BUFFER_SIZE + 3; i++) {
        // Modulo logic for circular indexing: (index + 1) % SIZE
        rx_buffer.head = (rx_buffer.head + 1) % RING_BUFFER_SIZE;

        // Print log when wrap-around threshold is reached
        if (i >= RING_BUFFER_SIZE - 2) {
            UART_Println("Head incremented -> Wrap around active!");
        }
    }

    // Verify if head correctly wrapped around to index 3
    if (rx_buffer.head == 3) {
        UART_Println("[SUCCESS] Circular Modulo Verified! Head wrapped to index 3.");
    }

    while (1) {
        // Main loop idle
    }
}
