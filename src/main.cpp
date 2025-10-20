#include "config.h"
#include <WiFi.h>
#include <WiFiManager.h>
#include "wifi_client.h"
#include "mic.h"
#include "display.h"
#include "led.h"
#include "spk.h"

WebSocketsClient ws;
QueueHandle_t mic_to_server;
QueueHandle_t server_to_spk;
volatile bool mic_enabled = false;
volatile bool pcm_sending = false;
volatile bool spk_enabled = false;
volatile bool pcm_receiving = false;
volatile int chunks = 0;

unsigned long last_update = 0;

void setup()
{
  Serial.begin(115200);
  delay(1000);

  wifi_connect();
  delay(500);

  display_init();
  delay(500);

  led_init();
  delay(500);

  mic_init();
  delay(500);

  spk_init();
  delay(500);

  mic_to_server = xQueueCreate(20, sizeof(PcmChunk));
  server_to_spk = xQueueCreate(100, sizeof(PcmChunk));

  display_clear();

  xTaskCreatePinnedToCore(mic_task, "mic_task", 1024 * 20, NULL, 1, NULL, 0);
  delay(10);
  xTaskCreatePinnedToCore(send_pcm_task, "send_pcm_task", 1024 * 12, NULL, 1, NULL, 1);
  delay(10);
  xTaskCreatePinnedToCore(spk_task, "spk_task", 1024 * 12, NULL, 1, NULL, 0);
}

void loop()
{
  websocket_loop();
  delay(1);
}
