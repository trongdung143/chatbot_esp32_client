#pragma once
#define WIFI_CLIENT_H

#include "config.h"
#include <WiFi.h>

void wifi_connect();
static void onWsEvent(WStype_t type, uint8_t *payload, size_t length);
static void init_websocket_client();
void websocket_loop();
void send_pcm_task(void *param);
void revice_pcm_task(void *param);