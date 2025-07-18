/*
 * =================================================================================
 * -- FILE: MCU 2 (Traffic Light Controller - FULL LOGIC) @ 180MHz - main.c
 * --
 * --
 * -- RESPONSIBILITIES:
 * --   1. Handles ALL application logic.
 * --   2. Receives character stream from MCU1 via UART4.
 * --   3. Assembles characters into a command string, terminated by newline.
 * --   4. Prints debug messages to its own PC terminal (USART2).
 * --   5. Processes completed commands and controls LEDs.
 * --   6. Sends official responses back to MCU1.
 * --
 * -- HARDWARE:
 * --   - USART2 (PA2/PA3) -> PC #2 Virtual COM Port (FOR DEBUGGING)
 * --   - UART4_TX  (PA0) -> MCU1 PA1 (RX)
 * --   - UART4_RX  (PA1) -> MCU1 PA0 (TX)
 * --   - LEDs            -> PA5-PA10
 * --   - GND             -> MCU1 GND
 * =================================================================================
 */

#include "stm32f446xx.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "clock.h"

// --- Global Variables and Typedefs ---
typedef enum { STATE_G1_R2, STATE_Y1_R2, STATE_R1_G2, STATE_R1_Y2 } TrafficState;
volatile TrafficState current_state = STATE_G1_R2;
volatile unsigned long g1_duration = 10, y1_duration = 2;
volatile unsigned long g2_duration = 10, y2_duration = 3;
volatile uint32_t state_countdown_s = 10;

#define CMD_BUFFER_SIZE      100
volatile char uart4_cmd_buffer[CMD_BUFFER_SIZE];
volatile uint8_t uart4_cmd_idx = 0;
volatile uint8_t uart4_cmd_ready = 0;

#define R1_PIN (1U << 5)
#define Y1_PIN (1U << 6)
#define G1_PIN (1U << 7)
#define R2_PIN (1U << 8)
#define Y2_PIN (1U << 9)
#define G2_PIN (1U << 10)
#define ALL_LIGHTS (R1_PIN | Y1_PIN | G1_PIN | R2_PIN | Y2_PIN | G2_PIN)

// --- Helper Functions ---
void uart_send_string(USART_TypeDef* UARTx, const char* str) {
    for (uint32_t i = 0; str[i] != '\0'; i++) {
        UARTx->DR = str[i];
        while (!(UARTx->SR & USART_SR_TXE));
    }
}

// --- Light & Command Processing ---
void update_lights_for_state() {
    GPIOA->ODR &= ~ALL_LIGHTS;
    switch (current_state) {
        case STATE_G1_R2: GPIOA->ODR |= (G1_PIN | R2_PIN); break;
        case STATE_Y1_R2: GPIOA->ODR |= (Y1_PIN | R2_PIN); break;
        case STATE_R1_G2: GPIOA->ODR |= (R1_PIN | G2_PIN); break;
        case STATE_R1_Y2: GPIOA->ODR |= (R1_PIN | Y2_PIN); break;
    }
}

void process_and_execute_command() {
    char response[150]; // Increased buffer size
    int light_num;
    unsigned long w, y, z, u;

    // Print what we are about to process to our own debug terminal
    uart_send_string(USART2, "Processing command: '");
    uart_send_string(USART2, (const char*)uart4_cmd_buffer);
    uart_send_string(USART2, "'\r\n");

    if (sscanf((const char*)uart4_cmd_buffer, "config traffic light %d G Y R %lu %lu %lu %lu", &light_num, &w, &y, &z, &u) == 5) {
        if (light_num == 1) { g1_duration = w > 0 ? w : 1; y1_duration = y > 0 ? y : 1; }
        else if (light_num == 2) { g2_duration = w > 0 ? w : 1; y2_duration = y > 0 ? y : 1; }
        sprintf(response, "\nACK: Configured light %d.", light_num);
    } 
    else if (strcmp((const char*)uart4_cmd_buffer, "read") == 0) {
        sprintf(response, "\nlight 1 G Y R %lu %lu %lu 0\r\nlight 2 G Y R %lu %lu %lu 0",
            g1_duration, y1_duration, g2_duration + y2_duration,
            g2_duration, y2_duration, g1_duration + y1_duration);
    } else if (sscanf((const char*)uart4_cmd_buffer, "read traffic light %d", &light_num) == 1) {
        if (light_num == 1) { sprintf(response, "\nlight 1 G Y R %lu %lu %lu 0", g1_duration, y1_duration, g2_duration + y2_duration); }
        else if (light_num == 2) { sprintf(response, "\nlight 2 G Y R %lu %lu %lu 0", g2_duration, y2_duration, g1_duration + y1_duration); }
        else { sprintf(response, "\nNACK: Invalid traffic light number."); }
    } else {
        sprintf(response, "\nNACK: Unknown Command.");
    }
    
    uart_send_string(USART2, "Sending response: '");
    uart_send_string(USART2, response);
    uart_send_string(USART2, "'\r\n");

    // Send the official response back to MCU1
    uart_send_string(UART4, response);
    uart_send_string(UART4, "\r\n"); // Send with CR+LF for good measure
}


