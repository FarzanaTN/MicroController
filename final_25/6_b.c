#include "stm32f4xx.h"

#define BUFFER_SIZE 3072  // 3 KB
uint8_t receive_buffer[BUFFER_SIZE];
volatile uint32_t buffer_index = 0;
volatile uint8_t data_ready = 0;

void SystemClock_Config(void) {
    RCC->CR |= RCC_CR_HSEON;                // Enable HSE (8 MHz)
    while (!(RCC->CR & RCC_CR_HSERDY));
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    PWR->CR |= PWR_CR_VOS;
    FLASH->ACR |= FLASH_ACR_LATENCY_5WS | FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN;
    RCC->PLLCFGR = (8 << 0) | (200 << 6) | (0 << 16) | RCC_PLLCFGR_PLLSRC_HSE;
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY));
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1 | RCC_CFGR_PPRE1_DIV2 | RCC_CFGR_PPRE2_DIV1;
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
}

void GPIO_Config(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;    // Enable GPIOC clock
    GPIOC->MODER &= ~((3U << 12) | (3U << 14));  // Clear PC6, PC7 mode
    GPIOC->MODER |= (2U << 12) | (2U << 14);     // Set to AF mode
    GPIOC->AFR[0] &= ~((0xFU << 24) | (0xFU << 28));  // Clear AFR for PC6, PC7
    GPIOC->AFR[0] |= (8U << 24) | (8U << 28);    // Set AF8 (0x08)
    GPIOC->OSPEEDR |= (3U << 12) | (3U << 14);   // High speed
}

void USART6_Config(void) {
    RCC->APB2ENR |= RCC_APB2ENR_USART6EN;    // Enable USART6 clock (bit 5)
    USART6->BRR = 320;                       // Baud rate 5 Mbps (approximate with 100 MHz PCLK)
    USART6->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE | USART_CR1_UE;
    NVIC_EnableIRQ(USART6_IRQn);
    __enable_irq();
}

void USART6_IRQHandler(void) {
    if (USART6->SR & USART_SR_RXNE) {
        uint8_t received_byte = USART6->DR;
        if (received_byte == 0x5C && buffer_index == 0) {
            buffer_index = 0;                  // Start of data
        } else if (buffer_index < BUFFER_SIZE) {
            receive_buffer[buffer_index++] = received_byte;
            if (buffer_index == BUFFER_SIZE) {
                data_ready = 1;                // Data collection complete
            }
        }
    }
}

void Send_Trigger(void) {
    while (!(USART6->SR & USART_SR_TXE));
    USART6->DR = 0x5C;                        // Send trigger byte
    while (!(USART6->SR & USART_SR_TXE));      // Wait for transmission
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
    Send_Trigger();                            // Initiate data transfer

    while (1) {
        if (data_ready) {
            Send_Data();                       // Transmit 3 KB to ground
            data_ready = 0;
            buffer_index = 0;
            Send_Trigger();                    // Send trigger again for next cycle
        }
    }
}