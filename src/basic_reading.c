#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "dfrobot_max30102.h"
#include "basic_reading.h"

int init_pulse()
{   
    gpio_init(POWER_PIN);
    gpio_set_dir(POWER_PIN, GPIO_OUT);
    gpio_put(POWER_PIN, 1);
    // Give the serial monitor a moment to connect
    sleep_ms(2000);
    printf("Starting DFRobot MAX30102 I2C Test...\n");

    // Initialize I2C at 100 kHz
    i2c_init(I2C_PORT, 100 * 1000);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    
    // Enable internal pull-ups (the module might have them, but it's a good fallback)
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);

    // Initialize the MAX30102 Module
    if (!dfrobot_max30102_init(I2C_PORT)) {
        printf("Failed to find MAX30102 at I2C address 0x%02X.\n", MAX30102_I2C_ADDR);
        printf("Please check your wiring and pull-ups.\n");
        return 1;
    }
    
    printf("MAX30102 initialized successfully!\n");
    
    // Command the module's MCU to start calculating PPG data
    dfrobot_max30102_start_collect(I2C_PORT);
    printf("Sensor collection started. Waiting for first reading...\n");

    return 0;
}

int get_pulse()
{
    max30102_data_t sensor_data;

    // Fetch SPO2 and Heart Rate
    dfrobot_max30102_get_data(I2C_PORT, &sensor_data);
    
    // Fetch Board Temperature
    float temp_c = dfrobot_max30102_get_temperature(I2C_PORT);

    // Print the results
    printf("------------------------------------\n");
    
    if (sensor_data.spo2 == -1) {
        printf("SPO2: Invalid/Measuring...\n");
    } else {
        printf("SPO2: %d %%\n", sensor_data.spo2);
    }

    if (sensor_data.heartbeat == -1) {
        printf("Heart Rate: Invalid/Measuring...\n");
    } else {
        printf("Heart Rate: %ld bpm\n", sensor_data.heartbeat);
    }
    
    printf("Board Temp: %.2f C\n", temp_c);

    return 0;
}