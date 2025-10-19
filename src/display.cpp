#include "display.h"

Adafruit_GC9A01A tft(TFT_CS, TFT_DC, TFT_SDA, TFT_SCL, TFT_RST);

void display_show_text(const String &text, int x, int y, uint16_t color, uint16_t bg)
{
    int16_t w = text.length() * 12;
    int16_t h = 20;
    tft.fillRect(x - 2, y - 2, w + 4, h + 4, bg);
    tft.setCursor(x, y + 5);
    tft.setTextColor(color, bg);
    tft.setTextSize(2);
    tft.println(text);
}

void display_init()
{
    tft.begin();
    tft.setRotation(0);
    tft.fillScreen(GC9A01A_BLACK);
    display_show_text("ChatBot-Esp Ready", 10, 100, GC9A01A_CYAN, GC9A01A_BLACK);
}

void display_clear()
{
    tft.fillScreen(GC9A01A_BLACK);
}
