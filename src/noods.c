#include "noods.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include "pico/time.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"

void init_nood()
{
    gpio_init(NOODS_PIN);
    gpio_set_dir(NOODS_PIN, GPIO_OUT);
}

void put_nood(bool on)
{
    gpio_put(NOODS_PIN, on);
}

void blink_nood(int times)
{
    for(int i=0; i<times; i++)
    {
        gpio_put(NOODS_PIN, 1);
        sleep_ms(200);
        gpio_put(NOODS_PIN, 0);
        sleep_ms(200);
    }
}

void on_pwm_wrap() {
    static int fade = 0;
    static bool going_up = true;
    // Clear the interrupt flag that brought us here
    pwm_clear_irq(pwm_gpio_to_slice_num(NOODS_PIN));

    if (going_up) {
        ++fade;
        if (fade > 255) {
            fade = 255;
            going_up = false;
        }
    } else {
        /*--fade;
        if (fade < 0) {
            fade = 0;
            going_up = true;
        }*/
        stop_wakeup();
    }
    // Square the fade value to make the LED's brightness appear more linear
    // Note this range matches with the wrap value
    pwm_set_gpio_level(NOODS_PIN, fade * fade);
}

/*make the PWM kind*/
void wakeup_nood()
{
    // Tell the LED pin that the PWM is in charge of its value.
    gpio_set_function(NOODS_PIN, GPIO_FUNC_PWM);
    // Figure out which slice we just connected to the LED pin
    uint slice_num = pwm_gpio_to_slice_num(NOODS_PIN);

    // Mask our slice's IRQ output into the PWM block's single interrupt line,
    // and register our interrupt handler
    pwm_clear_irq(slice_num);
    pwm_set_irq_enabled(slice_num, true);
    irq_set_exclusive_handler(PWM_DEFAULT_IRQ_NUM(), on_pwm_wrap);
    irq_set_enabled(PWM_DEFAULT_IRQ_NUM(), true);

    // Get some sensible defaults for the slice configuration. By default, the
    // counter is allowed to wrap over its maximum range (0 to 2**16-1)
    pwm_config config = pwm_get_default_config();
    // Set divider, reduces counter clock to sysclock/this value
    pwm_config_set_clkdiv(&config, 50.f);
    // Load the configuration into our PWM slice, and set it running.
    pwm_init(slice_num, &config, true);
}

void stop_wakeup()
{
    // TODO: Figure out how to get PWM back after the first cycle
    uint slice_num = pwm_gpio_to_slice_num(NOODS_PIN);

    pwm_set_irq_enabled(slice_num, false);
    irq_set_enabled(PWM_DEFAULT_IRQ_NUM(), false);
    pwm_clear_irq(slice_num); // Clear any leftover pending flags

    pwm_set_enabled(slice_num, false);

    gpio_set_function(NOODS_PIN, GPIO_FUNC_SIO);
    gpio_set_dir(NOODS_PIN, GPIO_OUT);
    gpio_put(NOODS_PIN, 1); // Keep nood on
}