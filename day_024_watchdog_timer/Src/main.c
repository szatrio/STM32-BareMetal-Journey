#include "stm32f401_gpio.h"
#include "stm32f401_usart.h"
#include "stm32f401_ring_buffer.h"

#define LED_PIN       5   // PA5
#define BUTTON_PIN    13  // PC13

RingBuffer_t tx_buffer;

static void Delay_Ms(uint32_t ms) {
    for (uint32_t i = 0; i < ms; i++) {
        for (volatile uint32_t j = 0; j < 1600; j++);
    }
}

// Watchdog Init - Prescaler /128 & RLR 1250 (~5 Detik Timeout)
static void IWDG_Init(void) {
    IWDG->KR = IWDG_KEY_ACCESS;
    IWDG->PR = IWDG_PR_DIV_128;
    IWDG->RLR = 1250;
    IWDG->KR = IWDG_KEY_RELOAD;
    IWDG->KR = IWDG_KEY_ENABLE;
}

static inline void IWDG_Feed(void) {
    IWDG->KR = IWDG_KEY_RELOAD;
}

int main(void)
{
    // 1. Enable Clock
    RCC_EnableGPIOClock();

    // 2. Configure GPIO Pins using Abstraction Layer
    GPIO_Init_t led_cfg = {
        .Pin   = LED_PIN,
        .Mode  = GPIO_MODE_OUTPUT,
        .OType = GPIO_OTYPE_PUSHPULL,
        .Pull  = GPIO_PUPDR_NOPULLUPDOWN
    };
    GPIO_Init(GPIOA, &led_cfg);

    GPIO_Init_t btn_cfg = {
        .Pin   = BUTTON_PIN,
        .Mode  = GPIO_MODE_INPUT,
        .OType = GPIO_OTYPE_PUSHPULL,
        .Pull  = GPIO_PUPDR_PULLUP
    };
    GPIO_Init(GPIOC, &btn_cfg);

    // 3. Init UART & Buffer
    USART2_Init(115200);
    RingBuffer_Init(&tx_buffer);

    USART2_SendString_Buffered(&tx_buffer, "\r\n=== SYSTEM BOOT / RESTART ===\r\n");
    USART2_Flush_TX(&tx_buffer);

    // 4. Start Hardware Watchdog
    IWDG_Init();

    uint32_t heartbeat_counter = 0;

    while (1)
    {
        // Periodic Watchdog Refresh (Normal State)
        IWDG_Feed();

        // Visual Heartbeat LED (~500ms toggle)
        heartbeat_counter++;
        if (heartbeat_counter >= 50) {
            GPIO_TogglePin(GPIOA, LED_PIN);
            heartbeat_counter = 0;
        }

        // Read Button State using Generic Function
        uint8_t raw_btn = GPIO_ReadPin(GPIOC, BUTTON_PIN);

        // Process Debounce Filter
        if (Button_Update_FSM(raw_btn)) {
            USART2_SendString_Buffered(&tx_buffer, "\r\n[FAULT INJECTION] Button pressed! Entering infinite lockup loop...\r\n");
            USART2_Flush_TX(&tx_buffer);

            // SIMULASI FAULT: Freeze execution without feeding watchdog
            while (1) {
                // Program stuck here! Hardware IWDG will reset MCU in ~5 seconds.
            }
        }

        USART2_Flush_TX(&tx_buffer);
        Delay_Ms(10);
    }
}
