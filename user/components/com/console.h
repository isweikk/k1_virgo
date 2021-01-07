/*
 * @Copyright: Copyright (C) 2021-2021 Kevin group. All rights reserved.
 * @Description: the api for the console
 * @Author: Kevin
 * @Email: weikaiup@163.com
 * @Date: 2021-01-06
 */

#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#define DEBUG   1

#define ulog(format, ...) printf("> %-4d: "format"\r\n", __LINE__, ##__VA_ARGS__)
#if DEBUG
#define ulogd(format, ...) printf("> %-4d: "format"\r\n", __LINE__, ##__VA_ARGS__)
#else
#define ulogd(format, ...)
#endif

void ConsoleInit(void);

#endif
