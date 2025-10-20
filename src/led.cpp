#include "led.h"

Adafruit_NeoPixel led(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void led_init()
{
    led.begin();
    led.show();
    led.setPixelColor(0, led.Color(0, 0, 255));
    led.show();
}

void led_set_color(uint8_t r, uint8_t g, uint8_t b)
{

    led.setPixelColor(0, led.Color(r, g, b));
    led.show();
}

void led_clear()
{
    led.clear();
    led.show();
}