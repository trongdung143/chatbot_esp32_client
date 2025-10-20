#pragma once
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>
#include "config.h"

extern Adafruit_GC9A01A tft;

void display_init();
void display_clear();
void display_show_text_in_rect(const String &text, int16_t x, int16_t y, uint16_t color, uint16_t bg, uint16_t border_color);
void display_clear_rect(int16_t x, int16_t y, int16_t w, int16_t h);
void display_update_line(uint8_t line, const String &text, uint16_t text_color, uint16_t bg_color, uint16_t border_color);
void display_update_line_centered(uint8_t line, const String &text, uint16_t text_color, uint16_t bg_color, uint16_t border_color);