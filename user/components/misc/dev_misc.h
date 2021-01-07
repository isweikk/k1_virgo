/*
 * @Copyright: Copyright (C) 2021-2021 Kevin group. All rights reserved.
 * @Description: file content
 * @Author: Kevin
 * @Email: weikaiup@163.com
 * @Date: 2021-01-07
 */
/**
 * the driver of all misc devices.
 */
#ifndef BSP_DEV_MISC_H
#define BSP_DEV_MISC_H

#include "typedef.h"

enum KeyState{
    KEY_IDLE = 0,
    KEY_ONE_CLICK,
};
// 按键扫描，按键松开生效，return: 按键状态 0：无动作 1：单击 
uint8_t KeyScan(void);

// true 开灯， false 灭灯
void LedSet(uint8_t onOff);
// 参数全0则灭灯
void LedBlink(uint16_t lightMs, uint16_t darkMs);

//usb charging port
enum ChargingState{
    CHARGING_NONE = 0,
    CHARGING_ON
};
uint8_t IsBatCharging(void);
// 电池电压，放大100倍
uint16_t GetBatteryLevel(void);

#endif  // BSP_DEV_MISC_H
