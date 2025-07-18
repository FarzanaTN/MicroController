void GPIO_Config_Pins(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;  // Enable GPIOA clock

    // Configure Pin-X (e.g., PA0) as input, speed 160 MHz
    GPIOA->MODER &= ~(3U << 0);           // Input mode
    GPIOA->OSPEEDR |= (3U << 0);          // Very high speed (160 MHz)

    // Configure Pin-Y (e.g., PA1) as input, speed 50 MHz
    GPIOA->MODER &= ~(3U << 2);           // Input mode
    GPIOA->OSPEEDR |= (2U << 2);          // High speed (50 MHz)
}

/*
Steps to Configure GPIO:

    Enable GPIO Clock:
        Set the appropriate bit in RCC_AHB1ENR (e.g., RCC_AHB1ENR_GPIOAEN for GPIOA).
    Set Mode to Input:
        Clear the two bits for the pin in MODER (set to 00).
    Configure Speed:
        For Pin-X: Set the two bits in OSPEEDR to 11 (very high speed, up to 160 MHz for STM32F4 with proper clock).
        For Pin-Y: Set the two bits in OSPEEDR to 10 (high speed, approximately 50 MHz).
    Configure Pull-Up/Pull-Down (Optional):
        Set PUPDR to 01 (pull-up) or 10 (pull-down) if needed for the input.

    Best Pin for Real-Time Sensor Event:

    Pin-X (160 MHz) will result best because a higher speed allows faster signal detection and response, critical for real-time sensor events (e.g., detecting rapid changes in traffic density or sensor data). The 160 MHz speed ensures minimal latency in reading the input state.
    
*/