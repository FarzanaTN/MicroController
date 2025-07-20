#include "CLOCK.h"
#include "GPIO.h"
#include "SYS_INIT.h"
#include "USART.h"
#include "I2C.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "stm32f4xx.h"

static char input_buff[100];
static uint8_t rcv_ptr;
static char *rcv_str;

#define TIM6_CLOCK ((uint32_t)0x0010)
#define TIM6_CR1_CEN ((uint32_t)0x0001)
#define TIM6_SR_UIF ((uint32_t)0x0001)

static uint32_t global_time = 0;

char tmp[100];

int timer = 0;
int e_tmp = 0;

int a, b, c, d;
int x, w, y, z, u, D;

int green, yellow;

int red = 3;
int green = 3;
int	yellow = 0;
int extra = 5;
int delay;
int traffic_monitor=10;

void TIM3Config(void);

void I2C1_EV_IRQHandler(void);

void USART2_IRQHandler(void);
void I2C1_EV_IRQHandler(void);
void getString(void);
void init(void);
void mainLoop(void);


char traffic1[200];
char traffic2[200];
char traffic3[200];
char final_output[500];

char t1[4][200] = {
	"traffic light 1 ",
	"traffic light 2 ",
	"road north south ",
	"road east west "};

char t2a[4][200] = {
	"ON ON OFF",
	"OFF OFF ON"};

char t2b[4][200] = {
	"heavy traffic",
	"light traffic"};

void TIM6_DAC_IRQHandler(void)
{
	
//check if update flag is set	
	if (TIM6->SR & TIM_SR_UIF)
	{
		TIM6->SR &= ~TIM_SR_UIF;

		timer++;

		if ((timer % traffic_monitor) == 0)
		{
			strcpy(final_output, "\n----------------------------------");

			char str[10];
			sprintf(str, "\n%d ", timer);
//			strcpy(str, "\n");
			strcpy(traffic3, traffic2);
			strcpy(traffic2, traffic1);
			strcpy(traffic1, "");

			for (int j = 0; j < 4; j++)
			{
				strcat(traffic1, str);
				strcat(traffic1, t1[j]);

				int x = rand()%2;
				int y = rand()%2;

				if (j == 0)
				{
					if (a == 0)
					{
						strcat(traffic1, t2a[0]);
						strcat(traffic1, str);
						strcat(traffic1, t1[j+1]);
						strcat(traffic1, t2a[1]);
					}
					else
					{
						strcat(traffic1, t2a[1]);
						strcat(traffic1, str);
						strcat(traffic1, t1[j+1]);
						strcat(traffic1, t2a[0]);
					}
					j++;
				}
				else if (j == 2)
				{
					c = (GPIOB->IDR & (1U << 3));

					if (c)
						strcat(traffic1, t2b[0]);
					else
						strcat(traffic1, t2b[1]);
				}
				else
				{
					d = (GPIOA->IDR & (1U << 4));

					if (d)
						strcat(traffic1, t2b[0]);
					else
						strcat(traffic1, t2b[1]);
				}
			}

			strcat(final_output, traffic3);
			strcat(final_output, traffic2);
			strcat(final_output, traffic1);

			UART_SendString(USART2, final_output);
		}
	}
}
void TIM6_Config(void)
{
	/************** STEPS TO FOLLOW *****************
	1. Enable Timer clock
	2. Set the prescalar and the ARR
	3. Enable update interrupt and timer 6 interrupt
	4. Enable the Timer
	************************************************/
	
// 1. Enable Timer clock	
	RCC->APB1ENR |= TIM6_CLOCK;  // Enable the timer6 clock

// 2. Set the prescalar and the ARR	
	TIM6->PSC = 8999;	//10 KHz
	TIM6->ARR = 9999;	//1s

//	3. Enable update interrupt and timer 6 interrupt	
	TIM6->DIER |= TIM_DIER_UIE; // Enable update interrupt
	NVIC_EnableIRQ(TIM6_DAC_IRQn); // Enable timer 6 interrupt

// 3. Enable the Timer	
	TIM6->CR1 |= TIM_CR1_CEN; // Enable the counter
}

