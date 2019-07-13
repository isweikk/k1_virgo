/*
 * @Descripttion: 
 * @version: v1.0.1
 * @Author: Kevin
 * @Email: kkcoding@qq.com
 * @Date: 2019-06-15 02:37:13
 * @LastEditors: Kevin
 * @LastEditTime: 2019-07-14 01:16:26
 */

#include "sys.h"
#include "bsp.h"
#include "mpu6050/mpu6050.h"
#include "motion_control.h"
#include "event_manager.h"
#include "data_scope_service.h"

/****************************上位机变量*********************************/   
unsigned char i;          	//计数变量
unsigned char Send_Count; 	//串口需要发送的数据个数
/**********************************************************************/

int DeviceInit(void)
{
    Uart1Init(115200);  //only use for print, you can use "ulog" and "ulogd" to print log.
    LedInit();
    KeyInit();
    ChargingPortInit();
    AdcInit();
    EncoderInit();
    OLED_Init();

    DelayMs(100);
/*****************修改蓝牙的默认通信波特率以及蓝牙默认的名字******************/
    Usart3Init(9600);              //蓝牙默认通信波特率9600
    Usart3SendStr("AT\r\n");
    Usart3SendStr("AT+NAMEK1\r\n");//发送蓝牙模块指令--设置名字
    DelayMs(100);
    Usart3SendStr("AT+BAUD8\r\n"); //发送蓝牙模块指令,将波特率设置成115200
    DelayMs(100);
    Usart3Init(115200);
/*****************************************************************************/		
    //OLED_Init();
    
    MPU_Init();					    //=====初始化MPU6050			 
    //UltrasonicWaveInit();         //=====初始化超声波的硬件IO口
    //DelayMs(1000);
    MotorInit();
    MPU6050_EXTI_Init();			//=====MPU6050 5ms定时中断初始化
    CommonTimerInit();  //通用定时器，5ms时间片

    return 0;
}

int main(void)	
{
    SysConfig();
    DelayInit();
    
    DeviceInit();
    
    ulog("system init ok");
    float pitch, roll, yaw;
    
    while (1) {
        EventLoop();
        DelayMs(5);
        KEY_mode_Select();				 //=====模式选择以及屏幕表情显示	
/*****************将下面的注释去掉即可将 pitch、yaw 输出******************/
        MotorGetConstEuler(&pitch, &roll, &yaw);
        DataScope_Get_Channel_Data(pitch, 1 );
        DataScope_Get_Channel_Data(roll, 2 );
        DataScope_Get_Channel_Data(yaw, 3 );
        Send_Count=DataScope_Data_Generate(3);
        for (i = 0 ; i < Send_Count; i++) {
            while((USART1->SR&0X40)==0);  
            USART1->DR = DataScope_OutPut_Buffer[i]; 
        }
        DelayMs(50); //20HZ 
/*************************************************************************/		
    }
}



