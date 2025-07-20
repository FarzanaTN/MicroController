#include "InterruptHandler.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "global.h"
#include <stdlib.h>
#include "SPI.h"

void USART2_IRQHandler(void)
{

	if (USART2->SR & USART_SR_RXNE)
	{
		while (!(USART2->SR & USART_SR_RXNE))
			;
		char c = (uint8_t)USART2->DR;
		con_buffer_in[con_in++] = c;
		USART2->SR &= ~USART_SR_RXNE;

		if (c == '\0' || c == '\n')
		{
			con_buffer_in[con_in] = '\n';
			con_flag = 1;
			con_in = 0;
			// UART_SendString(USART2,con_buffer_in);
		}
	}
	if (USART2->SR & USART_SR_TXE)
	{

		USART2->DR = con_buffer_outp[con_out];
		while (!(USART2->SR & USART_SR_TXE))
			;

		USART2->SR &= ~(USART_SR_TXE);
		USART2->CR1 &= ~(USART_CR1_TXEIE);
	}
}
void SPI3_IRQHandler(void)
{

    if (SPI3->SR & SPI_SR_RXNE) // Check for RXNE interrupt
    {
        // Read the data from the SPI data register
        uint8_t data = SPI3->DR;
        UART_SendChar(USART2,data);
				
        // Process the received data
        // ...
    }
}


/*
void I2C2_EV_IRQHandler(void)
{
	// UART_SendString(USART2,"HELLO\n");

//	UART_SendChar(USART2,'H');

	if (I2C2->SR1 & I2C_SR1_ADDR)
	{

		// Address matched, clear the ADDR flag
		(void)I2C2->SR2;
	}

	if (I2C2->SR1 & I2C_SR1_RXNE)
	{
		// Data received, read the data register
		uint8_t tmp;
		tmp = I2C2->DR;

//		UART_SendChar(USART2, tmp);
		
		out_buffer[out++] = tmp;

		if (tmp == '\n')
		{
			slave_flag = 1;
		}
	}
}
*/