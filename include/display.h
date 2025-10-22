#pragma once
#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>
#include "config.h"

extern Adafruit_GC9A01A tft;

void display_init();
void display_clear();

void display_update_line_centered(uint8_t line, const String &text, uint16_t text_color, uint16_t bg_color, uint16_t border_color);

void diplay_update_state(State state);