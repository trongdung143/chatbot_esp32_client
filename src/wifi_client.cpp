#include <WiFi.h>
#include <WiFiManager.h>
#include "display.h"
#include "wifi_client.h"
#include "freertos/queue.h"

void wifi_connect()
{
    display_update_line_centered(4, "WiFi Setup Mode", GC9A01A_YELLOW, GC9A01A_BLACK, GC9A01A_WHITE);

    WiFiManager wifiManager;
    bool connected = wifiManager.autoConnect("chatbot_client", "12345678");

    if (!connected)
    {
        display_update_line_centered(4, "No Connection!", GC9A01A_RED, GC9A01A_BLACK, GC9A01A_WHITE);
    }
    else
    {
        init_websocket_client();
        String ip = WiFi.localIP().toString();
        display_update_line_centered(4, "Connected", GC9A01A_GREEN, GC9A01A_BLACK, GC9A01A_WHITE);
    }
}

static void onWsEvent(WStype_t type, uint8_t *payload, size_t length)
{
    switch (type)
    {
    case WStype_CONNECTED:
        display_update_line_centered(4, "Connected to server", GC9A01A_GREEN, GC9A01A_BLACK, GC9A01A_WHITE);
        mic_enabled = true;
        break;

    case WStype_DISCONNECTED:
        display_update_line_centered(4, "Disconnected!", GC9A01A_RED, GC9A01A_BLACK, GC9A01A_WHITE);
        break;

    case WStype_TEXT:
    {

        String msg = String((char *)payload);
        if (msg == "end_stream_audio")
        {
            pcm_receiving = false;
        }
        else if (msg.startsWith("start_stream_audio"))
        {
            pcm_receiving = true;
            chunks = msg.indexOf("chunks=");
        }
        break;
    }

    case WStype_BIN:
    {
        if (pcm_receiving)
        {
            int16_t *pcm = (int16_t *)heap_caps_malloc(length, MALLOC_CAP_SPIRAM);
            if (pcm)
            {
                memcpy(pcm, payload, length);
                PcmChunk chunk = {pcm, length};
                xQueueSend(server_to_spk, &chunk, 0);
                UBaseType_t count = uxQueueMessagesWaiting(server_to_spk);
                if (count > chunks && !spk_enabled)
                {
                    spk_enabled = true;
                }
            }
        }
        break;
    }
    default:
        break;
    }
}

static void init_websocket_client()
{
    const char *server = IP_HOST;
    const int port = PORT_HOST;
    const char *path = "/ws_audio";

    ws.begin(server, port, path);
    ws.onEvent(onWsEvent);
    ws.setReconnectInterval(5000);
    ws.enableHeartbeat(15000, 3000, 2);
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
        if (!pcm_sending)
        {
            vTaskDelay(pdMS_TO_TICKS(50));
            continue;
        }

        PcmChunk chunk;

        if (xQueueReceive(mic_to_server, &chunk, pdMS_TO_TICKS(2000)) == pdTRUE)
        {
            if (chunk.pcm && chunk.bytes > 0 && ws.isConnected())
            {
                ws.sendBIN((uint8_t *)chunk.pcm, chunk.bytes);
            }

            if (chunk.pcm)
                heap_caps_free(chunk.pcm);
        }
        else
        {
            ws.sendTXT("end_chat");
            xQueueReset(mic_to_server);
            pcm_sending = false;
            mic_enabled = false;
        }

        vTaskDelay(pdMS_TO_TICKS(1));
    }
}
