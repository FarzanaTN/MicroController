void SPI_Send_Config(uint16_t address, uint32_t data) {
    // Ensure address is in configuration range (0xA000 to 0xA00F)
    uint8_t addr = (address & 0xFF) | 0x80; // MSB=1 for WRITE

    while (!(SPI1->SR & SPI_SR_TXE));  // Wait TX ready
    SPI1->DR = addr;
    while (!(SPI1->SR & SPI_SR_RXNE)); (void)SPI1->DR;

    // Send 4 bytes (MSB first)
    for (int i = 3; i >= 0; i--) {
        while (!(SPI1->SR & SPI_SR_TXE));
        SPI1->DR = (data >> (8 * i)) & 0xFF;
        while (!(SPI1->SR & SPI_SR_RXNE)); (void)SPI1->DR;
    }
}


uint32_t SPI_Read_Monitor(uint16_t address) {
    uint8_t addr = (address & 0xFF) & 0x7F; // MSB=0 for READ
    uint32_t result = 0;

    while (!(SPI1->SR & SPI_SR_TXE));
    SPI1->DR = addr;
    while (!(SPI1->SR & SPI_SR_RXNE)); (void)SPI1->DR;

    // Read 4 bytes from slave (send dummy 0x00 to receive)
    for (int i = 3; i >= 0; i--) {
        while (!(SPI1->SR & SPI_SR_TXE));
        SPI1->DR = 0x00;  // Dummy byte
        while (!(SPI1->SR & SPI_SR_RXNE));
        result |= ((uint32_t)(SPI1->DR) << (8 * i));
    }

    return result;
}
