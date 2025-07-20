#include "CLOCK.h"
#include "USART.h"
#include "GPIO.h"
#include "SYS_INIT.h"
#include "InterruptHandler.h"
#include "Timer.h"
#include "global.h"
#include "SPI.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//________________________________________USART__________________________________________
char in_buffer[500];
char out_buffer[500];
char con_buffer_in[500];
char con_buffer_outp[500];
int in;
int out;
int con_in;
int con_out;
int con_flag;
int tim_flag;
int slave_flag;
int traffic_monitor;
// sending from 6 to 1

char traffic1[200];
char traffic2[200];
char traffic3[200];
char final_output[500];

char tmp[100];

int a, b, c;
int x, w, y, z, u, D;
int green, yellow;

int red = 3;
int extra = 5;
int delay;

void handle_string(char *str, int index, int type)
{
	if (type == 5)
	{
		sprintf(str + strlen(str), "%d G Y R %d %d %d %d", index, green, yellow, red, extra);
	}
	else if (type == 3)
	{
		sprintf(str + strlen(str), "%d", traffic_monitor);
	}
}
int handle_input(char *str)
{
	if (sscanf(str, "\nconfig traffic light %d G Y R %d %d %d %d", &x, &w, &y, &z, &u) == 5)
		return 1;
	else if (sscanf(str, "\nconfig traffic monitor %d", &D) == 1)
		return 2;
	else if (strcmp(str, "\nread traffic monitor") == 0)
		return 3;
	else if (strcmp(str, "\nread") == 0)
		return 4;
	else if (sscanf(str, "\nread traffic light %d", &x) == 1)
		return 5;
	else
		return 0;
}
void process_output()
{
	int type = handle_input(out_buffer);

	if (type == 1)
	{
		green = w,
		yellow = y,
		red = z,
		extra = u;
	}

	else if (type == 2)
	{
		traffic_monitor = D;
		strcpy(traffic1, "");
		strcpy(traffic2, "");
		strcpy(traffic3, "");
	}

	else if (type == 3)
	{
		strcpy(tmp, "\ntraffic monitor ");
		handle_string(tmp, 0, type);
		strcat(tmp, "\n");
	}
	else if (type == 4)
	{
		strcpy(tmp, "");
		strcpy(tmp, "\ntraffic light ");
		handle_string(tmp, 1, 5);
		strcat(tmp, "\ntraffic light ");
		handle_string(tmp, 2, 5);
		strcat(tmp, "\ntraffic monitor ");
		handle_string(tmp, 0, 3);
		strcat(tmp, "\n");
	}
	else if (type == 5)
	{
		strcpy(tmp, "\ntraffic light ");
		handle_string(tmp, x, type);
		strcat(tmp, "\n");
	}
	UART_SendString(USART2, tmp);
	strcpy(tmp, "");
}
char *intToString(int num)
{
	static char str[20];
	sprintf(str, "%d", num);
	return str;
}
int trafficGenerator(void)
{
	return rand() % 2;
}

// Read the Trimming parameters saved in the NVM ROM of the device
void read (uint8_t address,uint8_t size)
{
	CS_Enable ();  // pull the pin lowF
	SPI2_Transmit (&address, 1);  // send address
//	SPI_Receive (RxData, size);  // receive 6 bytes data
	CS_Disable ();  // pull the pin high
}

void write (uint8_t *value)
{
				CS_Enable ();  // pull the cs pin low

	for(int i=0; i<strlen(value); i++)
	{
		
		uint8_t data[2];
		data[0] = value[i];
		ms_delay(10);
		SPI2_Transmit (data, 1);  // write data to register

	}
				CS_Disable ();  // pull the cs pin high


	
}
void control_to_light(char *s)
{
	write(s);
	UART_SendString(USART2,s);
}

int main()
{
	/*con_in = 0;
	con_out = 0;
	con_flag = 0;
	tim_flag = 0;
	slave_flag = 0;
	out = 0;
	// usart1 for light, usart6 for control
	initClock();
	sysInit();
	UART2_Config();
	TIM6_Config();
	I2C_Config();
	I2C2_Config();
	traffic_monitor = 5;

	strcpy(traffic1, "");
	strcpy(traffic2, "");
	strcpy(traffic3, "");
	strcpy(final_output, "");

	RCC->AHB1ENR |= (1 << 0);
	RCC->AHB1ENR |= (1 << 1);
	RCC->AHB1ENR |= (1 << 2);

	GPIOA->MODER |= (1U << 10);
	GPIOA->MODER |= (1U << 12);
	GPIOA->MODER |= (1U << 18);
	GPIOA->MODER |= (1U << 20);
	GPIOA->MODER |= (1U << 22);
	GPIOA->MODER |= (1U << 24);

	GPIOA->ODR |= (1U << 9);
	GPIOA->ODR |= (1U << 10);

	delay = red * 1000;

	while (1)
	{
		a = (GPIOA->IDR & (1U << 7));
		b = (GPIOA->IDR & (1U << 4));

		int x = trafficGenerator();
		int y = trafficGenerator();

		if (x == 1)
			GPIOA->ODR |= (1U << 5);

		if (y == 1)
			GPIOA->ODR |= (1U << 11);

		if (a)
		{
			ms_delay(delay);

			c = (GPIOA->IDR & (1U << 8));

			if (c)
				delay = extra * 1000;
			else
				delay = red * 1000;

			if (con_flag == 1)
			{
				con_flag = 0;
				control_to_light(con_buffer_in);
				strcpy(con_buffer_in, "");
			}
			else if (slave_flag == 1)
			{
				slave_flag = 0;
				process_output();
			}

			GPIOA->ODR &= (0U << 9);
			GPIOA->ODR &= (0U << 10);
			GPIOA->ODR |= (1U << 12);
			GPIOA->ODR |= (1U << 6);
		}
		if (b)
		{
			ms_delay(delay);

			c = (GPIOB->IDR & (1U << 3));

			if (c)
				delay = extra * 1000;
			else
				delay = red * 1000;

			if (con_flag == 1)
			{
				con_flag = 0;
				control_to_light(con_buffer_in);
				strcpy(con_buffer_in, "");
			}
			else if (slave_flag == 1)
			{
				slave_flag = 0;
				process_output();
			}

			GPIOA->ODR &= (0U << 12);
			GPIOA->ODR &= (0U << 6);
			GPIOA->ODR |= (1U << 9);
			GPIOA->ODR |= (1U << 10);
		}
	}*/

	initClock();
	sysInit();
	gpio_usart_init();
	UART2_Config();
	TIM6_Config();
	SPI2_Config();
	//SPI3_Config();
	SPI2_Enable();
	//SPI3_Enable();
	strcpy(con_buffer_in,"");
	con_in=0;
	
	while(1)
	{
			

		if(con_flag==1)
		{
			con_flag=0;
			control_to_light(con_buffer_in);
			strcpy(con_buffer_in,"");
			con_in=0;
		}

		
	}
	
	
}