//grok
void GPIOA_Config(void) {
    // Enable GPIOA clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    // Configure PIN 0-3 as inputs (MODER bits 0-6)
    GPIOA->MODER &= ~0x000000FF;  // Clear bits 0-7
    // Configure PIN 4-7 as outputs (MODER bits 8-15)
    GPIOA->MODER |= 0x0000FF00;   // Set bits 8-15 to 01

    // Configure PIN 4-7 as push-pull (OTYPER bits 4-7)
    GPIOA->OTYPER &= ~0x000000F0;  // Clear bits 4-7

    // Configure PIN 4-7 as high speed (OSPEEDR bits 8-15)
    GPIOA->OSPEEDR |= 0x0000FF00;  // Set bits 8-15 to 11

    // No pull-up/pull-down for all pins (PUPDR bits 0-15)
    GPIOA->PUPDR &= ~0x0000FFFF;   // Clear bits 0-15
}



//gpt
void GPIOA_Config(void) {
    // 1. Enable clock for GPIOA
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    // 2. Set PA0 - PA3 as input (MODER = 00)
    GPIOA->MODER &= ~(3 << (0 * 2)); // PA0
    GPIOA->MODER &= ~(3 << (1 * 2)); // PA1
    GPIOA->MODER &= ~(3 << (2 * 2)); // PA2
    GPIOA->MODER &= ~(3 << (3 * 2)); // PA3

    // 3. Set PA4 - PA7 as output (MODER = 01)
    GPIOA->MODER &= ~(3 << (4 * 2)); // clear first
    GPIOA->MODER |=  (1 << (4 * 2)); // PA4 as output

    GPIOA->MODER &= ~(3 << (5 * 2));
    GPIOA->MODER |=  (1 << (5 * 2)); // PA5

    GPIOA->MODER &= ~(3 << (6 * 2));
    GPIOA->MODER |=  (1 << (6 * 2)); // PA6

    GPIOA->MODER &= ~(3 << (7 * 2));
    GPIOA->MODER |=  (1 << (7 * 2)); // PA7

    // 4. Set PA4 - PA7 as push-pull output (OTYPER = 0)
    GPIOA->OTYPER &= ~(1 << 4);  // PA4
    GPIOA->OTYPER &= ~(1 << 5);  // PA5
    GPIOA->OTYPER &= ~(1 << 6);  // PA6
    GPIOA->OTYPER &= ~(1 << 7);  // PA7

    // 5. Set PA4 - PA7 to high speed (OSPEEDR = 11)
    GPIOA->OSPEEDR |= (3 << (4 * 2)); // PA4
    GPIOA->OSPEEDR |= (3 << (5 * 2)); // PA5
    GPIOA->OSPEEDR |= (3 << (6 * 2)); // PA6
    GPIOA->OSPEEDR |= (3 << (7 * 2)); // PA7

    // 6. Disable pull-up/pull-down for PA0 - PA7 (PUPDR = 00)
    for (int i = 0; i <= 7; i++) {
        GPIOA->PUPDR &= ~(3 << (i * 2)); // Clear 2 bits for each pin
    }
}
