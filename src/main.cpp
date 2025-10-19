#include "config.h"
#include <WiFi.h>
#include <WiFiManager.h>
#include "wifi_client.h"
#include "mic.h"
#include "display.h"

WebSocketsClient ws;
QueueHandle_t mic_to_server = xQueueCreate(10, I2S_BUFFER_LEN * sizeof(int16_t));
QueueHandle_t server_to_speaker;
volatile bool g_send_pcm_enabled = false;
volatile bool g_mic_enabled = false;
volatile bool enabled = false;

void setup()
{
  Serial.begin(115200);
  delay(500);
  display_init();
  mic_init();
  wifi_connect();
  display_clear();
  init_websocket_client();
  delay(500);
  xTaskCreate(mic_task, "mic_task", 4096, NULL, 1, NULL);
  xTaskCreate(send_pcm_task, "send_pcm_task", 4096, NULL, 1, NULL);
}

void loop()
{
  websocket_loop();
  delay(10);
}
