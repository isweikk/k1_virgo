/*
 * @Copyright: Copyright (C) 2021-2021 Kevin group. All rights reserved.
 * @Description: the delay, time.
 * @Author: Kevin
 * @Email: weikaiup@163.com
 * @Date: 2021-01-07
 */
#include "util_sys.h"

#include "cmsis_os.h"

void DelayMs(uint32_t ms)
{
    uint32_t tick = osKernelGetTickFreq() * ms / 1000;
    osDelay(tick);
}

void DelayUs(uint32_t us)
{
    int i = 1000;
    while(i > 0) {
        i--;
    }
}

uint32_t GetRuntimeMs(void)
{
    uint64_t time = osKernelGetTickCount();
    time = time * 1000 / osKernelGetTickFreq();
    return (uint32_t)time;
}
