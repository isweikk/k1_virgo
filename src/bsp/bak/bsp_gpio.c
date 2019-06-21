/**
* @par Copyright (C): 2010-2019, Shenzhen Yahboom Tech
* @file         bsp_gpio.c
* @author       liusen
* @version      V1.0
* @date         2017.07.10    
* @brief        驱动gpio源文件
* @details      
* @par History  见如下说明
*                 
* version:	liusen_20170717
*/

#include "stm32f10x.h"
#include "stm32f10x_pwr.h"
#include "stm32f10x_bkp.h"
#include "bsp_gpio.h"
#include "bsp_motor.h"
#include "bsp_servo.h"
#include "bsp.h"




#if 0
/**
* Function       Colorful_GPIO_Init
* @author        liusen
* @date          2015.01.03    
* @brief         七彩探照灯GPIO初始化接口
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   无
*/
void Colorful_GPIO_Init(void)
{


#ifdef USE_COLOR_RED
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;
	/*开启外设时钟*/
	RCC_APB2PeriphClockCmd(Colorful_Red_RCC, ENABLE); 
	/*选择要控制的引脚*/															   
  	GPIO_InitStructure.GPIO_Pin = Colorful_Red_PIN;	
	/*设置引脚模式为通用推挽输出*/
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	/*设置引脚速率为50MHz */   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	/*调用库函数，初始化PORT*/
  	GPIO_Init(Colorful_Red_PORT, &GPIO_InitStructure);		  
#endif

#ifdef USE_COLOR_GREEN

	/*开启外设时钟*/
	RCC_APB2PeriphClockCmd(Colorful_Green_RCC, ENABLE); 
	/*选择要控制的引脚*/															   
  	GPIO_InitStructure.GPIO_Pin = Colorful_Green_PIN;	
	/*设置引脚模式为通用推挽输出*/
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	/*设置引脚速率为50MHz */   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	/*调用库函数，初始化PORT*/
  	GPIO_Init(Colorful_Green_PORT, &GPIO_InitStructure);
			  
#endif

#ifdef USE_COLOR_BLUE

	/*开启外设时钟*/
	RCC_APB2PeriphClockCmd(Colorful_Blue_RCC, ENABLE); 
	/*选择要控制的引脚*/															   
  	GPIO_InitStructure.GPIO_Pin = Colorful_Blue_PIN;	
	/*设置引脚模式为通用推挽输出*/
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	/*设置引脚速率为50MHz */   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	/*调用库函数，初始化_PORT*/
  	GPIO_Init(Colorful_Blue_PORT, &GPIO_InitStructure);	
#endif
	  
}

/**
* Function       LineWalking_GPIO_Init
* @author        liusen
* @date          2017.07.10    
* @brief         巡线传感器GPIO初始化接口
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   无
*/
void LineWalking_GPIO_Init(void)
{
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;

	PWR_BackupAccessCmd(ENABLE);	//允许修改RTC 和后备寄存器
	RCC_LSEConfig(RCC_LSE_OFF);		//关闭外部低速外部时钟信号功能 后，PC13 PC14 PC15 才可以当普通IO用。
	BKP_TamperPinCmd(DISABLE);		//关闭入侵检测功能，也就是 PC13，也可以当普通IO 使用
	PWR_BackupAccessCmd(DISABLE);	//禁止修改后备寄存器

#ifdef USE_LINE_L1

	/*开启外设时钟*/
	RCC_APB2PeriphClockCmd(LineWalk_L1_RCC | RCC_APB2Periph_AFIO, ENABLE); 
	/*选择要控制的引脚*/															   
  	GPIO_InitStructure.GPIO_Pin = LineWalk_L1_PIN;	
	/*设置引脚模式为通用推挽输出*/
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   
	/*设置引脚速率为50MHz */   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
	/*调用库函数，初始化PORT*/
  	GPIO_Init(LineWalk_L1_PORT, &GPIO_InitStructure);		 
#endif

#ifdef USE_LINE_L2

	/*开启外设时钟*/
	RCC_APB2PeriphClockCmd(LineWalk_L2_RCC | RCC_APB2Periph_AFIO, ENABLE); 
	/*选择要控制的引脚*/															   
  	GPIO_InitStructure.GPIO_Pin = LineWalk_L2_PIN;	
	/*设置引脚模式为通用推挽输出*/
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   
	/*设置引脚速率为50MHz */   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
	/*调用库函数，初始化PORT*/
  	GPIO_Init(LineWalk_L2_PORT, &GPIO_InitStructure);	
#endif

#ifdef USE_LINE_R1

	/*开启外设时钟*/
	RCC_APB2PeriphClockCmd(LineWalk_R1_RCC | RCC_APB2Periph_AFIO, ENABLE); 
	/*选择要控制的引脚*/															   
  	GPIO_InitStructure.GPIO_Pin = LineWalk_R1_PIN;	
	/*设置引脚模式为通用推挽输出*/
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   
	/*设置引脚速率为50MHz */   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
	/*调用库函数，初始化PORT*/
  	GPIO_Init(LineWalk_R1_PORT, &GPIO_InitStructure);	
#endif
	
#ifdef USE_LINE_R2

	/*开启外设时钟*/
	RCC_APB2PeriphClockCmd(LineWalk_R2_RCC, ENABLE); 
	/*选择要控制的引脚*/															   
  	GPIO_InitStructure.GPIO_Pin = LineWalk_R2_PIN;	
	/*设置引脚模式为通用推挽输出*/
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   
	/*设置引脚速率为50MHz */   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	/*调用库函数，初始化PORT*/
  	GPIO_Init(LineWalk_R2_PORT, &GPIO_InitStructure);	
#endif

}