void TIM3Config(void){
    RCC->APB1ENR |= (1<<1);
	
	TIM3->PSC = 45000 - 1; /* fck = 90 mhz, CK_CNT = fck / (psc[15:0] + 1)*/
	TIM3->ARR = 0xFFFF; /*maximum clock count*/
	
	TIM3->CR1 |= (1<<0);
	
	while(!(TIM3->SR & (1<<0)));
}
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
char *intToString(int num)
{
	static char str[20];
	sprintf(str, "%d", num);
	return str;
}
int handle_input(char *str)
{
	if (sscanf(str, "config traffic light %d G Y R %d %d %d %d", &x, &w, &y, &z, &u) == 5)
		return 1;
	else if (sscanf(str, "config traffic monitor %d", &D) == 1){
//		UART_SendString(USART2, intToString(timer));
		return 2;		
	}

	else if (strcmp(str, "read traffic monitor") == 0)
		return 3;
	else if (strcmp(str, "read") == 0)
		return 4;
	else if (sscanf(str, "read traffic light %d", &x) == 1)
		return 5;
	else
		return 0;
}
void process_output()
{
	int type = handle_input(input_buff);

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
		timer = e_tmp;
		strcpy(traffic1, "");
		strcpy(traffic2, "");
		strcpy(traffic3, "");
	}

	else if (type == 3)
	{
		strcpy(tmp, "\ntraffic monitor ");
		handle_string(tmp, 0, type);
		strcat(tmp, "\n");
		UART_SendString(USART2, tmp);
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
		UART_SendString(USART2, tmp);	
	}
	else if (type == 5)
	{
		strcpy(tmp, "\ntraffic light ");
		handle_string(tmp, x, type);
		strcat(tmp, "\n");
		UART_SendString(USART2, tmp);		
	}
	strcpy(tmp, "");
}
void getString(void){
    uint8_t ch,idx = 0;
    ch = UART_GetChar(USART2);
    while(ch != '!'){
        input_buff[idx++] = ch;
        ch = UART_GetChar(USART2);
        if(ch == '!')break;
    }      
    input_buff[idx] = '\0';
    
}
void USART2_IRQHandler(void){
    USART2->CR1 &= ~(USART_CR1_RXNEIE);
    getString();
    USART2->CR1 |= (USART_CR1_RXNEIE);

}
void I2C1_EV_IRQHandler(void){ 
		e_tmp = timer;    
    I2C1->CR2 &= ~I2C_CR2_ITEVTEN;

    if(I2C1->SR1 & I2C_SR1_ADDR)
        rcv_str = I2C1_ReceiveSlave(&rcv_ptr);     
		
    if(strlen(rcv_str) != 0){
        strcpy(input_buff,rcv_str);
        sendString(input_buff);
				process_output();
    }

    I2C1->CR2 |= I2C_CR2_ITEVTEN;
}
int main(void)
{   
		//1 master
		//0 slave
		uint8_t setupMode = 0;
		I2C1_Config(setupMode);
     	
		initClock();
		sysInit();
		TIM3Config();
		UART2_Config();
		TIM6_Config();
	
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

//		GPIOA->ODR |= (1U << 5);
//		GPIOA->ODR |= (1U << 11);

		delay = red * 1000;
//		GPIOA->ODR |= (1U << 5);
//		GPIOA->ODR |= (1U << 10);
//		GPIOA->ODR |= (1U << 9);
//		GPIOA->ODR |= (1U << 6);
//		GPIOA->ODR |= (1U << 11);
//		GPIOA->ODR |= (1U << 12);

    strcpy(input_buff,"");
    
    if(!setupMode){
        strcpy(input_buff,"");
       while(true)
			 {
				 	I2C1->CR1 |= I2C_CR1_ACK;

					a = (GPIOA->IDR & (1U << 7));
					b = (GPIOC->IDR & (1U << 7));

					int x = rand()%2;
					int y = rand()%2;

					if (x==1)
						GPIOA->ODR |= (1U << 5);
					if (y==1)
						GPIOA->ODR |= (1U << 11);
				 
					if (a){

							ms_delay(delay);						

							c = (GPIOA->IDR & (1U << 8));

							if (c)
									delay = extra * 1000;
							else
									delay = red * 1000;						

							GPIOA->ODR &= (0U << 9);
							GPIOA->ODR &= (0U << 10);
							GPIOA->ODR |= (1U << 12);
							GPIOA->ODR |= (1U << 6);
						
					}
					if (b) {
						
							ms_delay(delay);						

							c = (GPIOB->IDR & (1U << 3));

							if (c)
									delay = extra * 1000;
							else
									delay = red * 1000;
						
							GPIOA->ODR &= (0U << 12);
							GPIOA->ODR &= (0U << 6);
							GPIOA->ODR |= (1U << 9);
							GPIOA->ODR |= (1U << 10);						
				}				 
			 }
    }
    else{
        while(setupMode){
            if (strlen(input_buff) != 0){          
                sendString(input_buff); 
                while(!I2C1_TransmitMaster(input_buff,strlen(input_buff)));
                strcpy(input_buff, "");
            }
        }
    }
    
}