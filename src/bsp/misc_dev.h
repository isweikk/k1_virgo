/**
 * the driver of all misc devices.
 */
#ifndef __BSP_MISC_DEV_H__
#define __BSP_MISC_DEV_H__

#include "sys.h"

// keyboard
#define KEY_PIN_READ PCin(15)

void KeyInit(void);          //按键初始化
int KeyScan(void);          //按键扫描

//LED 端口定义
#define LED_PIN_OUT PCout(14) // PC14

void LedInit(void);  //初始化
void LedBlink(u16 time);

//usb charging port
void ChargingPortInit(void);
u8 GetChargingStatus(void);

#endif  
