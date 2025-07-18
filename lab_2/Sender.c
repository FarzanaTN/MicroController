#include "stm32f446xx.h"
#include "CLOCK.h"
#include "UART.h"



int main(void) {
    initClock();
    setup_uarts();
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