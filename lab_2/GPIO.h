#ifndef GPIO_H
#define GPIO_H

#include "stm32f4xx.h"

#define GPIO_PIN_SET   1
#define GPIO_PIN_RESET 0

void GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t pin, uint8_t state);

void traffic_init(void);
#endif