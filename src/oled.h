#ifndef OLED_H
#define OLED_H

#include "OLED_1in3_c.h"
#include "GUI_Paint.h"

#define key0 15 
#define key1 17

typedef struct
{
    volatile int sleephour;
    volatile int sleepminutes;
    volatile int wakehour;
    volatile int wakeminutes;
    volatile int durhour;
    volatile int durminutes;
    int total;
} time;

int init_display(void);
int set_time(void);

static const char *intro_str = "UNTAMO";
static const char del_char = ':';

#endif