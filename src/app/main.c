
#include "sys.h"
#include "bsp.h"
#include "mpu6050/mpu6050.h"
#include "motion_control.h"
#include "event_manager.h"
#include "data_scope_service.h"

/****************************全局变量*************************************/    

u8    key=0;								 									 //按键的键值
u8    KEY_MODE=0;					 										 //模式0,显示名字===HELLO BLIZ
                                                                                             //模式1,电量模式
                                                                                             //模式2,表情模式
                                                                                             //模式3,参数模式
u8 DIS_STATE=255;				 											 //用来实现只刷一次屏幕的变量			
/***********************************************************************/

/****************************上位机变量*********************************/   
unsigned char i;          										 //计数变量
unsigned char Send_Count; 										 //串口需要发送的数据个数
/**********************************************************************/

int DeviceInit(void)
{
    CommonTimerInit();  //通用定时器，5ms中断
    Uart1Init(115200);  //only use for print, you can use "ulog" and "ulogd" to print log.
    LedInit();
    KeyInit();
    ChargingPortInit();
    AdcInit();                    //=====初始化ADC
    EncoderInit();           //=====初始化编码器

    DelayMs(100);
/*****************修改蓝牙的默认通信波特率以及蓝牙默认的名字******************/
    Usart3Init(9600);              //=====串口3初始化成能与蓝牙进行通信的波特率,蓝牙默认通信波特率9600
    Usart3SendStr("AT\r\n");
    Usart3SendStr("AT+NAMEPico\r\n");//发送蓝牙模块指令--设置名字为：Bliz
    DelayMs(100);
    Usart3SendStr("AT+BAUD8\r\n"); 		 //发送蓝牙模块指令,将波特率设置成115200
    DelayMs(100);
    Usart3Init(115200);            //=====初始化串口3
/*****************************************************************************/		
    //OLED_Init();
    
    MPU_Init();					    			 //=====初始化MPU6050			 
    UltrasonicWaveInit();   //=====初始化超声波的硬件IO口
    DelayMs(2000);								 //=====延时2s 解决小车上电轮子乱转的问题
    MotorInit();									 //=====初始化与电机连接的硬件IO/timer
    MPU6050_EXTI_Init();					 //=====MPU6050 5ms定时中断初始化
    return 0;
}

///开机显示需要显示的，只需要显示一次即可。减少CPU的运算。/////////////////
void oled_first_show(void)
{
    OLED_ShowString(0,0,"ANGLE:",12);
    OLED_ShowString(0,2,"BAT :",12);
    OLED_ShowString(90,2,"V",12);
    OLED_ShowString(0,4,"Distance:",12);
}

void oled_show(void)
{
    OLED_Float(0,48,0,3);							//显示角度
    OLED_Float(2,48,GetBatteryVolt(),3);						//显示电压
//		OLED_Num2(4,4,EncoderRightCnt);					//显示右边电机的编码器值
//		OLED_Num2(14,4,EncoderLeftCnt);					//显示左边电机的编码器值
    OLED_Float(4,70,UltrasonicWaveDistance,3);						//显示超声波的距离
    if (GetChargingStatus()==1) {
        OLED_ShowString(0,6,"Charging...",12);
    }
    if (GetChargingStatus()==0) {
        OLED_ShowString(0,6,"            ",12);
    }
}


int main(void)	
{
    SysConfig();
    DelayInit();
    
    DeviceInit();
    
    ulog("system init ok");

    while (1) {
        EventLoop();
        DelayMs(5);
        KEY_mode_Select();				 //=====模式选择以及屏幕表情显示	
/*****************将下面的注释去掉即可将 pitch、yaw 输出******************/
//			DataScope_Get_Channel_Data(pitch, 1 );
//			DataScope_Get_Channel_Data(yaw, 2 );
//			DataScope_Get_Channel_Data(roll, 3 );
//			Send_Count=DataScope_Data_Generate(3);
//			for( i = 0 ; i < Send_Count; i++) 
//			{
//			while((USART1->SR&0X40)==0);  
//			USART1->DR = DataScope_OutPut_Buffer[i]; 
//			}
//			DelayMs(50); //20HZ 
/*************************************************************************/		
    }
}



