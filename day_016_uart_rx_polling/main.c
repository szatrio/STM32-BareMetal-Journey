#include "stm32f401_gpio.h"
#include "stm32f401_usart.h"

#define LED_PIN    5

// PIN UART2
#define UART2_TX_PIN 2  // PA2
#define UART2_RX_PIN 3  // PA3

static void System_Init(void)
{
    // 1. Enable Clock GPIOA & GPIOC
    RCC_EnableGPIOClock();

    // 2. Setup LED (PA5)
    GPIO_Init_t led_config = {
        .Pin   = LED_PIN,
        .Mode  = GPIO_MODE_OUTPUT,
        .OType = GPIO_OTYPE_PUSHPULL,
        .Pull  = GPIO_PUPDR_NOPULLUPDOWN
    };
    GPIO_Init(GPIOA, &led_config);

	// 3. Setup Pin Alternate Function UART2 (PA2 & PA3)
	GPIO_SetMode(&GPIOA->MODER, UART2_TX_PIN, GPIO_MODE_ALT);
	GPIO_SetMode(&GPIOA->MODER, UART2_RX_PIN, GPIO_MODE_ALT);

	GPIO_SetAltFunction(GPIOA, UART2_TX_PIN, 7); // PA2 -> AF7 (USART2_TX)
	GPIO_SetAltFunction(GPIOA, UART2_RX_PIN, 7); // PA3 -> AF7 (USART2_RX)

	// 4. Set Baud Rate 115200
	USART2_Init(115200);
}

int main(void)
{
	System_Init();

	UART_SendChar('R');
	UART_SendChar('X');
	UART_SendChar(' ');
	UART_SendChar('O');
	UART_SendChar('K');
	UART_SendChar('\r');
	UART_SendChar('\n');

    while(1)
    {
    	// Blocking while waiting input from PC
        char rx_byte = UART_ReceiveChar();

        // Sending char
        UART_SendChar(rx_byte);

        // Controlling actuator, based on PC input
        if (rx_byte == '1') {
			GPIOA_WritePin(LED_PIN, 1); // LED ON
		} else if (rx_byte == '0') {
			GPIOA_WritePin(LED_PIN, 0); // LED OFF
		}

    }
}
