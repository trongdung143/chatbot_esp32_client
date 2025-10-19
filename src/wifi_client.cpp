#include <WiFi.h>
#include <WiFiManager.h>
#include "display.h"
#include "wifi_client.h"
#include "freertos/queue.h"

void wifi_connect()
{
    tft.fillScreen(GC9A01A_BLACK);
    display_show_text("WiFi Setup Mode", 30, 80, GC9A01A_YELLOW, GC9A01A_BLACK);

    WiFiManager wifiManager;
    bool connected = wifiManager.autoConnect("chatbot-esp32", "12345678");

    if (!connected)
    {
        display_show_text("No Connection!", 40, 120, GC9A01A_RED, GC9A01A_BLACK);
    }
    else
    {
        display_show_text("Connected!", 40, 120, GC9A01A_GREEN, GC9A01A_BLACK);
        String ip = WiFi.localIP().toString();
        display_show_text("IP: " + ip, 40, 150, GC9A01A_CYAN, GC9A01A_BLACK);
    }
}

static void onWsEvent(WStype_t type, uint8_t *payload, size_t length)
{
    switch (type)
    {
    case WStype_CONNECTED:
        display_show_text("WS Connected", 30, 180, GC9A01A_GREEN, GC9A01A_BLACK);
        g_send_pcm_enabled = true;
        g_mic_enabled = true;
        enabled = true;
        break;

    case WStype_DISCONNECTED:
        display_show_text("WS Disconnected", 30, 180, GC9A01A_RED, GC9A01A_BLACK);
        break;

    case WStype_TEXT:
    {
        String msg = String((char *)payload);
        if (msg == "continue")
        {
            g_send_pcm_enabled = true;
            g_mic_enabled = true;
            enabled = true;
            display_show_text(">> Continue", 30, 200, GC9A01A_GREEN, GC9A01A_BLACK);
        }
        else if (msg == "stop")
        {
            display_show_text(">> Stopped", 30, 200, GC9A01A_RED, GC9A01A_BLACK);
        }
        break;
    }
    default:
        break;
    }
}

void init_websocket_client()
{
    const char *server = IP_HOST;
    const int port = PORT_HOST;
    const char *path = "/ws_audio";

    ws.begin(server, port, path);
    ws.onEvent(onWsEvent);
    ws.setReconnectInterval(5000);
    ws.enableHeartbeat(15000, 3000, 2);

    display_show_text("WebSocket Init", 30, 80, GC9A01A_YELLOW, GC9A01A_BLACK);
}

void websocket_loop()
{
    ws.loop();
}

void send_pcm_task(void *param)
{
    (void)param;
    while (true)
    {
        if (!g_send_pcm_enabled)
        {
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }
        int16_t *pcm;
        if (xQueueReceive(mic_to_server, &pcm, portMAX_DELAY) == pdTRUE)
        {
            if (ws.isConnected())
            {
                ws.sendBIN((uint8_t *)pcm, I2S_BUFFER_LEN * sizeof(int16_t));
            }
            free(pcm);
            vTaskDelay(pdMS_TO_TICKS(1));
            continue;
        }
        ws.sendTXT("end");
        g_send_pcm_enabled = false;
    }
}

void revice_pcm(void *param)
{
    (void)param;
    g_send_pcm_enabled = true;
}