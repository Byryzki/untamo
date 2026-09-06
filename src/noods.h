#ifndef NOODS_H
#define NOODS_H

#include "pico/stdlib.h"

#define NOODS_PIN 14

void init_nood();
void put_nood(bool on);
void blink_nood(int times);
void wakeup_nood();
void stop_wakeup();

#endif