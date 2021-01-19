/*
 * @Copyright: Copyright (C) 2021-2021 Kevin group. All rights reserved.
 * @Description: the callback of all uart
 * @Author: Kevin
 * @Email: weikaiup@163.com
 * @Date: 2021-01-14
 */

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_usart.h"

#include "components/util/util_sys.h"
#include "wireless.h"
#include "console.h"

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) {
        ConsoleUartRxCallback();
    } else if (huart->Instance == USART2) {

    } else if (huart->Instance == USART3) {
        WirelessUartRxCallback();
    }
}
