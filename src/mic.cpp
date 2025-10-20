#include "mic.h"
#include "display.h"
#include "esp_heap_caps.h"
#include "led.h"

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
    (void)param;
    int8_t queue_size;
    while (1)
    {
        if (!mic_enabled)
        {
            queue_size = 0;
            vTaskDelay(pdMS_TO_TICKS(50));
            continue;
        }

        int32_t i2s_read_buff[I2S_BUFFER_LEN];
        int16_t i2s_buf[I2S_BUFFER_LEN];
        size_t bytes_read;

        i2s_read(I2S_MIC_PORT, i2s_read_buff, sizeof(i2s_read_buff), &bytes_read, portMAX_DELAY);

        for (int i = 0; i < bytes_read / 4; i++)
            i2s_buf[i] = i2s_read_buff[i] >> 14;

        if (bytes_read > 0)
        {
            if (!is_silent(i2s_buf, I2S_BUFFER_LEN, SILENCE_THRESHOLD))
            {

                queue_size += 1;
                int16_t *pcm = (int16_t *)heap_caps_malloc(bytes_read / 2, MALLOC_CAP_SPIRAM);
                memcpy(pcm, i2s_buf, bytes_read / 2);
                PcmChunk chunk = {pcm, bytes_read / 2};
                xQueueSend(mic_to_server, &chunk, 0);
                if (queue_size > 3 && !pcm_sending)
                {
                    ws.sendTXT("start_chat|" + String(CLIENT_ID));
                    pcm_sending = true;
                }
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
