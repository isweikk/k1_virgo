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
#include "components/util/util_sys.h"
#include "components/motor/mpu6050/mpu6050.h"

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

osThreadId_t myMotionTaskHdl;
const osThreadAttr_t myMotionTaskAttr = {
    .name = "myMotionTask",
    .priority = (osPriority_t)osPriorityHigh,
    .stack_size = 128 * 4
};

osMessageQueueId_t myQueue01Handle;
const osMessageQueueAttr_t myQueue01_attributes = {
    .name = "myQueue01"
};

void StartDefaultTask(void *argument);
void FaceTask(void *argument);
void MiscTask(void *argument);
void MotionTask(void *argument);

void OsTaskRun(void)
{
    /* Init scheduler */
    osKernelInitialize(); /* Call init function for freertos objects (in freertos.c) */

    myQueue01Handle = osMessageQueueNew(16, sizeof(uint16_t), &myQueue01_attributes);

    defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);
    if (defaultTaskHandle == NULL) {
        uloge("defaultTask fail");
    }

    myFaceTaskHdl = osThreadNew(FaceTask, NULL, &myFaceTaskAttr);
    if (myFaceTaskHdl == NULL) {
        uloge("FaceTask fail");
    }

    myMiscTaskHdl = osThreadNew(MiscTask, NULL, &myMiscTaskAttr);
    if (myMiscTaskHdl == NULL) {
        uloge("MiscTask fail");
    }

    myMotionTaskHdl = osThreadNew(MotionTask, NULL, &myMotionTaskAttr);
    if (myMotionTaskHdl == NULL) {
        uloge("MotionTask fail");
    }
    ulog("Create all tasks OK");

    osKernelStart();
}

void StartDefaultTask(void *argument)
{
    ulog("DefaultTask start");
    
    for (;;) {
        if (IsBatCharging() == CHARGING_ON) {
            // ulog("is charging");
        }
        // ulog("Battery : %dV", GetBatteryLevel());
        DelayMs(1000);
    }
    ulog("DefaultTask exit");
}

void FaceTask(void *argument)
{
    ulog("FaceTask start");
    task_screen(NULL);
    ulog("FaceTask exit");
}

void MiscTask(void *argument)
{
    uint16_t ledTim = 0;

    ulog("MiscTask start");
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
    ulog("MiscTask exit");
}

void MotionTask(void *argument)
{
    ulog("MotionTask start");
    MPU_Init();
    while (1) {
        if (g_mpu6050Flag) {
            float pitch, roll, yaw; 					    //欧拉角(姿态角)
            short gyrox, gyroy, gyroz;						//陀螺仪原始数据
            mpu_dmp_get_data(&pitch, &roll, &yaw);					//===得到欧拉角（姿态角）的数据
            MPU_GetGyroscope(&gyrox, &gyroy, &gyroz);				//===得到陀螺仪数据
            ulogd("inv: %f, %f, %f, %d, %d, %d", pitch, roll, yaw, gyrox, gyroy, gyroz);
            g_mpu6050Flag = 0;
        }
        osDelay(1);
    }
    ulog("MotionTask exit");
}
