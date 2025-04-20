#pragma once

#include <stdint.h>
#include <stddef.h>

/**
 * @brief 初始化 I2S 接口，用于从 INMP441 读取音频数据
 */
void i2s_reader_init();

/**
 * @brief 从 INMP441 读取一段音频样本数据
 * 
 * @param sample_count 返回实际采集的样本数量
 * @return int32_t* 指向样本数据的指针（数组）
 */
int32_t* i2s_read_samples(size_t* sample_count);
