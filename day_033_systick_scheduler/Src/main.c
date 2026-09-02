#include "stm32f401_gpio.h"
#include "stm32f401_nvic.h"
#include "stm32f401_exti.h"
#include "stm32f401_systick.h"
#include "stm32f401_usart.h"
#include <stdio.h>

#define LED_PIN          5   // PA5 (Onboard LED)
#define BUTTON_PIN       13  // PC13 (User Button)
#define EXTI15_10_IRQn   40

/**
 * @brief Helper function to check if a non-blocking timeout/interval has elapsed.
 *        Uses unsigned integer subtraction to remain safe against 32-bit tick rollover.
 * @param start_tick Timestamp when the event/task started.
 * @param interval_ms Duration to wait in milliseconds.
 * @return 1 if interval has elapsed, 0 otherwise.
 */
static inline uint8_t Has_Timeout_Elapsed(uint32_t start_tick, uint32_t interval_ms) {
    return ((SysTick_GetTick() - start_tick) >= interval_ms);
}

int main(void) {
    // 1. Hardware Initialization
    RCC_EnableGPIOClock();

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
        .Pull  = GPIO_PUPDR_NOPULLUPDOWN
    };
    GPIO_Init(GPIOC, &btn_cfg);

    EXTI_PC13_Init();
    USART2_Init(115200); // Initialize UART for logging/debugging

    // 2. SysTick Initialization (1 ms Tick @ 16 MHz HSI -> 16000 cycles)
    SysTick_Init(16000);
    NVIC_EnableIRQ_Custom(EXTI15_10_IRQn);

    UART_Print("\r\n=== SysTick Scheduler Started ===\r\n");

    // 3. Task Timers (Timestamp Trackers for Co-operative Multitasking)
    uint32_t last_heartbeat_tick = SysTick_GetTick();
    uint32_t last_telemetry_tick = SysTick_GetTick();

    // Non-blocking Timeout Guard state variables
    uint32_t process_start_tick  = SysTick_GetTick();
    uint8_t  is_waiting_response = 1; // Simulates waiting for external peripheral response

    while (1) {
        // -------------------------------------------------------------
        // TASK A: Asynchronous Event (EXTI Button Press)
        // -------------------------------------------------------------
        if (g_btn_event_flag) {
            g_btn_event_flag = 0; // Clear software flag
            GPIOA->ODR ^= (1U << LED_PIN); // Fast toggle action
            UART_Print("[EVENT] Button Pressed! LED Toggled.\r\n");

            // Pressing button resets the non-blocking timeout guard
            process_start_tick = SysTick_GetTick();
            is_waiting_response = 1;
            UART_Print("[GUARD] Timeout Guard Reset! Waiting 2000 ms...\r\n");
        }

        // -------------------------------------------------------------
        // TASK B: Scheduled Task 1 - Heartbeat LED (Every 500 ms)
        // -------------------------------------------------------------
        if (Has_Timeout_Elapsed(last_heartbeat_tick, 500)) {
            last_heartbeat_tick = SysTick_GetTick(); // Reload timestamp
            GPIOA->ODR ^= (1U << LED_PIN);           // Toggle Heartbeat LED
        }

        // -------------------------------------------------------------
        // TASK C: Scheduled Task 2 - Telemetry Logger via UART (Every 1000 ms)
        // -------------------------------------------------------------
        if (Has_Timeout_Elapsed(last_telemetry_tick, 1000)) {
            last_telemetry_tick = SysTick_GetTick(); // Reload timestamp

            char msg[64];
            snprintf(msg, sizeof(msg), "[SCHEDULER] Uptime: %lu ms | Running Tasks...\r\n", SysTick_GetTick());
            UART_Print(msg);
        }

        // -------------------------------------------------------------
        // TASK D: Non-Blocking Timeout Guard Pattern (Safety Mechanism)
        // -------------------------------------------------------------
        if (is_waiting_response) {
            // Evaluates timeout without blocking CPU execution
            if (Has_Timeout_Elapsed(process_start_tick, 2000)) {
                is_waiting_response = 0; // Abort waiting state
                UART_Print("[WARN/ERROR] Timeout Guard Triggered! Peripheral No Response.\r\n");
            }
        }
    }
}
