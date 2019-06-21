/**
 * the driver of the ultrasonic wav 
 */
#ifndef __ULTRASONIC_WAVE_H__
#define __ULTRASONIC_WAVE_H__

#include "sys.h" 

extern float UltrasonicWaveDistance;                 //超声波测距

void UltrasonicWaveInit(void);               //对超声波模块初始化
void UltrasonicWaveDetect(void);                //开始测距，发送一个>10us的脉冲，然后测量返回的高电平时间
u8 UltrasonicWaveWarning(void);  //检查超声波是否危险，0 = 安全，1=危险

#endif /* __UltrasonicWave_H */

