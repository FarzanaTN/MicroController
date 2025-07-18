#include "stm32f446xx.h"
#include "CLOCK.h"

void setup_uarts() {
    // Enable clocks for all peripherals
	
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    RCC->APB1ENR |= RCC_APB1ENR_UART4EN;

    // Configure GPIO Pins PA0, PA1, PA2, PA3 to AF mode
	
    GPIOA->MODER &= ~((3U<<0) | (3U<<2) | (3U<<4) | (3U<<6));
    GPIOA->MODER |= (2U<<0) | (2U<<2) | (2U<<4) | (2U<<6);
    
    GPIOA->AFR[0] &= ~((0xF<<0) | (0xF<<4) | (0xF<<8) | (0xF<<12));
    GPIOA->AFR[0] |= (7U<<8) | (7U<<12); // AF7 for USART2 (PA2, PA3)
    GPIOA->AFR[0] |= (8U<<0) | (8U<<4);  // AF8 for UART4 (PA0, PA1)
    
    // Configure USART2 and UART4
    // PCLK1 is 45MHz. Baud rate is 9600. BRR = 45,000,000 / 9600 = 4687.5 => 0x1253
    // Integer division is fine too: 45000000 / 9600 = 4687
    USART2->BRR = 4687;
    UART4->BRR  = 4687;
    
    // Enable Tx, Rx, Rx-Interrupt, and the peripherals themselves
    USART2->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE | USART_CR1_UE;
    UART4->CR1  = USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE | USART_CR1_UE;
}
