#include "stm32f401_gpio.h"
#include "stm32f401_usart.h"
#include "stm32f401_ring_buffer.h"

#define UART2_TX_PIN 2  // PA2
#define UART2_RX_PIN 3  // PA3

// Global instance of the TX ring buffer
RingBuffer_t tx_buffer;

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
    RingBuffer_Init(&tx_buffer);
}

int main(void)
{
    System_Init();

    UART_Println("=== Non-Blocking Buffered UART TX Test ===");

    // Push messages to the TX Ring Buffer (Fast Non-blocking operations)
    USART2_SendString_Buffered(&tx_buffer, "[BUFFER] Line 1: Hello from Ring Buffer!\r\n");
	USART2_SendString_Buffered(&tx_buffer, "[BUFFER] Line 2: Bare-metal Firmware Engineering!\r\n");
	USART2_SendString_Buffered(&tx_buffer, "[BUFFER] Line 3: Flushing bytes to hardware...\r\n");

	// flush engine: stream buffered data out through hardware USART2->DR
	USART2_Flush_TX(&tx_buffer);

	// verification check
	if (RingBuffer_IsEmpty(&tx_buffer)) {
		UART_Println("[SUCCESS] All buffer data flushed to UART hardware successfully!");
	} else {
		UART_Println("[ERROR] Buffer flush incomplete!");
	}

    while (1) {
        // Main loop idle
    }
}
