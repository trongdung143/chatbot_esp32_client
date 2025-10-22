#pragma once
#include <Arduino.h>
#include "driver/i2s.h"
#include "config.h"
#include "esp_dsp.h"
extern QueueHandle_t mic_to_server;
void mic_init();
void mic_task(void *param);
bool is_silent(int16_t *buf, int len, int threshold);