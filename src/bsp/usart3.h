/**
 * this usart is using to transfer data to bluetooths.
 */
#ifndef __BSP_USRAT3_H__
#define __BSP_USRAT3_H__

#include "sys.h"	  	

void Usart3Init(u32 bound);
void Usart3SendByte(char byte);   //串口发送一个字节
void Usart3SendBuf(char *buf, u16 len);
void Usart3SendStr(char *str);
#endif

