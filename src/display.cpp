#include "display.h"

Adafruit_GC9A01A tft(TFT_CS, TFT_DC, TFT_SDA, TFT_SCL, TFT_RST);

void display_init()
{
    tft.begin();
    tft.setRotation(0);
    tft.fillScreen(GC9A01A_BLACK);
    display_update_line_centered(3, "chatbot-luu", GC9A01A_ORANGE, GC9A01A_BLACK, GC9A01A_WHITE);
}

void display_clear()
{
    tft.fillScreen(GC9A01A_BLACK);
}

void display_update_line_centered(uint8_t line, const String &text,
                                  uint16_t text_color, uint16_t bg_color, uint16_t border_color)
{
    const int16_t char_width = 12;
    const int16_t screen_width = 240;
    const int16_t screen_center_x = screen_width / 2;
    const int16_t text_w = text.length() * char_width;
    const int16_t h = 20;

    int16_t y = 10 + line * 30;

    int16_t x = screen_center_x - (text_w / 2);

    tft.fillRect(0, y - 6, screen_width, h + 12, bg_color);

    tft.setCursor(x, y + 5);
    tft.setTextColor(text_color, bg_color);
    tft.setTextSize(2);
    tft.println(text);
}
