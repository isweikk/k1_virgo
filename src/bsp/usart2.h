/*
 *the Peripherals use to communicate data.
 */

#ifndef __BSP_USART2_H__
#define __BSP_USART2_H__

#include "sys.h"

//uart2
void Uart2Init(u32 bound);
void Uart2SendByte(u8 data);
u8 U2GetCmdFlag(void);
void U2ClearCmdFlag(void);
u16 U2GetCmdData(u8 *buf);

#endif
