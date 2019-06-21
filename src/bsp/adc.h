/**
 * info: 通过ADC采样电池电压
 */

#ifndef __ADC_H__
#define __ADC_H__

#include "sys.h"

#define Battery_Ch 8		//通道8
void AdcInit(void);
u16 GetAdc(u8 ch);
float GetBatteryVolt(void);  
#endif 
