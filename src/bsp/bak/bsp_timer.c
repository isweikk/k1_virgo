/**
* @par Copyright (C): 2010-2019, Shenzhen Yahboom Tech
* @file         bsp_timer.h	
* @author       liusen
* @version      V1.0
* @date         2017.07.17
* @brief        定时器
* @details      
* @par History  见如下说明
*                 
* version:		liusen_20170717
*/

#include "bsp_timer.h"
#include "bsp_servo.h"
#include "bsp.h"



/**
* Function       Timer1Iint
* @author        liusen
* @date          2015.01.03    
* @brief         定时器1初始化接口
* @param[in]     arr：自动重装值。psc：时钟预分频数
* @param[out]    void
* @retval        void
* @par History   这里时钟选择为APB1的2倍，而APB1为36M
*/
void Timer1Iint(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); //时钟使能
	
	//定时器TIM1初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler = (psc-1); //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim   //36Mhz
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;    //重复计数关闭
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE ); //使能指定的TIM1中断,允许更新中断

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;  //TIM1中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器

	TIM_Cmd(TIM1, ENABLE);  //使能TIMx					 
}
/**
* Function       TIM1_UP_IRQHandler
* @author        liusen
* @date          2015.01.03    
* @brief         定时器1中断服务程序: 主要控制6路舵机运行，100Khz的计数频率，计数到10为10us 
* @param[in]     arr：自动重装值。psc：时钟预分频数
* @param[out]    void
* @retval        void
* @par History   这里时钟选择为APB1的2倍，而APB1为36M
*/
int kTim1IrqCnt = 0;	 /*记录中断次数，控制舵机PWM时间*/
long g_count = 0; /*用来上报数据*/

void TIM1_UP_IRQHandler(void)   //TIM1中断
{
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)  //检查更新中断发生与否
	{
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);  //清除TIMx更新中断标志 
		kTim1IrqCnt++;
		g_count++;
	
		#ifdef USE_SERVO_J1
		if (kTim1IrqCnt * 10 <= ServoGetPw(kEmServoID1)) {
			GPIO_SetBits(SERVO_1_PORT, SERVO_1_PIN);
		} else {
			GPIO_ResetBits(SERVO_1_PORT, SERVO_1_PIN);
		}
		#endif	   	

		#ifdef USE_SERVO_J2
		if (kTim1IrqCnt * 10 <= ServoGetPw(kEmServoID2)) {
			GPIO_SetBits(SERVO_2_PORT, SERVO_2_PIN);
		} else {
			GPIO_ResetBits(SERVO_2_PORT, SERVO_2_PIN);
		}
		#endif

		#ifdef USE_SERVO_J3
        if (kTim1IrqCnt * 10 <= ServoGetPw(kEmServoID3)) {
			GPIO_SetBits(SERVO_3_PORT, SERVO_3_PIN);
		} else {
			GPIO_ResetBits(SERVO_3_PORT, SERVO_3_PIN);
		}
		#endif

		#ifdef USE_SERVO_J4
		if (kTim1IrqCnt * 10 <= ServoGetPw(kEmServoID4)) {
			GPIO_SetBits(SERVO_4_PORT, SERVO_4_PIN);
		} else {
			GPIO_ResetBits(SERVO_4_PORT, SERVO_4_PIN);
		}
		#endif

		#ifdef USE_SERVO_J5
		if (kTim1IrqCnt * 10 <= ServoGetPw(kEmServoID5)) {
			GPIO_SetBits(SERVO_5_PORT, SERVO_5_PIN);
		} else {
			GPIO_ResetBits(SERVO_5_PORT, SERVO_5_PIN);
		}
		#endif

		#ifdef USE_SERVO_J6
		if (kTim1IrqCnt * 10 <= ServoGetPw(kEmServoID6)) {
			GPIO_SetBits(SERVO_6_PORT, SERVO_6_PIN);
		} else {
			GPIO_ResetBits(SERVO_6_PORT, SERVO_6_PIN);
		}
		#endif
		
		if(kTim1IrqCnt == 2000) {//2000*10us = 20ms 周期
			kTim1IrqCnt = 0;
		}		
	}
}
