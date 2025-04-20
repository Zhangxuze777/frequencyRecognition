// === fft_processor.c ===
#include <math.h>
#include "fft_processor.h"
#include "esp_dsp.h"

#define FFT_SIZE 1024

float calculate_dominant_freq(int32_t* samples, size_t count, int sample_rate)
{
    float input[FFT_SIZE * 2];
    float magnitude[FFT_SIZE / 2];

    for (int i = 0; i < FFT_SIZE; i++) {
        input[2 * i] = (float)samples[i];
        input[2 * i + 1] = 0.0f;
    }

    dsps_fft2r_fc32(input, FFT_SIZE);
    dsps_bit_rev_fc32(input, FFT_SIZE);

    for (int i = 0; i < FFT_SIZE / 2; i++) {
        float real = input[2 * i];
        float imag = input[2 * i + 1];
        magnitude[i] = sqrtf(real * real + imag * imag);
    }

    int max_index = 1;
    float max_value = magnitude[1];
    for (int i = 2; i < FFT_SIZE / 2; i++) {
        if (magnitude[i] > max_value) {
            max_value = magnitude[i];
            max_index = i;
        }
    }

    float freq_resolution = ((float)sample_rate / FFT_SIZE);
    return max_index * freq_resolution;
}
