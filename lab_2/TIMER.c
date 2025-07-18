#include "TIMER.h"
#include "stm32f4xx.h"


void TIM2_Init(void) {
    // Enable TIM2 clock
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    
    // Set prescaler for 1MHz clock (84MHz/84 = 1MHz)
    TIM2->PSC = 84 - 1;
    
    // Auto-reload value (maximum)
    TIM2->ARR = 0xFFFFFFFF;
    
    // Clear counter
    TIM2->CNT = 0;
    
    // Enable timer
    TIM2->CR1 |= TIM_CR1_CEN;
}

void Delay(uint32_t ms) {
    uint32_t start = TIM2->CNT;
    // Wait for the specified number of milliseconds
    while ((TIM2->CNT - start) < (ms * 1000));
}