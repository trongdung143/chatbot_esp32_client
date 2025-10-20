#pragma once
#include "config.h"
#include <Adafruit_NeoPixel.h>

extern Adafruit_NeoPixel led;

void led_init();
void led_set_color(uint8_t r, uint8_t g, uint8_t b);
void led_clear();