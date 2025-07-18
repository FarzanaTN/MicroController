#include "stm32f4xx.h"

#define BUFFER_SIZE 3072  // 3 KB
uint8_t receive_buffer[BUFFER_SIZE];
volatile uint32_t buffer_index = 0;
volatile uint8_t data_ready = 0;

void SystemClock_Config(void) {
    // Simplified clock configuration (assuming 180 MHz from HSE as in previous example)
    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY));
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    PWR->CR |= PWR_CR_VOS;
    FLASH->ACR |= FLASH_ACR_LATENCY_5WS | FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN;
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1 | RCC_CFGR_PPRE1_DIV4 | RCC_CFGR_PPRE2_DIV2;
    RCC->PLLCFGR = (4 << 0) | (180 << 6) | (0 << 16) | RCC_PLLCFGR_PLLSRC_HSE;
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY));
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
}

void GPIO_Config(void) {
    // Enable GPIOC clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;

    // Configure PC6 (TX) and PC7 (RX) as AF8 for USART6
    GPIOC->MODER &= ~((3U << 12) | (3U << 14));  // Clear mode bits
    GPIOC->MODER |= (2U << 12) | (2U << 14);     // Set to AF mode
    GPIOC->AFR[0] &= ~((0xFU << 24) | (0xFU << 28));  // Clear AFR bits for PC6, PC7
    GPIOC->AFR[0] |= (8U << 24) | (8U << 28);     // Set AF8 (0x08)

    // Set high speed
    GPIOC->OSPEEDR |= (3U << 12) | (3U << 14);
}

void USART6_Config(void) {
    // Enable USART6 and GPIOC clocks
    RCC->APB2ENR |= RCC_APB2ENR_USART6EN;

    // Configure baud rate: 1 Mbps with 180 MHz APB2 clock
    // USARTDIV = 180,000,000 / (16 * 1,000,000) = 11.25, use 11
    USART6->BRR = 11;

    // Enable TX, RX, RX interrupt, and USART
    USART6->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE | USART_CR1_UE;

    // Enable NVIC for USART6
    NVIC_EnableIRQ(USART6_IRQn);
    __enable_irq();
}

void USART6_IRQHandler(void) {
    if (USART6->SR & USART_SR_RXNE) {
        uint8_t received_byte = USART6->DR;

        if (received_byte == 0x5C && buffer_index == 0) {
            // Start of data triggered by 0x5C
            buffer_index = 0;
        } else if (buffer_index < BUFFER_SIZE) {
            receive_buffer[buffer_index++] = received_byte;
            if (buffer_index == BUFFER_SIZE) {
                data_ready = 1;  // Data collection complete
            }
        }
    }
}

void Send_Data(void) {
    for (uint32_t i = 0; i < BUFFER_SIZE; i++) {
        while (!(USART6->SR & USART_SR_TXE));
        USART6->DR = receive_buffer[i];
    }
}

int main(void) {
    SystemClock_Config();
    GPIO_Config();
    USART6_Config();

    while (1) {
        if (data_ready) {
            Send_Data();  // Transmit 3 KB data to ground
            data_ready = 0;
            buffer_index = 0;  // Reset for next transmission
        }
    }
}