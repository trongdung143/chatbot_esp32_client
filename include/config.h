#include <Arduino.h>
#include <WebSocketsClient.h>
#include "freertos/queue.h"

extern QueueHandle_t mic_to_server;
extern QueueHandle_t server_to_speaker;
extern WebSocketsClient ws;

extern volatile bool g_send_pcm_enabled;
extern volatile bool g_mic_enabled;
extern volatile bool enabled;

#define CLIENT_ID "c7b12d5f-8a3e-4bc1-ae3d-9f6a1200a52b"

// SERVER CONFIG
#define IP_HOST "192.168.1.149"
#define PORT_HOST 8080
#define API_AUDIO_UPLOAD "/audio_stream"
#define API_STREAM_REPLY "/stream_reply"

// MIC (INMP441) CONFIG
#define I2S_MIC_PORT I2S_NUM_0
#define I2S_SAMPLE_RATE 16000
#define I2S_BUFFER_LEN 1024
#define SILENCE_THRESHOLD 300
#define SILENCE_BLOCK_LIMIT 32

#define I2S_WS 4
#define I2S_SCK 5
#define I2S_SD 6

// LOA (MAX98357A) CONFIG
#define I2S_SPK_PORT I2S_NUM_1
#define I2S_SPK_BCLK 15
#define I2S_SPK_LRCK 16
#define I2S_SPK_DOUT 7
#define I2S_SPK_SAMPLE_RATE 16000

// MÀN HÌNH GC9A01A CONFIG
#define TFT_CS 41
#define TFT_DC 40
#define TFT_RST 45
#define TFT_SDA 47
#define TFT_SCL 21

// PSRAM / BUFFER CONFIG
#define USE_PSRAM true
#define MAX_AUDIO_BUFFER 8192