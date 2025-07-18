/*
 * =================================================================================
 * -- FILE: MCU 1 (Control Center - PURE RELAY) @ 180MHz - main.c
 * --
 * --
 * -- RESPONSIBILITIES:
 * --   1. Acts as a transparent two-way bridge.
 * --   2. Forwards any character from PC (USART2) directly to MCU2 (UART4).
 * --   3. Forwards any character from MCU2 (UART4) directly to PC (USART2).
 * --
 * -- HARDWARE:
 * --   - USART2 (PA2/PA3) -> PC #1 Virtual COM Port
 * --   - UART4_TX  (PA0) -> MCU2 PA1 (RX)
 * --   - UART4_RX  (PA1) -> MCU2 PA0 (TX)
 * --   - GND             -> MCU2 GND
 * =================================================================================
 */
#include "stm32f446xx.h"
#include "clock.h"

// No global buffers or flags needed, this is a direct relay.

void setup_all_uarts() {
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


int main(void) {
    initClock();
    setup_all_uarts();
    SystemCoreClockUpdate();
    
    // Enable interrupts for both UARTs
    NVIC_EnableIRQ(USART2_IRQn);
    NVIC_EnableIRQ(UART4_IRQn);
    __enable_irq();

    // The main loop does nothing. Everything is handled by interrupts.
    while(1) {
        // MCU sits idle, waiting for interrupts
    }
}

// This IRQ fires when a character is received from the PC terminal
void USART2_IRQHandler(void) {
    if (USART2->SR & USART_SR_RXNE) {
        // Read character from PC
        char char_from_pc = (char)USART2->DR;
        
        // Immediately forward it to MCU2
        UART4->DR = char_from_pc;
        while(!(UART4->SR & USART_SR_TXE));
    }
}

// This IRQ fires when a character is received from MCU2
void UART4_IRQHandler(void) {
    if (UART4->SR & USART_SR_RXNE) {
        // Read character from MCU2
        char char_from_mcu2 = (char)UART4->DR;

        // Immediately forward it to the PC
        USART2->DR = char_from_mcu2;
        while(!(USART2->SR & USART_SR_TXE));
    }
}