/*
 * @Copyright: Copyright (C) 2021-2021 Kevin group. All rights reserved.
 * @Description: the api for the wireless
 * @Author: Kevin
 * @Email: weikaiup@163.com
 * @Date: 2021-01-14
 */

#ifndef COM_WIRELESS_H
#define COM_WIRELESS_H

#include "typedef.h"

uint32_t WirelessInit(void);
void WirelessUartRxCallback(void);

void WirelessTask(void *argument);
uint32_t WirelessSendData(uint8_t *data, uint16_t len);

#endif  // COM_WIRELESS_H
