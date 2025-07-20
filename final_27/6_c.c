void gpio_init() {
    RCC_AHB1ENR |= (1 << 2);   // Enable GPIOC
    RCC_AHB1ENR |= (1 << 0);   // Enable GPIOA

    // Configure PC1–PC4 as output
    for (int i = 1; i <= 4; i++) {
        GPIOC_MODER &= ~(3 << (i * 2));
        GPIOC_MODER |= (1 << (i * 2));  // Output mode
    }

    // Configure PA7 and PA9 as input (00)
    GPIOA_MODER &= ~((3 << (7 * 2)) | (3 << (9 * 2)));
}

void set_leds(uint8_t sw) {
    // Bit 0 = PA7, Bit 1 = PA9
    switch (sw) {
        case 0b00:
            GPIOC_ODR |= (0x1E);  // PC1-PC4 ON
            break;
        case 0b01:
            GPIOC_ODR |= (0x1E);
            GPIOC_ODR &= ~(1 << 1); // PC1 OFF
            break;
        case 0b10:
            GPIOC_ODR |= (0x1E);
            GPIOC_ODR &= ~(1 << 2); // PC2 OFF
            break;
        case 0b11:
            GPIOC_ODR &= ~(0x1E);  // All OFF
            break;
    }
}

void main_loop() {
    while (1) {
        uint8_t sw = 0;
        sw |= ((GPIOA_IDR >> 7) & 0x01);       // PA7 → bit 0
        sw |= ((GPIOA_IDR >> 9) & 0x01) << 1;  // PA9 → bit 1

        set_leds(sw);
    }
}