void setup_peripherals() {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_UART4EN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    // GPIO AF setup for both UARTs
    GPIOA->MODER &= ~((3U << 0) | (3U << 2) | (3U << 4) | (3U << 6));
    GPIOA->MODER |= ((2U << 0) | (2U << 2) | (2U << 4) | (2U << 6));
    GPIOA->AFR[0] &= ~((0xF << 0) | (0xF << 4) | (0xF << 8) | (0xF << 12));
    GPIOA->AFR[0] |= ((8U << 0) | (8U << 4) | (7U << 8) | (7U << 12));
    
    // LED GPIOs (PA5-PA10) as outputs
    GPIOA->MODER &= ~(0x3FFU << 10);
    GPIOA->MODER |= (0x155U << 10);

    // Configure UART peripherals
    UART4->BRR = 4687;
    USART2->BRR = 4687;
    
    UART4->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE | USART_CR1_UE;
    USART2->CR1 = USART_CR1_TE | USART_CR1_UE; // USART2 only needs to transmit for debug

    SysTick_Config(180000000);
}


int main(void) {
    initClock();
    setup_peripherals();
    SystemCoreClockUpdate();
    
    NVIC_EnableIRQ(UART4_IRQn);
    __enable_irq();
    
    uart_send_string(USART2, "\r\n--- MCU2: Traffic Controller Online ---\r\n");

    state_countdown_s = g1_duration;
    update_lights_for_state();

    while(1) {
        if (uart4_cmd_ready) {
            process_and_execute_command();
            uart4_cmd_ready = 0;
            uart4_cmd_idx = 0;
            memset((void*)uart4_cmd_buffer, 0, CMD_BUFFER_SIZE);
        }
    }
}


void SysTick_Handler(void) { /* ... NO CHANGE from your last full code ... */
    if (state_countdown_s > 0) { state_countdown_s--; }
    if (state_countdown_s == 0) {
        switch (current_state) {
            case STATE_G1_R2: current_state = STATE_Y1_R2; state_countdown_s = y1_duration; break;
            case STATE_Y1_R2: current_state = STATE_R1_G2; state_countdown_s = g2_duration; break;
            case STATE_R1_G2: current_state = STATE_R1_Y2; state_countdown_s = y2_duration; break;
            case STATE_R1_Y2: current_state = STATE_G1_R2; state_countdown_s = g1_duration; break;
        }
        update_lights_for_state();
    }
}

// IRQ handles receiving a character stream from MCU1 and building a command
void UART4_IRQHandler(void) {
    if (UART4->SR & USART_SR_RXNE) {
        char c = (char)UART4->DR;
        
        // Command is terminated by newline or carriage return
        if (c == '\n' || c == '\r') {
            if (uart4_cmd_idx > 0) {
                uart4_cmd_buffer[uart4_cmd_idx] = '\0'; // Null-terminate string
                uart4_cmd_ready = 1; // Signal main loop to process
            }
        } 
        else if (uart4_cmd_idx < CMD_BUFFER_SIZE - 1) {
            uart4_cmd_buffer[uart4_cmd_idx++] = c;
        }
    }
}