/*
 * @Copyright: Copyright (C) 2021-2021 Kevin group. All rights reserved.
 * @Description: file content
 * @Author: Kevin
 * @Email: weikaiup@163.com
 * @Date: 2021-01-06
 */

#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_usart.h"
#include "typedef.h"

UART_HandleTypeDef *BspGetUsart1Hdl(void);
UART_HandleTypeDef *BspGetUsart2Hdl(void);
UART_HandleTypeDef *BspGetUsart3Hdl(void);

uint32_t BspUsart1Init(void);
uint32_t BspUsart2Init(void);
uint32_t BspUsart3Init(void);

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */
