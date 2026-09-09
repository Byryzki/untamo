#ifndef BASIC_READING
#define BASIC_READING

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "dfrobot_max30102.h"

#define POWER_PIN 3
#define I2C_PORT i2c0
#define I2C_SDA_PIN 0
#define I2C_SCL_PIN 1

int init_pulse();
int get_pulse();

#endif