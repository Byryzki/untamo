#include <stdio.h>
#include "pico/stdlib.h"
#include "noods.h"
#include "basic_reading.h"
#include "hardware/i2c.h"
#include "oled.h"

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
    volatile bool wakeup_on;
} Status;

static Status run =
{
    .state = IDLE,  // Init state here
    .sleeptime = 0,
};

void gpio_callback(uint gpio, uint32_t events)
{
    if(run.wakeup_on)
    {
        run.state = IDLE;
    } else {
        run.state = SET;
    }
}
/*Wakeful sleep breaking out of state when new detected.*/
int dog_sleep(int secs)
{
    volatile State prev_state = run.state;
    for(int i=0; i<secs*10; i++)
    {
        sleep_ms(100);
        if(run.state != prev_state){return 1;}
    }
    return 0;
}

void status_log()
{
    printf("State is: %d\n", run.state);
    printf("Wakeup time is: %d\n", run.sleeptime.hour);
}

void present_error(int subject)
/*0: stdio, 1: display, 2: nood, 3: pulse*/
{
    char* subs[4] = {"stdio", "display", "nood", "pulse"};
    while(true)
    {
        printf("Problems with initializing %s\n", subs[subject]);
        sleep_ms(2000);
    }
}

int main() {
    // Initialize standard I/O for serial printing
    stdio_init_all();
    sleep_ms(5000); //Time to get serial going

    if(init_display()){present_error(1);} 
    if(init_nood()){present_error(2);} 
    //if(init_pulse()){present_error(3);} // TODO: Find why init hangs

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
                dog_sleep(60);
                break;

            case SET:
                status_log();
                run.sleeptime = set_time();
                gpio_acknowledge_irq(key0, GPIO_IRQ_EDGE_FALL);
                gpio_acknowledge_irq(key1, GPIO_IRQ_EDGE_FALL);
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
                run.wakeup_on = true;

                set_nood(0);
                if(dog_sleep(30))
                {
                    stop_nood();
                    run.wakeup_on = false;
                    break;
                }
                //stop_nood();
                // Blinks in the end to make sure person woke
                //set_nood(1);
                //sleep_ms(5000);
                stop_nood();

                run.wakeup_on = false;
                run.state = IDLE;
                break;
        }
    }

    return 0;
}

