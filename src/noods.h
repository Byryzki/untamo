#ifndef NOODS_H
#define NOODS_H

#include "pico/stdlib.h"

#define NOODS_PIN 14

int init_nood();
void put_nood(bool on);
void blink_nood(int times);
void set_nood(uint8_t action);
void stop_nood();

#endif