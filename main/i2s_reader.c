// === i2s_reader.c ===
#include "driver/i2s_std.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "i2s_reader.h"

#define I2S_PORT         I2S_NUM_0
#define I2S_SAMPLE_RATE  16000
#define I2S_BUFFER_SIZE  1024
#define I2S_DMA_BUF_LEN  512

static int32_t i2s_buffer[I2S_BUFFER_SIZE];
static i2s_chan_handle_t rx_handle = NULL;

void i2s_reader_init()
{
    i2s_chan_config_t chan_cfg = {
        .id = I2S_PORT,
        .role = I2S_ROLE_MASTER,
        .dma_desc_num = 4,
        .dma_frame_num = I2S_DMA_BUF_LEN,
        .auto_clear = true
    };
    i2s_new_channel(&chan_cfg, NULL, &rx_handle);

    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(I2S_SAMPLE_RATE),
        .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = GPIO_NUM_5,
            .ws   = GPIO_NUM_4,
            .dout = I2S_GPIO_UNUSED,
            .din  = GPIO_NUM_6,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv   = false,
            },
        }
    };
    i2s_channel_init_std_mode(rx_handle, &std_cfg);
    i2s_channel_enable(rx_handle);
}

int32_t* i2s_read_samples(size_t* sample_count)
{
    size_t bytes_read = 0;
    i2s_channel_read(rx_handle, i2s_buffer, sizeof(i2s_buffer), &bytes_read, portMAX_DELAY);
    *sample_count = bytes_read / sizeof(int32_t);
    return i2s_buffer;
}