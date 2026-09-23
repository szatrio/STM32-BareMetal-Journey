#ifndef STM32F401_GPIO_H
#define STM32F401_GPIO_H

#include "stm32f401_registers.h"
#include <stdbool.h>

// --- Function Prototypes ---
bool validatePin(uint8_t pin);
void GPIO_Set2BitField(volatile uint32_t *reg, uint8_t pin, uint32_t value);
void RCC_EnableGPIOClock(void);
void GPIO_SetMode(volatile uint32_t *MODER_reg, uint8_t pin, uint32_t mode);
void GPIOA_SetOutputOtype(uint8_t pin, uint32_t type);
void GPIOA_SetPullUpDown(uint8_t pin, uint32_t state);
void GPIO_WritePin(GPIO_TypeDef *GPIOx, uint8_t pin, uint8_t state);
void GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint8_t pin);
void Delay_Simple(uint32_t count);
uint8_t GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint8_t pin);
ButtonState_t get_button_state(uint8_t pin);
bool Button_Update_FSM(uint8_t current_pin_level);
void GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_Init_t *init);
void GPIO_SetAltFunction(GPIO_TypeDef *GPIOx, uint8_t pin, uint8_t af_value);

#endif
