#include "stm32f4xx.h"  // or the relevant header

#define I2C_WRITE_ADDR 0x82  // 0x41 << 1
#define I2C_READ_ADDR  0x83  // 0x41 << 1 | 1

void I2C_Write_Unit(uint8_t reg, uint8_t data) {
    I2C1->CR1 |= I2C_CR1_START;
    while (!(I2C1->SR1 & I2C_SR1_SB));

    I2C1->DR = I2C_WRITE_ADDR;
    while (!(I2C1->SR1 & I2C_SR1_ADDR));
    (void)I2C1->SR2;

    while (!(I2C1->SR1 & I2C_SR1_TXE));
    I2C1->DR = reg;
    while (!(I2C1->SR1 & I2C_SR1_TXE));
    I2C1->DR = data;
    while (!(I2C1->SR1 & I2C_SR1_TXE));

    I2C1->CR1 |= I2C_CR1_STOP;
}

void I2C_Set_Units(void) {
    I2C_Write_Unit(0x0A, 0x0F);  // Set temperature to Fahrenheit
    I2C_Write_Unit(0x0B, 0x01);  // Set pressure to Kilopascal (Kp)
}

uint8_t I2C_Read_Register(uint8_t reg) {
    // Write register address first
    I2C1->CR1 |= I2C_CR1_START;
    while (!(I2C1->SR1 & I2C_SR1_SB));

    I2C1->DR = I2C_WRITE_ADDR;
    while (!(I2C1->SR1 & I2C_SR1_ADDR));
    (void)I2C1->SR2;

    while (!(I2C1->SR1 & I2C_SR1_TXE));
    I2C1->DR = reg;
    while (!(I2C1->SR1 & I2C_SR1_TXE));

    // Repeated start for read
    I2C1->CR1 |= I2C_CR1_START;
    while (!(I2C1->SR1 & I2C_SR1_SB));

    I2C1->DR = I2C_READ_ADDR;
    while (!(I2C1->SR1 & I2C_SR1_ADDR));
    (void)I2C1->SR2;

    I2C1->CR1 &= ~I2C_CR1_ACK;  // NACK after 1 byte
    I2C1->CR1 |= I2C_CR1_STOP;

    while (!(I2C1->SR1 & I2C_SR1_RXNE));
    return I2C1->DR;
}

void I2C_Read_Values(uint8_t *temp, uint8_t *press) {
    *temp = I2C_Read_Register(0x0C);  // Read temperature from register 3
    *press = I2C_Read_Register(0x0D); // Read pressure from register 4
}
