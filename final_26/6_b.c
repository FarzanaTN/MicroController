void I2C_Write_Unit(uint8_t reg, uint8_t data) {
    I2C1->CR1 |= I2C_CR1_START; while (!(I2C1->SR1 & I2C_SR1_SB));
    I2C1->DR = 0x82; while (!(I2C1->SR1 & I2C_SR1_ADDR)); (void)I2C1->SR2;
    I2C1->DR = reg; while (!(I2C1->SR1 & I2C_SR1_TXE));
    I2C1->DR = data; while (!(I2C1->SR1 & I2C_SR1_TXE));
    I2C1->CR1 |= I2C_CR1_STOP; while (I2C1->SR1 & I2C_SR1_STOPF);
}

void I2C_Set_Units(void) {
    I2C_Write_Unit(0x0AH, 0xA0); // Set pressure to Kp
    I2C_Write_Unit(0x0BH, 0x0FH); // Set temperature to F
}

uint8_t I2C_Read_Register(uint8_t reg) {
    I2C1->CR1 |= I2C_CR1_START; while (!(I2C1->SR1 & I2C_SR1_SB));
    I2C1->DR = 0x82; while (!(I2C1->SR1 & I2C_SR1_ADDR)); (void)I2C1->SR2;
    I2C1->DR = reg; while (!(I2C1->SR1 & I2C_SR1_TXE));
    I2C1->CR1 |= I2C_CR1_START; while (!(I2C1->SR1 & I2C_SR1_SB));
    I2C1->DR = 0x83; while (!(I2C1->SR1 & I2C_SR1_ADDR)); (void)I2C1->SR2;
    I2C1->CR1 &= ~I2C_CR1_ACK; I2C1->CR1 |= I2C_CR1_STOP;
    while (!(I2C1->SR1 & I2C_SR1_RXNE));
    uint8_t data = I2C1->DR;
    return data;
}

void I2C_Read_Values(uint8_t *temp, uint8_t *press) {
    *temp = I2C_Read_Register(0x0CH);
    *press = I2C_Read_Register(0x0DH);
}