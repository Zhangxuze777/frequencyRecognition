#include <stdio.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s_std.h"
#include "esp_log.h"
#include "esp_dsp.h"

// I2S pin configuration
#define I2S_WS   4      // WS (Word Select) a.k.a LRCK
#define I2S_SD   6      // Serial Data (microphone DOUT to ESP32 DIN)
#define I2S_SCK  5      // Serial Clock (Bit Clock)

// I2S and FFT parameters
#define I2S_PORT I2S_NUM_0
#define SAMPLE_RATE 48000      // Sampling rate in Hz
#define BUFFER_LEN 1024        // Number of audio samples per buffer
#define FFT_LEN BUFFER_LEN     // FFT length matches buffer length
#define MIN_BIN 0              // Minimum FFT bin to consider
#define MAX_BIN 86             // Maximum bin corresponds to ~4000Hz

static const char* TAG = "FFT";
static i2s_chan_handle_t rx_handle;
static int16_t sBuffer[BUFFER_LEN];                           // Raw audio buffer (signed 16-bit samples)
__attribute__((aligned(16))) float fft_input[FFT_LEN * 2];   // Interleaved complex input array [real, imag, real, imag...]
__attribute__((aligned(16))) float window[FFT_LEN];          // Hann window coefficients

// Initialize the I2S peripheral for standard mode RX
void i2s_reader_init()
{
    i2s_chan_config_t chan_cfg = {
        .id = I2S_PORT,
        .role = I2S_ROLE_MASTER,
        .dma_desc_num = 4,
        .dma_frame_num = BUFFER_LEN,
        .auto_clear = true
    };

    ESP_ERROR_CHECK(i2s_new_channel(&chan_cfg, NULL, &rx_handle));

    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(SAMPLE_RATE),
        .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = I2S_SCK,
            .ws   = I2S_WS,
            .dout = I2S_GPIO_UNUSED,
            .din  = I2S_SD,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv   = false,
            }
        }
    };

    ESP_ERROR_CHECK(i2s_channel_init_std_mode(rx_handle, &std_cfg));
    ESP_ERROR_CHECK(i2s_channel_enable(rx_handle));
    ESP_LOGI(TAG, "I2S is initialized!"); // I2S initialized
}

// Perform FFT on the collected audio samples and detect dominant frequency
void fft_process()
{
    // Apply Hann window and prepare complex input (imaginary parts = 0)
    for (int i = 0; i < FFT_LEN; i++) {
        fft_input[i * 2]     = (float)sBuffer[i] * window[i];  // Real part
        fft_input[i * 2 + 1] = 0.0f;                            // Imaginary part
    }

    // Perform FFT
    dsps_fft2r_fc32(fft_input, FFT_LEN);
    dsps_bit_rev_fc32(fft_input, FFT_LEN);
    dsps_cplx2reC_fc32(fft_input, FFT_LEN);

    // Search for peak magnitude in selected bin range
    int max_index = MIN_BIN;
    float max_power = 0.0f;
    for (int i = MIN_BIN; i <= MAX_BIN; i++) {
        float real = fft_input[2 * i];
        float imag = fft_input[2 * i + 1];
        float mag = real * real + imag * imag;
        if (mag > max_power) {
            max_power = mag;
            max_index = i;
        }
    }

    // Convert bin index to frequency and apply correction factor
    float frequency = ((float)max_index * SAMPLE_RATE / FFT_LEN) * 2.13675f;
    ESP_LOGI(TAG, "the main frequency is  %.2f Hz", frequency);
}

// Main application entry point
void app_main()
{
    i2s_reader_init();                                // Setup I2S interface
    dsps_fft2r_init_fc32(NULL, FFT_LEN);              // Initialize FFT module
    dsps_wind_hann_f32(window, FFT_LEN);              // Generate Hann window

    while (1) {
        size_t bytes_read = 0;
        esp_err_t result = i2s_channel_read(rx_handle, sBuffer, sizeof(sBuffer), &bytes_read, portMAX_DELAY);
        if (result == ESP_OK && bytes_read > 0) {
            fft_process();                            // Analyze audio samples
        }
        vTaskDelay(pdMS_TO_TICKS(50));                // Small delay (50 ms)
    }
}
