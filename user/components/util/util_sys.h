/*
 * @Copyright: Copyright (C) 2021-2021 Kevin group. All rights reserved.
 * @Description: the delay, time.
 * @Author: Kevin
 * @Email: weikaiup@163.com
 * @Date: 2021-01-07
 */
#ifndef UTIL_SYS_H
#define UTIL_SYS_H

#include "stdio.h"
#include "typedef.h"

#define DEBUG   1

#define ulog(format, ...) printf("[%-15.15s][%4d][I]: "format"\r\n", __func__, __LINE__, ##__VA_ARGS__)
#define uloge(format, ...) printf("[%-15.15s][%4d][E]: "format"\r\n", __func__, __LINE__, ##__VA_ARGS__)

#if DEBUG
#define ulogd(format, ...) printf("[%-15.15s][%4d][D]: "format"\r\n", __func__, __LINE__, ##__VA_ARGS__)
#else
#define ulogd(format, ...)
#endif

void DelayMs(uint32_t ms);
void DelayUs(uint32_t us);
uint32_t GetRuntimeMs(void);    // 最长运行49天

#endif  // UTIL_SYS_H
