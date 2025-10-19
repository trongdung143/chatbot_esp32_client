#include "mic.h"
#include "display.h"
#include "esp_heap_caps.h"

void mic_init()
{
    i2s_config_t cfg = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = I2S_SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = 0,
        .dma_buf_count = 4,
        .dma_buf_len = I2S_BUFFER_LEN,
        .use_apll = false,
        .tx_desc_auto_clear = false,
        .fixed_mclk = 0};

    i2s_pin_config_t pin = {
        .bck_io_num = I2S_SCK,
        .ws_io_num = I2S_WS,
        .data_out_num = -1,
        .data_in_num = I2S_SD};

    i2s_driver_install(I2S_MIC_PORT, &cfg, 0, NULL);
    i2s_set_pin(I2S_MIC_PORT, &pin);
    i2s_start(I2S_MIC_PORT);
}

void mic_task(void *param)
{
    int8_t silence_counter = 0;
    (void)param;
    while (1)
    {
        if (!g_mic_enabled)
        {
            vTaskDelay(pdMS_TO_TICKS(50));
            continue;
        }

        if (enabled && g_mic_enabled)
        {
            ws.sendTXT("start|" + String(CLIENT_ID));
            enabled = false;
        }

        int32_t i2s_read_buff[I2S_BUFFER_LEN];
        int16_t i2s_buf[I2S_BUFFER_LEN];
        size_t bytes_read;
        i2s_read(I2S_MIC_PORT, i2s_read_buff, sizeof(i2s_read_buff), &bytes_read, portMAX_DELAY);

        for (int i = 0; i < I2S_BUFFER_LEN; i++)
            i2s_buf[i] = i2s_read_buff[i] >> 14;

        if (bytes_read > 0)
        {
            if (is_silent(i2s_buf, I2S_BUFFER_LEN, SILENCE_THRESHOLD))
            {
                silence_counter++;
                if (silence_counter >= SILENCE_BLOCK_LIMIT)
                {
                    display_show_text("Thinking...", 10, 100, GC9A01A_CYAN, GC9A01A_BLACK);
                    g_mic_enabled = false;
                }
            }
            else
            {
                silence_counter = 0;
                int16_t *pcm = (int16_t *)heap_caps_malloc(bytes_read / 2, MALLOC_CAP_SPIRAM);
                memcpy(pcm, i2s_buf, bytes_read / 2);
                xQueueSend(mic_to_server, &pcm, 0);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

bool is_silent(int16_t *buf, int len, int threshold)
{
    uint64_t sum = 0;
    for (int i = 0; i < len; i++)
        sum += abs(buf[i]);
    return (sum / len) < threshold;
}
