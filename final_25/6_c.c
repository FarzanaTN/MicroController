/*
USART Reception Interrupts Configuration:

    The STM32 USART peripheral uses the CR1 register to enable reception interrupts. The key bit to set is RXNEIE (Receive Not Empty Interrupt Enable), which triggers an interrupt when data is received and the receive data register (DR) is not empty.
    Bits to Set:
        USART_CR1_RXNEIE (Bit 5): Enable the RXNE interrupt. This bit must be set in the CR1 register to allow the USART to generate an interrupt when a character is received.
        USART_CR1_UE (Bit 13): Enable the USART itself, which is a prerequisite for interrupt operation.
    Bits to Check:
        USART_SR_RXNE (Bit 5 in the SR register): Check this status bit in the interrupt service routine (ISR) to confirm that the interrupt was triggered due to received data. This bit is set by hardware when data is available in the DR register.

Interrupt Service Routine (ISR) Activation:

    The ISR for reception is activated when the RXNE flag is set in the SR register and the RXNEIE bit is enabled in CR1. This occurs when the USART receives a new data byte and the DR register is ready to be read.
    The ISR must clear the RXNE flag implicitly by reading DR or explicitly by writing 0 to SR if needed (though reading DR is sufficient in most cases).

    Explanation:

    Setting RXNEIE enables the interrupt, and checking RXNE in the ISR confirms the interrupt cause.
    The ISR is triggered on each byte reception, allowing real-time data handling.
    */


void USART_Config(void) {
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;  // Enable USART1 clock (example)
    USART1->CR1 |= USART_CR1_RXNEIE | USART_CR1_UE;  // Enable RXNE interrupt and USART
    NVIC_EnableIRQ(USART1_IRQn);           // Enable NVIC for USART1
    __enable_irq();                        // Globally enable interrupts
}

void USART1_IRQHandler(void) {
    if (USART1->SR & USART_SR_RXNE) {      // Check RXNE flag
        uint8_t received_data = USART1->DR;  // Read data to clear RXNE
        // Process received_data (e.g., store in buffer)
    }
}