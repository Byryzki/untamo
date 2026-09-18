#include "pulse.h"
#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

int init_pulse()
{
    adc_init();
    adc_gpio_init(PULSE_PIN);
    adc_select_input(2);
    
    return 0;
}

int get_pulse()
{
    while(true)
    {
        const float conversion_factor = 3.3f / (1 << 12);
        uint16_t result = adc_read();
        printf("Raw value: 0x%03x, voltage: %f V\n", result, result * conversion_factor);
        sleep_ms(500);
    }
    
    return 0;
}