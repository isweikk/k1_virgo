/**
* @par Copyright (C): 2010-2019, Shenzhen Yahboom Tech
* @file         bsp_timer.h	
* @author       liusen
* @version      V1.0
* @date         2017.07.17
* @brief        定时器初始化
* @details      
* @par History  见如下说明
*                 
* version:		liusen_20170717
*/

#ifndef __BSP_TIMER_H__
#define __BSP_TIMER_H__

#include "stm32f10x.h"


extern long g_count; /*用来上报数据*/

void Timer1Iint(u16 arr,u16 psc);


#endif

