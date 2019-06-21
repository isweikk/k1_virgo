/**
 * the handler of the events.
 */
#include "general_api.h"

#include "bsp.h"

static float BatteryVolt = 0;

/**************************************************************************
函数功能：读取电池电压 
入口参数：无
返回  值：电池电压 单位MV
**************************************************************************/
float GetBatteryVolt(void)
{
    BatteryVolt = GetAdc(Battery_Ch)*3.3*11/4096;
    return BatteryVolt;
}

/**************************************************************************
函数功能：检查电池低电压 
入口参数：无
返回  值：电池状态，0=正常，1=低压
**************************************************************************/
u8 CheckBatteryLowPwr(void)
{
    if (BatteryVolt < 3.6) {
        return 1;
    }
    return 0;
}

