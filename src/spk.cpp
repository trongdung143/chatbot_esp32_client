#include "spk.h"

void spk_init()
{
    i2s_config_t cfg = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = I2S_SPK_SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 4,
        .dma_buf_len = 512,
        .use_apll = false};

    i2s_pin_config_t pin = {
        .bck_io_num = I2S_SPK_BCLK,
        .ws_io_num = I2S_SPK_LRCK,
        .data_out_num = I2S_SPK_DOUT,
        .data_in_num = -1};

    i2s_driver_install(I2S_SPK_PORT, &cfg, 0, NULL);
    i2s_set_pin(I2S_SPK_PORT, &pin);
    i2s_start(I2S_SPK_PORT);
}

void spk_task(void *param)
{
    (void)param;
    const float volume = 0.3f;
    PcmChunk chunk;

    while (true)
    {
        if (!spk_enabled)
        {
            vTaskDelay(pdMS_TO_TICKS(50));
            continue;
        }

        if (xQueueReceive(server_to_spk, &chunk, pdMS_TO_TICKS(300)) == pdTRUE)
        {
            if (chunk.pcm && chunk.bytes > 0)
            {
                size_t samples = chunk.bytes / sizeof(int16_t);
                int16_t *buf = (int16_t *)heap_caps_malloc(chunk.bytes, MALLOC_CAP_SPIRAM);
                if (buf)
                {
                    for (size_t i = 0; i < samples; i++)
                    {
                        int32_t val = (int32_t)(chunk.pcm[i] * volume);
                        if (val > 32767)
                            val = 32767;
                        if (val < -32768)
                            val = -32768;
                        buf[i] = (int16_t)val;
                    }

                    size_t bytes_written;
                    i2s_write(I2S_SPK_PORT, buf, chunk.bytes, &bytes_written, portMAX_DELAY);

                    heap_caps_free(buf);
                }
                heap_caps_free(chunk.pcm);
            }
        }
        else
        {
            if (uxQueueMessagesWaiting(server_to_spk) == 0)
            {
                spk_enabled = false;
                mic_enabled = true;
                xQueueReset(server_to_spk);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(1));
    }
}