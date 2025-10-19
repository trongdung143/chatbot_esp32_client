#pragma once
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>
#include "config.h"

extern Adafruit_GC9A01A tft;

void display_init();
void display_clear();
void display_show_text(const String &text, int x = 20, int y = 100,
                       uint16_t color = GC9A01A_WHITE, uint16_t bg = GC9A01A_BLACK);
