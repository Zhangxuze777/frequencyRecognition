# frequencyRecognition

This project is for frequency recognition using ESP32-S3 and a digital microphone (e.g., INMP441).

| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

## Description

This project reads audio samples from a digital I2S microphone and performs real-time FFT analysis to estimate the dominant frequency in the signal.

## Features

- I2S audio acquisition
- Hann window smoothing
- FFT analysis (ESP-DSP library)
- Dominant frequency estimation
