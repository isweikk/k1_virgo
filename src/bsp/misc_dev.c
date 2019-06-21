/**
 * the driver of the misc devices
 * keyboard, led, charging pin.
 */
#include "misc_dev.h"

/**************************************************************************
函数功能：按键初始化
入口参数：无
返回  值：无 
**************************************************************************/
void KeyInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); //使能PA端口时钟
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;	           //端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;         //上拉输入
    GPIO_Init(GPIOC, &GPIO_InitStructure);					      //根据设定参数初始化GPIOA 
} 
/**************************************************************************
函数功能：按键扫描
入口参数：无
返回  值：按键状态 0：无动作 1：单击 
**************************************************************************/
int KeyScan(void)
{
    static u8 flag_key = 1;//按键按松开标志
    if (flag_key && KEY_PIN_READ==0) {
        flag_key = 0;
        return 1;	// 按键按下
    } else if (1 == KEY_PIN_READ) {
        flag_key = 1;
        return 0;//无按键按下
    }
    return 0;
}

/**************************************************************************
函数功能：LED接口初始化
入口参数：无 
返回  值：无
**************************************************************************/
void LedInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); //使能端口时钟
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;	          //端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;      //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     //50M
    GPIO_Init(GPIOC, &GPIO_InitStructure);					      //根据设定参数初始化GPIOA 
}

/**************************************************************************
函数功能：LED闪烁
入口参数：闪烁频率 
返回  值：无
**************************************************************************/
void LedBlink(u16 time)
{
    static int temp;
    if (0 == time) {
        LED_PIN_OUT = 0;
    } else if (++temp == time) {
        LED_PIN_OUT = ~LED_PIN_OUT;
        temp = 0;
    }
}

// charging port --------------------------------------------------------------
#define CHARGING_IO_READ PBin(1)

/**************************************************************************
函数功能：充电的IO口初始化
入口参数：无 
返回  值：无
**************************************************************************/
void ChargingPortInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //使能PB端口时钟
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;	            //端口配置  usb检测io
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;         //下拉输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);					      //根据设定参数初始化GPIOB
} 

/**************************************************************************
函数功能：检查充电状态
入口参数：无 
返回  值：1=充电，0=未充电
目		的：当检查USB插入时，进入充电模式。可把电机关闭掉。
                    检测USB的IO口电平变化可由电路原理图得到，参考资料中的电路图
**************************************************************************/
u8 GetChargingStatus(void)
{
    return CHARGING_IO_READ;
}
