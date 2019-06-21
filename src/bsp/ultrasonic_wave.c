/**
 * the driver of the ultrasonic wav 
 */
#include "ultrasonic_wave.h"
#include "bsp.h"

#define	ULTRASONIC_TRIG_PIN       GPIO_Pin_3   //TRIG       
#define	ULTRASONIC_ECHO_PIN       GPIO_Pin_2		//ECHO   

float UltrasonicWaveDistance = 0;                 //超声波测距
/*
 * 函数名：UltrasonicWaveInit
 * 描述  ：超声波模块的初始化
 * 输入  ：无
 * 输出  ：无	
 */
void UltrasonicWaveInit(void)
{
#ifdef USE_ULTRASONIC
    GPIO_InitTypeDef GPIO_InitStructure;	
    EXTI_InitTypeDef EXTI_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;					 //PA3接TRIG
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		     //设为推挽输出模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	         
    GPIO_Init(GPIOA, &GPIO_InitStructure);	                 //初始化外设GPIO 

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;				     //PA3接ECH0
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;		 //设为输入
    GPIO_Init(GPIOA,&GPIO_InitStructure);						 //初始化GPIOA

    //GPIOA.2	  中断线以及中断初始化配置
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource2);

    EXTI_InitStructure.EXTI_Line=EXTI_Line2;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);		//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
#endif
}


void EXTI2_IRQHandler(void)
{
    DelayUs(10);		                      //延时10us
    if (EXTI_GetITStatus(EXTI_Line2) != RESET) {
        TIM_SetCounter(TIM3,0);																		
        TIM_Cmd(TIM3, ENABLE);                               				//开启时钟
        while(GPIO_ReadInputDataBit(GPIOA,ULTRASONIC_ECHO_PIN));	       				//等待低电平
        TIM_Cmd(TIM3, DISABLE);			                         				//定时器3失能
        UltrasonicWaveDistance = TIM_GetCounter(TIM3)*1.7;						//计算距离&&UltrasonicWaveDistance<150

        //	if(UltrasonicWaveDistance>0)
        //	{
        //		printf("distance:%f cm",UltrasonicWaveDistance);
        //	}
        //		
        EXTI_ClearITPendingBit(EXTI_Line2);  //清除EXTI2线路挂起位
    }

}

/*
 * 函数名：UltrasonicWave_StartMeasure
 * 描述  ：开始测距，发送一个>10us的脉冲，然后测量返回的高电平时间
 * 输入  ：无
 * 输出  ：无	
 */
void UltrasonicWaveDetect(void)
{
#ifdef USE_ULTRASONIC
    GPIO_SetBits(GPIOA,ULTRASONIC_TRIG_PIN); 		  //送>10US的高电平RIG_PORT,TRIG_PIN这两个在define中有说
    DelayUs(20);		                      //延时20US
    GPIO_ResetBits(GPIOA,ULTRASONIC_TRIG_PIN);
#endif
}

u8 UltrasonicWaveWarning(void)
{
#ifdef USE_ULTRASONIC
    if (UltrasonicWaveDistance < 10 
        && UltrasonicWaveDistance > 3) {
        return 1;
    }
#endif
    return 0;
}
