#pragma once

#include <stdint.h>
#include <stddef.h>

/**
 * @brief 使用 FFT 计算音频样本中的主频率
 * 
 * @param samples 原始采集的音频样本数组
 * @param count 样本数量
 * @param sample_rate 采样率（Hz）
 * @return float 返回主频（Hz）
 */
float calculate_dominant_freq(int32_t* samples, size_t count, int sample_rate);
