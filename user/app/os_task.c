/*
 * @Copyright: Copyright (C) 2021-2021 Kevin group. All rights reserved.
 * @Description: APP for freertos applications
 * @Author: Kevin
 * @Email: weikaiup@163.com
 * @Date: 2021-01-06
 */
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_conf.h"

#include "app/face.h"
#include "components/misc/dev_misc.h"

osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
    .name = "defaultTask",
    .priority = (osPriority_t)osPriorityNormal,
    .stack_size = 128 * 4
};

osThreadId_t myFaceTaskHdl;
const osThreadAttr_t myFaceTaskAttr = {
    .name = "myFaceTask",
    .priority = (osPriority_t)osPriorityLow,
    .stack_size = 512 * 4
};

osThreadId_t myMiscTaskHdl;
const osThreadAttr_t myMiscTaskAttr = {
    .name = "myMiscTask",
    .priority = (osPriority_t)osPriorityLow1,
    .stack_size = 128 * 4
};

osMessageQueueId_t myQueue01Handle;
const osMessageQueueAttr_t myQueue01_attributes = {
    .name = "myQueue01"
};

void StartDefaultTask(void *argument);
void FaceTask(void *argument);
void MiscTask(void *argument);

void OsTaskRun(void)
{
    myQueue01Handle = osMessageQueueNew(16, sizeof(uint16_t), &myQueue01_attributes);

    defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);
    if (defaultTaskHandle == NULL) {
        printf("defaultTask fail\r\n");
    }

    myFaceTaskHdl = osThreadNew(FaceTask, NULL, &myFaceTaskAttr);
    if (myFaceTaskHdl == NULL) {
        printf("FaceTask fail\r\n");
    }

    myMiscTaskHdl = osThreadNew(MiscTask, NULL, &myMiscTaskAttr);
    if (myMiscTaskHdl == NULL) {
        printf("MiscTask fail\r\n");
    }
}

void StartDefaultTask(void *argument)
{
    printf("DefaultTask start\r\n");
    for (;;) {
        if (IsBatCharging() == CHARGING_ON) {
            printf("is charging\r\n");
        }
        printf("Battery : %dV\r\n", GetBatteryLevel());
        osDelay(1000);
    }
}

void FaceTask(void *argument)
{
    printf("FaceTask start\r\n");
    task_screen(NULL);
}

void MiscTask(void *argument)
{
    uint16_t ledTim = 0;

    printf("MiscTask start\r\n");
    for (;;) {
        // 按键事件
        if (KeyScan() == KEY_ONE_CLICK) {
            ledTim += 100;
            if (ledTim == 3000) {
                ledTim = 0;
            }
        }
        // LED事件
        LedBlink(ledTim, ledTim);

        // 电池电压采样，这里循环调用，其他函数调用时，也会得到最新数据
        GetBatteryLevel();
        osDelay(10);
    }
}
