void i2c_reset_bus(void) {
    // Configure SDA and SCL as GPIO output
    set_sda_as_gpio_output();
    set_scl_as_gpio_output();

    // Toggle SCL 9 times (to release stuck slave)
    for (int i = 0; i < 9; i++) {
        set_scl_high();
        delay_short();
        set_scl_low();
        delay_short();
    }

    // Generate STOP condition manually
    set_sda_low();
    delay_short();
    set_scl_high();
    delay_short();
    set_sda_high();  // SDA goes HIGH while SCL is HIGH = STOP
}
