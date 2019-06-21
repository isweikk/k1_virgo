/*
 *the Peripherals use to communicate data.
 */

#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include "sys.h"

#define UART1_LINUX_MODE        1
#define UART1_BUFFER_SIZE       256
#define DEBUG   1

#define ulog(format, ...) printf("> %-4d: "format"\r\n", __LINE__, ##__VA_ARGS__)
#if DEBUG
#define ulogd(format, ...) printf("> %-4d: "format"\r\n", __LINE__, ##__VA_ARGS__)
#else
#define ulogd(format, ...)
#endif

extern u8 kUart1RxBuf[UART1_BUFFER_SIZE];
extern u16 kUart1RxFlag;

int Uart1ConsoleStrHdl(const u8 *string);
void Uart1ConsoleTask(void);
void Uart1Init(u32 bound);
void Uart1SendByte(u8 data);


#endif
