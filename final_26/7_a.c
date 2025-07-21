void SPI_Master_Init(void) {
    // Enable SPI1 clock (example: APB2)
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    // Configure SPI1_CR1 (at 0x00)
    SPI1->CR1 = 0;
    SPI1->CR1 |= (1 << 2);   // MSTR = 1 (Master)
    SPI1->CR1 |= (1 << 6);   // SPE = 1 (Enable SPI)
    SPI1->CR1 |= (0 << 1);   // CPOL = 0
    SPI1->CR1 |= (0 << 0);   // CPHA = 0
    SPI1->CR1 |= (0b000 << 3); // BR[2:0] = 000 (f_PCLK/2)

    // Enable SSOE if using NSS hardware
    SPI1->CR2 |= (1 << 2);   // SSOE = 1
}

void SPI_Slave_Init(void) {
    // Enable SPI2 clock (example: APB1)
    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;

    // Configure SPI2_CR1
    SPI2->CR1 = 0;
    SPI2->CR1 &= ~(1 << 2);  // MSTR = 0 (Slave)
    SPI2->CR1 |= (1 << 6);   // SPE = 1 (Enable SPI)
    SPI2->CR1 |= (0 << 1);   // CPOL = 0
    SPI2->CR1 |= (0 << 0);   // CPHA = 0
}
