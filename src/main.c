#include <stdio.h>
#include "pico/stdlib.h"
#include "noods.h"
#include "dfrobot_max30102.h"
#include "hardware/i2c.h"
#include "oled.h"

#define I2C_PORT i2c0
#define I2C_SDA_PIN 4
#define I2C_SCL_PIN 5

typedef enum
{
    IDLE,
    SET,
    MEASURE,
    WAKE
} State;

typedef struct
{
    volatile State state;
    volatile time sleeptime;
    volatile bool diplay_on;
} Status;

static Status run =
{
    .state = IDLE,  // Init state here
    .sleeptime = 0,
};

void gpio_callback(uint gpio, uint32_t events)
{
    run.diplay_on = true;
    run.state = SET;
}

void status_log()
{
    printf("State is: %d\n", run.state);
    printf("Wakeup time is: %d\n", run.sleeptime.hour);
}

int main() {
    // Initialize standard I/O for serial printing
    stdio_init_all(); 
    init_display();
    init_nood();

    int sleep_mins = 0;

    // Wake display from either button
    gpio_set_irq_enabled_with_callback(key0, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_set_irq_enabled_with_callback(key1, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    /*Scheduling - Must not break!*/
    while(true)
    {
        switch(run.state)
        {
            case IDLE:
                //status_log();
                sleep_ms(100);
                break;

            case SET:
                status_log();
                run.sleeptime = set_time();
                gpio_acknowledge_irq(key0, GPIO_IRQ_EDGE_FALL);
                gpio_acknowledge_irq(key1, GPIO_IRQ_EDGE_FALL);
                run.diplay_on = false;
                run.state = MEASURE;
                status_log();
                break;

            case MEASURE:
                status_log();
                sleep_mins = (((run.sleeptime.hour*60) + run.sleeptime.minutes*10) *1000);
                printf("Sleep duration shall be: %d\n", sleep_mins);
                sleep_ms(sleep_mins);

                run.state = WAKE;
                break;

            case WAKE:
                status_log();
                wakeup_nood();
                sleep_ms(15000);
                put_nood(false);
                run.state = IDLE;
                break;
        }
        sleep_ms(1000);
    }

    return 0;
}

