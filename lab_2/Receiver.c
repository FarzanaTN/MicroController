#include "stm32f446xx.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "CLOCK.h"
#include "UART.h"
#include "TIMER.h"
#include "GPIO.h"

#define MAX_REPORT_LEN 128
#define RED_1 (1U << 0)
#define YEL_1 (1U << 1)
#define GRN_1 (1U << 2)
#define RED_2 (1U << 3)
#define YEL_2 (1U << 4)
#define GRN_2 (1U << 5)

#define LOAD_1_LED (1 << 6)
#define LOAD_2_LED (1 << 7)

#define ALL_LIGHTS (RED_1 | YEL_1 | GRN_1 | RED_2 | YEL_2 | GRN_2 | LOAD_1_LED | LOAD_2_LED)

typedef enum
{
    NORTH_SOUTH_GREEN,
    YELLOW1,
    EAST_WEST_GREEN,
    YELLOW2
} TrafficState;
volatile TrafficState current_state = NORTH_SOUTH_GREEN;
volatile uint32_t g1_time = 10, y1_time = 2, r1_time = 10, u1_time = 5;
volatile uint32_t g2_time = 10, y2_time = 2, r2_time = 10, u2_time = 3;
volatile uint32_t monitor_interval = 10;

#define CMD_BUFFER_SIZE 100
volatile char uart4_cmd_buffer[CMD_BUFFER_SIZE];
volatile uint8_t uart4_cmd_idx = 0;
volatile uint8_t uart4_cmd_ready = 0;

// UART helper , Sends a string out of one UART manually
void uart_send_string(USART_TypeDef *UARTx, const char *str)
{
    while (*str)
    {
        UARTx->DR = *str++;
        while (!(UARTx->SR & USART_SR_TXE));
    }
}

void update_lights_for_state(void)
{
    GPIOB->BSRR = (ALL_LIGHTS << 16); // Reset all lights
    switch (current_state)
    {
    case NORTH_SOUTH_GREEN:
        GPIOB->BSRR = GRN_1 | RED_2;
        break;
    case YELLOW1:
    case YELLOW2:
        GPIOB->BSRR = YEL_1 | YEL_2;
        break;
    case EAST_WEST_GREEN:
        GPIOB->BSRR = GRN_2 | RED_1;
        break;
    }
}

void simulate_random_load(uint8_t *ns_high, uint8_t *ew_high)
{
    *ns_high = (rand() % 2); // 0 or 1
    *ew_high = (rand() % 2);

    GPIO_WritePin(GPIOA, LOAD_1_LED, *ns_high);
    GPIO_WritePin(GPIOA, LOAD_2_LED, *ew_high);
}

void send_monitoring_report(void)
{
    char report[256];
    uint8_t ns_load, ew_load;
    simulate_random_load(&ns_load, &ew_load);

    sprintf(report, "traffic light 1 %s %s %s\r\n",
            (current_state == NORTH_SOUTH_GREEN) ? "ON" : "OFF",
            (current_state == YELLOW1 || current_state == YELLOW2) ? "ON" : "OFF",
            (current_state == EAST_WEST_GREEN) ? "ON" : "OFF");
    uart_send_string(UART4, report);

    sprintf(report, "traffic light 2 %s %s %s\r\n",
            (current_state == EAST_WEST_GREEN) ? "ON" : "OFF",
            (current_state == YELLOW1 || current_state == YELLOW2) ? "ON" : "OFF",
            (current_state == NORTH_SOUTH_GREEN) ? "ON" : "OFF");
    uart_send_string(UART4, report);

    sprintf(report, "road north south %s traffic\r\n", ns_load ? "heavy" : "light");
    uart_send_string(UART4, report);

    sprintf(report, "road east west %s traffic\r\n", ew_load ? "heavy" : "light");
    uart_send_string(UART4, report);
}


void process_and_execute_command(void)
{
    char response[256];
    int light_num, interval;
    unsigned long w, y, z, u;

    //
    uart_send_string(USART2, "Processing command: ");
    uart_send_string(USART2, (const char *)uart4_cmd_buffer);
    uart_send_string(USART2, "\r\n");
    //

    if (sscanf((const char *)uart4_cmd_buffer, "config traffic light %d G Y R %lu %lu %lu %lu",
               &light_num, &w, &y, &z, &u) == 5)
    {
        if (light_num == 1)
        {
            g1_time = w;
            y1_time = y;
            r1_time = z;
            u1_time = u;
        }
        else if (light_num == 2)
        {
            g2_time = w;
            y2_time = y;
            r2_time = z;
            u2_time = u;
        }
        sprintf(response, "\nACK: Configured light %d.\r\n", light_num);
    }
    else if (sscanf((const char *)uart4_cmd_buffer, "config traffic monitor %d", &interval) == 1)
    {
        if (interval > 0)
        {
            monitor_interval = interval;
            sprintf(response, "\nACK: Monitoring interval set to %d seconds.\r\n", interval);
        }
        else
        {
            sprintf(response, "\nNACK: Invalid interval.\r\n");
        }
    }
    else if (strcmp((const char *)uart4_cmd_buffer, "read") == 0)
    {
        sprintf(response, "traffic light 1 G Y R %lu %lu %lu %lu\r\ntraffic light 2 G Y R %lu %lu %lu %lu\r\ntraffic monitor %d\r\n",
                g1_time, y1_time, r1_time, u1_time,
                g2_time, y2_time, r2_time, u2_time,
                monitor_interval);
    }
    else
    {
        sprintf(response, "\nNACK: Unknown Command.\r\n");
    }
    uart_send_string(UART4, response);
}

int main(void)
{
    initClock();
    traffic_init();
    setup_uarts();
    TIM2_Init();
    NVIC_EnableIRQ(UART4_IRQn);
    __enable_irq();

    uart_send_string(USART2, "\r\n--- MCU2 Online ---\r\n");

    while (1)
    {
        // Run traffic signal
        current_state = NORTH_SOUTH_GREEN;
        update_lights_for_state();
        Delay(g1_time * 1000);

        current_state = YELLOW1;
        update_lights_for_state();
        Delay(y1_time * 1000);

        current_state = EAST_WEST_GREEN;
        update_lights_for_state();
        Delay(g2_time * 1000);

        current_state = YELLOW2;
        update_lights_for_state();
        Delay(y2_time * 1000);

        send_monitoring_report();
        Delay(monitor_interval * 1000);

        if (uart4_cmd_ready)
        {
            process_and_execute_command();
            uart4_cmd_ready = 0;
            uart4_cmd_idx = 0;
            memset((void *)uart4_cmd_buffer, 0, CMD_BUFFER_SIZE);
        }
    }
}

void UART4_IRQHandler(void)
{
    if (UART4->SR & USART_SR_RXNE)
    {
        char c = (char)UART4->DR;
        //
        USART2->DR = c;
        while (!(USART2->SR & USART_SR_TXE))
            ;
        //
        // Command is terminated by newline
        if (c == '\n' || c == '\r')
        {
            if (uart4_cmd_idx > 0)
            {
                uart4_cmd_buffer[uart4_cmd_idx] = '\0'; // Null-terminate string
                uart4_cmd_ready = 1;                    // Signal main loop to process
            }
        }
        else if (uart4_cmd_idx < CMD_BUFFER_SIZE - 1)
        {
            uart4_cmd_buffer[uart4_cmd_idx++] = c;
        }
    }
}
