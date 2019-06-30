/**
 * the timers for every app.
 */
#include "timers.h"

volatile u32 VoltageWaitTime = 0;
volatile u32 UltrasonicWaveWaitTime = 0;

//通用定时器中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3
void Timer3_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能

    TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到500为5ms
    TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值  100Khz的计数频率  
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
    
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
    NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
    
    TIM_Cmd(TIM3, ENABLE);  //使能TIMx外设
}

/**
 * this timer is triggered every 5 ms
 */
void TIM3_IRQHandler(void)   //TIM3中断
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) {//检查指定的TIM中断发生与否:TIM 中断源 
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //清除TIMx的中断待处理位:TIM 中断源
        
        if (VoltageWaitTime < 65535) {
            VoltageWaitTime += 5;
        }
        if (UltrasonicWaveWaitTime < 65535) {
            UltrasonicWaveWaitTime += 5;
        }
    }
}

void CommonTimerInit(void)
{
    Timer3_Init(499,719);
}