/**
* Function       IRAvoid_GPIO_Init
* @author        liusen
* @date          2017.07.20    
* @brief         红外避障传感器GPIO初始化接口
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   无
*/
void IRAvoid_GPIO_Init(void)
{

#ifdef USE_IRAVOID_L1
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*开启外设时钟*/
	RCC_APB2PeriphClockCmd(IRAvoid_L1_RCC, ENABLE); 
	/*选择要控制的引脚*/															   
  	GPIO_InitStructure.GPIO_Pin = IRAvoid_L1_PIN;	
	/*设置引脚模式为通用推挽输出*/
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   
	/*设置引脚速率为50MHz */   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	/*调用库函数，初始化PORT*/
  	GPIO_Init(IRAvoid_L1_PORT, &GPIO_InitStructure);		 
#endif

#ifdef USE_IRAVOID_R1
	/*开启外设时钟*/
	RCC_APB2PeriphClockCmd(IRAvoid_R1_RCC, ENABLE); 
	/*选择要控制的引脚*/															   
  	GPIO_InitStructure.GPIO_Pin = IRAvoid_R1_PIN;	
	/*设置引脚模式为通用推挽输出*/
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   
	/*设置引脚速率为50MHz */   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	/*调用库函数，初始化PORT*/
  	GPIO_Init(IRAvoid_R1_PORT, &GPIO_InitStructure);	
#endif

}


/**
* Function       LightSeeking_GPIO_Init
* @author        liusen
* @date          2017.07.20    
* @brief         寻光传感器GPIO初始化接口
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   无
*/
void LightSeeking_GPIO_Init(void)
{

#ifdef USE_LIGHTSEEKING_L1
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*开启外设时钟*/
	RCC_APB2PeriphClockCmd(LightSeek_L1_RCC | RCC_APB2Periph_AFIO, ENABLE); 
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);  	     //PB3禁止JTAG
	/*选择要控制的引脚*/															   
  	GPIO_InitStructure.GPIO_Pin = LightSeek_L1_PIN;	
	/*设置引脚模式为通用推挽输出*/
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   
	/*设置引脚速率为50MHz */   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	/*调用库函数，初始化SERVO_4_PORT*/
  	GPIO_Init(LightSeek_L1_PORT, &GPIO_InitStructure);		 
#endif

#ifdef USE_LIGHTSEEKING_R1

	/*开启外设时钟*/
	RCC_APB2PeriphClockCmd(LightSeek_R1_RCC, ENABLE); 
	/*选择要控制的引脚*/															   
  	GPIO_InitStructure.GPIO_Pin = LightSeek_R1_PIN;	
	/*设置引脚模式为通用推挽输出*/
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   
	/*设置引脚速率为50MHz */   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	/*调用库函数，初始化SERVO_4_PORT*/
  	GPIO_Init(LightSeek_R1_PORT, &GPIO_InitStructure);	
#endif

}


/**
* Function       Ultrasonic_GPIO_Init
* @author        liusen
* @date          2017.07.20    
* @brief         超声波传感器GPIO初始化接口
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   无
*/
void Ultrasonic_GPIO_Init(void)
{

#ifdef USE_ULTRASONIC

	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*开启外设时钟*/
	RCC_APB2PeriphClockCmd(TRIG_RCC | RCC_APB2Periph_AFIO, ENABLE); 
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);  //禁能jtag
	/*TRIG触发信号*/														   
  	GPIO_InitStructure.GPIO_Pin = ULTRASONIC_TRIG_PIN;	
	/*设置引脚模式为通用推挽输出*/
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	/*设置引脚速率为50MHz */   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	/*调用库函数，初始化*/
  	GPIO_Init(TRIG_PORT, &GPIO_InitStructure);		 


	/*开启外设时钟*/
	RCC_APB2PeriphClockCmd(ECHO_RCC, ENABLE); 
	/*ECOH回响信号*/															   
  	GPIO_InitStructure.GPIO_Pin = ULTRASONIC_ECHO_PIN;	
	/*设置引脚模式为通用推挽输出*/
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   
	/*设置引脚速率为50MHz */   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	/*调用库函数，初始化PORT*/
  	GPIO_Init(ECHO_PORT, &GPIO_InitStructure);
		
#endif

}

/**
* Function       Buzzer_GPIO_Init
* @author        liusen
* @date          2017.07.20    
* @brief         蜂鸣器GPIO初始化接口
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   无
*/
void Buzzer_GPIO_Init(void)
{

#ifdef USE_BUZZER

	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*开启外设时钟*/
	RCC_APB2PeriphClockCmd(Buzzer_RCC, ENABLE); 
	/*TRIG触发信号*/														   
  	GPIO_InitStructure.GPIO_Pin = Buzzer_PIN;	
	/*设置引脚模式为通用推挽输出*/
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	/*设置引脚速率为50MHz */   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	/*调用库函数，初始化*/
  	GPIO_Init(Buzzer_PORT, &GPIO_InitStructure);		 

#endif

}

/**
* Function       Fire_GPIO_Init
* @author        liusen
* @date          2017.07.20    
* @brief         灭火GPIO初始化接口
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   无
*/
void Fire_GPIO_Init(void)
{

#ifdef USE_FIRE

	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*开启外设时钟*/
	RCC_APB2PeriphClockCmd(FIRE_RCC, ENABLE); 
	/*TRIG触发信号*/														   
  	GPIO_InitStructure.GPIO_Pin = FIRE_PIN;	
	/*设置引脚模式为通用推挽输出*/
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	/*设置引脚速率为50MHz */   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	/*调用库函数，初始化*/
  	GPIO_Init(FIRE_PORT, &GPIO_InitStructure);		 

#endif

}

#endif//结束未使用的借接口

