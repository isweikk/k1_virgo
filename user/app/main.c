/*
 * @Copyright: Copyright (C) 2021-2021 Kevin group. All rights reserved.
 * @Description: main process
 * @Author: Kevin
 * @Email: weikaiup@163.com
 * @Date: 2021-01-06
 */
#include "bsp.h"
#include "cmsis_os.h"
#include "components/com/console.h"

void OsTaskRun(void);

void ErrorHdl(void)
{
    __disable_irq();
    while (1) {
    }
}

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
    ConsoleInit();  // 初始化控制台结构，优先创建，可让printf提前生效
    BspInit();
    OsTaskRun();

    /* We should never get here as control is now taken by the scheduler */
    while (1) {
    }
}
