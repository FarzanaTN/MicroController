#include <cstdint>
#define RCC_AHB1ENR   (*(volatile unsigned int*)0x40023830)
#define RCC_APB2ENR   (*(volatile unsigned int*)0x40023844)
#define GPIOC_MODER   (*(volatile unsigned int*)0x40020800)
#define GPIOC_AFRL    (*(volatile unsigned int*)0x40020820)
#define USART6_SR     (*(volatile unsigned int*)0x40011400)
#define USART6_DR     (*(volatile unsigned int*)0x40011404)
#define USART6_BRR    (*(volatile unsigned int*)0x40011408)
#define USART6_CR1    (*(volatile unsigned int*)0x4001140C)
#define USART6_CR2    (*(volatile unsigned int*)0x40011410)
#define USART6_CR3    (*(volatile unsigned int*)0x40011414)

void uart6_init(void) {
    // 1. Enable clocks
    RCC_AHB1ENR |= (1 << 2);    // Enable GPIOC
    RCC_APB2ENR |= (1 << 5);    // Enable USART6

    // 2. Configure PC6 and PC7 to Alternate Function mode (MODER = 10)
    GPIOC_MODER &= ~((3 << (6 * 2)) | (3 << (7 * 2))); // Clear bits
    GPIOC_MODER |= ((2 << (6 * 2)) | (2 << (7 * 2)));  // Set AF mode

    // 3. Set Alternate function to AF8 (USART6)
    GPIOC_AFRL &= ~((0xF << (6 * 4)) | (0xF << (7 * 4))); // Clear
    GPIOC_AFRL |= ((8 << (6 * 4)) | (8 << (7 * 4)));      // Set AF8

    // 4. Configure baud rate = 1 Mbps with fclk = 54 MHz → BRR = 0x36
    USART6_BRR = 0x36;

    // 5. Enable USART6: TE, RE, UE
    USART6_CR1 |= (1 << 3) | (1 << 2) | (1 << 13);  // TE, RE, UE
}

void uart6_send_byte(uint8_t data) {
    while (!(USART6_SR & (1 << 7))); // Wait for TXE
    USART6_DR = data;
}

uint8_t uart6_receive_byte(void) {
    while (!(USART6_SR & (1 << 5))); // Wait for RXNE
    return (uint8_t)USART6_DR;
}

void send_3KB_data() {
    for (int i = 0; i < 3072; ++i) {
        uart6_send_byte(0xAB); // Example data byte
    }
}

int main(void) {
    uart6_init();

    while (1) {
        uint8_t received = uart6_receive_byte();
        if (received == 0x5C) {
            send_3KB_data();
        }
    }
}
