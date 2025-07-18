void USART_Config(void) {
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;  // Enable USART1 clock
    USART1->BRR = 800;                     // Mantissa=3, Fractional=32 for ~2 Mbps with 100 MHz PCLK
    USART1->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;  // Enable TX, RX, USART
}

/*
(c) [4 points] UART Baud Rate Calculation

Given:

    MCU clock = 400 MHz

    Target baud rate = 2 Mbps

    USARTx->BRR register:

        22 bits mantissa

        8 bits fraction

🧮 Formula:
USARTDIV=fckBaudRate=400 MHz2 Mbps=200
USARTDIV=BaudRatefck​​=2Mbps400MHz​=200

Since 200 is a whole number:

    Mantissa = 200

    Fraction = 0

✅ Final Answer:

    Mantissa = 200

    Fraction = 0

These values will be written into USARTx->BRR register (total 30-bit field: 22 bits mantissa + 8 bits fraction).

*/

