/*
 * @Copyright: Copyright (C) 2021-2021 Kevin group. All rights reserved.
 * @Description: the configuration of the timers.
 * @Author: Kevin
 * @Email: weikaiup@163.com
 * @Date: 2021-01-06
 */
#ifndef __TIM_H__
#define __TIM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_tim.h"

#include "typedef.h"

uint32_t BspTim1Init(void);
uint32_t BspTim2Init(void);
uint32_t BspTim4Init(void);

TIM_HandleTypeDef *GetHandleOfTim1(void);
TIM_HandleTypeDef *GetHandleOfTim2(void);
TIM_HandleTypeDef *GetHandleOfTim4(void);

#ifdef __cplusplus
}
#endif

#endif /* __TIM_H__ */
