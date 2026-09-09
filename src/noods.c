#include "noods.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include "pico/time.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"

static int fade = 0;
static bool going_up = true;
static int blink_iter = 0;

int init_nood()
{
    gpio_init(NOODS_PIN);
    gpio_set_dir(NOODS_PIN, GPIO_OUT);
    
    return 0;
}

void put_nood(bool on)
{
    gpio_put(NOODS_PIN, on);
}

void on_blink()
{
    // Clear the interrupt flag that brought us here
    pwm_clear_irq(pwm_gpio_to_slice_num(NOODS_PIN));

    if (going_up)
    {
        ++fade;
        if (fade > 255)
        {
            fade = 255;
            going_up = false;
        }
    } else {
        --fade;
        if (fade < 0)
        {
            fade = 0;
            going_up = true;
        }
    }
    // Square the fade value to make the LED's brightness appear more linear
    // Note this range matches with the wrap value
    pwm_set_gpio_level(NOODS_PIN, fade * fade);
}

void on_wakeup()
{
    // Clear the interrupt flag that brought us here
    pwm_clear_irq(pwm_gpio_to_slice_num(NOODS_PIN));

    if (going_up)
    {
        ++fade;
        if (fade > 255)
        {
            fade = 255;
            going_up = false;
        }
    }
    // Square the fade value to make the LED's brightness appear more linear
    // Note this range matches with the wrap value
    pwm_set_gpio_level(NOODS_PIN, fade * fade);
}

void set_nood(uint8_t action)   // 0: Fade on, 1: Smooth blink
{
    fade = 0;
    going_up = true;
    
    gpio_set_function(NOODS_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(NOODS_PIN);

    pwm_clear_irq(slice_num);
    pwm_set_irq_enabled(slice_num, true);

    pwm_config config = pwm_get_default_config();

    switch(action)
    {
        case 0:
            irq_set_exclusive_handler(PWM_DEFAULT_IRQ_NUM(), on_wakeup);
            pwm_config_set_clkdiv(&config, 50.f);
            break;

        case 1:
            irq_set_exclusive_handler(PWM_DEFAULT_IRQ_NUM(), on_blink);
            pwm_config_set_clkdiv(&config, 5.f);
            break;
    }
    
    irq_set_enabled(PWM_DEFAULT_IRQ_NUM(), true);

    pwm_init(slice_num, &config, true);
}

void stop_nood()
{
    // TODO: Figure out how to get PWM back after the first cycle
    blink_iter = 0;
    uint slice_num = pwm_gpio_to_slice_num(NOODS_PIN);

    pwm_set_irq_enabled(slice_num, false);
    irq_set_enabled(PWM_DEFAULT_IRQ_NUM(), false);
    pwm_clear_irq(slice_num); // Clear any leftover pending flags

    pwm_set_enabled(slice_num, false);

    gpio_set_function(NOODS_PIN, GPIO_FUNC_SIO);
    gpio_set_dir(NOODS_PIN, GPIO_OUT);
    gpio_put(NOODS_PIN, 0);
}