#pragma once
#include <Arduino.h>
#include "driver/i2s.h"
#include "config.h"

void spk_init();
void spk_task(void *param);
