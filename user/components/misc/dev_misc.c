/*
 * @Copyright: Copyright (C) 2021-2021 Kevin group. All rights reserved.
 * @Description: the driver of the misc devices: keyboard, led, charging pin.
 * @Author: Kevin
 * @Email: weikaiup@163.com
 * @Date: 2021-01-07
 */
#include "dev_misc.h"

#include "cmsis_os.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_conf.h"

#include "bsp/adc.h"

#define DEV_KEY_PIN_GROUP GPIOC
#define DEV_KEY_PIN_NUM GPIO_PIN_15
#define DEV_KEY_PIN_ON_LEVEL GPIO_PIN_RESET

#define DEV_LED_PIN_GROUP GPIOC
#define DEV_LED_PIN_NUM GPIO_PIN_14
#define DEV_LED_ON GPIO_PIN_RESET
#define DEV_LED_OFF GPIO_PIN_SET

#define DEV_USB_STATE_PIN_GROUP GPIOC
#define DEV_USB_STATE_PIN_NUM GPIO_PIN_14
#define DEV_USB_STATE_ON GPIO_PIN_SET

// 按键扫描，按键松开生效，return: 按键状态 0：无动作 1：单击
uint8_t KeyScan(void)
{
    static uint8_t isKeyUp = true;  // 按键按松开标志
    uint8_t pinLevel = HAL_GPIO_ReadPin(DEV_KEY_PIN_GROUP, DEV_KEY_PIN_NUM);
    if (isKeyUp && (pinLevel == DEV_KEY_PIN_ON_LEVEL)) {
        isKeyUp = false;
        return KEY_IDLE;
    } else if ((isKeyUp == false) && (pinLevel != DEV_KEY_PIN_ON_LEVEL)) {
        isKeyUp = true;
        return KEY_ONE_CLICK;  // 确认一次点击
    }
    return KEY_IDLE;
}

void LedSet(uint8_t onOff)
{
    if (onOff) {
        HAL_GPIO_WritePin(DEV_LED_PIN_GROUP, DEV_LED_PIN_NUM, DEV_LED_ON);
    } else {
        HAL_GPIO_WritePin(DEV_LED_PIN_GROUP, DEV_LED_PIN_NUM, DEV_LED_OFF);
    }
}

// LED闪烁, 点亮时间，熄灭时间，时间为毫秒，参数都为0，则灭灯
void LedBlink(uint16_t lightMs, uint16_t darkMs)
{
    static uint8_t isLedLight = false;
    static uint32_t lastTime = 0;
    uint32_t nowTime = osKernelGetTickCount() * 1000 / osKernelGetTickFreq();  // runtime，单位毫秒

    if (lightMs == 0 && darkMs == 0) {
        LedSet(false);
        return;
    }

    if (lastTime == 0) {
        lastTime = nowTime;
    }
    if (isLedLight == true) {
        if (nowTime - lastTime > lightMs) {
            LedSet(false);
            isLedLight = false;
            lastTime = nowTime;
        }
    } else {
        if (nowTime - lastTime > darkMs) {
            LedSet(true);
            isLedLight = true;
            lastTime = nowTime;
        }
    }
}

// 充电状态
uint8_t IsBatCharging(void)
{
    if (HAL_GPIO_ReadPin(DEV_USB_STATE_PIN_GROUP, DEV_USB_STATE_PIN_NUM) == DEV_USB_STATE_ON) {
        return CHARGING_ON;
    }
    return CHARGING_NONE;
}

// 电池采样
uint16_t GetBatteryLevel(void)
{
    static uint16_t batLevel = 0;
    static uint16_t valBuf[5] = {0};    // 采样数据缓存，采样5次，进行一次计算
    static uint8_t capCnt = 0;  // 采样次数
    
    if (capCnt < 5) {
        valBuf[capCnt++] = BspAdc1GetValueOnce();
    }

    if (capCnt == 5) {
        uint32_t batLevelTmp = 0;
        for (uint8_t i = 0; i < 5; i++) {
            batLevelTmp += valBuf[i];
        }

        // 乘以11，是因为1k + 10k电阻分压，总电压是11份；计算时放大100倍，即返回值330=3.3v
        batLevelTmp = (batLevelTmp / capCnt) * 330 * 11 / 4096;
        batLevel = (uint16_t)batLevelTmp;
        capCnt = 0;
    }

    return batLevel;
}
