#include "stm32f4xx.h"
#include "GPIO.h"



void GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t pin, uint8_t state) {
    if (state)
        GPIOx->BSRR = pin;          // Set pin
    else
        GPIOx->BSRR = (pin << 16);  // Reset pin
}

void traffic_init(void) {

	
	  RCC->AHB1ENR |= (1 << 0); // GPIOA
    RCC->AHB1ENR |= (1 << 1); // GPIOB

    /******** PB0?PB5 as Output ********/
    // Set mode to output (01)
    GPIOB->MODER &= ~(0b11 << 0);  // Clear PB0
    GPIOB->MODER |=  (0b01 << 0);  // Set PB0 as output
    GPIOB->MODER &= ~(0b11 << 2);  // PB1
    GPIOB->MODER |=  (0b01 << 2);
    GPIOB->MODER &= ~(0b11 << 4);  // PB2
    GPIOB->MODER |=  (0b01 << 4);
    GPIOB->MODER &= ~(0b11 << 6);  // PB3
    GPIOB->MODER |=  (0b01 << 6);
    GPIOB->MODER &= ~(0b11 << 8);  // PB4
    GPIOB->MODER |=  (0b01 << 8);
    GPIOB->MODER &= ~(0b11 << 10); // PB5
    GPIOB->MODER |=  (0b01 << 10);

   // GPIOB->OSPEEDR |=  (0x3FFF);   // High speed
	 
	 // PB0
   GPIOB->OSPEEDR &= ~(0b11 << 0);   // Clear bits
   GPIOB->OSPEEDR |=  (0b11 << 0);   // Set high speed

// PB1
  GPIOB->OSPEEDR &= ~(0b11 << 2);
  GPIOB->OSPEEDR |=  (0b11 << 2);

// PB2
  GPIOB->OSPEEDR &= ~(0b11 << 4);
  GPIOB->OSPEEDR |=  (0b11 << 4);

// PB3
  GPIOB->OSPEEDR &= ~(0b11 << 6);
  GPIOB->OSPEEDR |=  (0b11 << 6);

// PB4
  GPIOB->OSPEEDR &= ~(0b11 << 8);
  GPIOB->OSPEEDR |=  (0b11 << 8);

// PB5
  GPIOB->OSPEEDR &= ~(0b11 << 10);
  GPIOB->OSPEEDR |=  (0b11 << 10);
//PA6
    GPIOA->MODER &= ~(0b11 << 12); // Clear PA6
    GPIOA->MODER |=  (0b01 << 12); // Set PA6 as output
//PA7
    GPIOA->MODER &= ~(0b11 << 14); // Clear PA7
    GPIOA->MODER |=  (0b01 << 14); // Set PA7 as output
		


    GPIOA->OSPEEDR |=  (0b11 << 12) | (0b11 << 14); // High speed
}