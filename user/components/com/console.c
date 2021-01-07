/*
 * @Copyright: Copyright (C) 2021-2021 Kevin group. All rights reserved.
 * @Description: 控制台实现代码
 * @Author: Kevin
 * @Email: weikaiup@163.com
 * @Date: 2021-01-06
 */

#include "console.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bsp/usart.h"

// #define Uart1PrintUsage(a, b) ulog("usage:%s %s", a, b)
// typedef struct {
//     u8 num;
//     u8 cmd[16];
//     u8 info[48];
//     u8 prmNum;
// } StConsoleCmd;

// const StConsoleCmd kConsoleCmdTab[] = {
//     {0, "help", "[cmd][parm1][parm1]...", 1},

// };

// u8 kUart1RxBuf[UART1_BUFFER_SIZE];  //
// // recieve status
// // bit15,	recieve ok flag,0x0d
// // bit14,	recieve 0x0a
// // bit13~0,	the data length
// u16 kUart1RxFlag = 0;
// u16 kUart1EchoPtr = 0;

// int String2Int(u8 *string)
// {
//     u32 val = 0;
//     u8 ptr = 0;
//     while (1) {
//         if (*(string + ptr) == NULL) {
//             return val;
//         } else if ((*(string + ptr) >= '0') && (*(string + ptr) <= '9')) {
//             val *= 10;
//             val += (*(string + ptr)) - '0';
//             ptr++;
//         } else {
//             return EVN_ERROR;
//         }
//     }
// }

// int Uart1ConsoleStrHdl(const u8 *string)
// {
//     u8 cmd_num = 0, cmd_ptr = 0;
//     u8 is_cmd_ok = 0;
//     u8 prm_num, prm_list[5][12] = {0};  // 5 parameter, 11 byte mostest int every one.
//     u8 i;

//     if ((*string < 'a') || (*string > 'z'))  // limit the first word.
//         return EVN_ERROR;
//     for (; cmd_num < 100; cmd_num++) {
//         if (*string != kConsoleCmdTab[cmd_num].cmd[0]) {
//             continue;
//         } else {
//             if ((strncmp((char *)string, (char *)kConsoleCmdTab[cmd_num].cmd,
//                          strlen((char *)kConsoleCmdTab[cmd_num].cmd)) == NULL)) {
//                 if ((*(string + strlen((char *)kConsoleCmdTab[cmd_num].cmd)) != ' ')  // only partial same. not right
//                     && (*(string + strlen((char *)kConsoleCmdTab[cmd_num].cmd)) != NULL)) {
//                     continue;
//                 } else {
//                     is_cmd_ok = 1;
//                     break;
//                 }
//             } else {
//                 continue;
//             }
//         }
//     }
//     if (is_cmd_ok == 0)
//         return EVN_ERROR;
//     cmd_ptr += strlen((char *)kConsoleCmdTab[cmd_num].cmd);
//     for (; prm_num < 5;) {
//         while (*(string + cmd_ptr) == ' ') {  // ignore ' '
//             cmd_ptr++;
//         }
//         i = 0;
//         while ((*(string + cmd_ptr) != ' ') && (*(string + cmd_ptr) != NULL)) {
//             prm_list[prm_num][i++] = *(string + cmd_ptr);
//             cmd_ptr++;
//             if (i > 12) {
//                 Uart1PrintUsage(kConsoleCmdTab[cmd_num].cmd, kConsoleCmdTab[cmd_num].info);
//                 return EVN_ERROR;
//             }
//         }
//         if (i > 0)
//             prm_num++;
//         if (*(string + cmd_ptr) == NULL)
//             break;
//     }
//     if (prm_num != kConsoleCmdTab[cmd_num].prmNum) {
//         Uart1PrintUsage(kConsoleCmdTab[cmd_num].cmd, kConsoleCmdTab[cmd_num].info);
//         return EVN_ERROR;
//     }

//     switch (cmd_num) {
//         case 0:
//             // tmp_m0 = String2Int(prm_list[0]);
//             // MotorSetPos(0, tmp_m0);
//             break;
//         case 1:
//             break;
//         default:
//             break;
//     }

//     return EVN_OK;
// }

// void Uart1ConsoleTask(void)
// {
//     if (kUart1EchoPtr < (kUart1RxFlag & 0x3fff)) {  // have not echoed over.
//         for (; kUart1EchoPtr < (kUart1RxFlag & 0x3fff); kUart1EchoPtr++) {
//             Uart1SendByte(kUart1RxBuf[kUart1EchoPtr]);
//         }
//     } else if ((kUart1RxFlag & 0x8000) > 0) {  // receive ok
//         printf("\n");
//         if (Uart1ConsoleStrHdl(kUart1RxBuf) == RET_OK) {
//             printf(" <ok>\n");
//         } else if ((kUart1RxFlag & 0x3fff) > 0) {
//             printf(" <error>\n");
//         }
//         printf("stm32: ");
//         kUart1RxFlag = 0;
//         kUart1EchoPtr = 0;
//     }
// }

#define CONSOLE_BUFF_SIZE 256
#define CONSOLE_TIMEOUT 100
#define RX_FLAG_COMPLETE 0x8000
#define RX_FLAG_HAVE_ROLL_BACK 0X4000
#define RX_FLAG_DATA_LEN_MASK 0X3FFF
#define RX_ASCII_BACKSPACE 0X08 // <--
#define RX_ASCII_ROLL_BACK 0X0D // \r
#define RX_ASCII_NEW_LINE  0X0A // \n

UART_HandleTypeDef *g_uartHdl = NULL;
uint8_t g_rxBuf[CONSOLE_BUFF_SIZE] = {0};
uint8_t g_rxOneByte = 0;    // 用于HAL层中断接收缓存，1个字节
uint16_t g_rxFlag = 0;

uint8_t IsRxComplete(void)
{
    return ((g_rxFlag & RX_FLAG_COMPLETE) > 0);
}

void SetRxComplete(void)
{
    g_rxFlag |= RX_FLAG_COMPLETE;
}

uint8_t IsRxHaveRollBack(void)
{
    return ((g_rxFlag & RX_FLAG_HAVE_ROLL_BACK) > 0);
}

void SetRxHaveRollBack(void)
{
    g_rxFlag |= RX_FLAG_HAVE_ROLL_BACK;
}

uint16_t GetRxLen(void)
{
    return (g_rxFlag & RX_FLAG_DATA_LEN_MASK);
}

void ConsoleInit(void)
{
    if (g_uartHdl == NULL) {
        g_uartHdl = BspGetUsart1Hdl();
    }

    // 设置缓存区，开启接收中断
    if (g_uartHdl != NULL) {
        HAL_UART_Receive_IT(g_uartHdl, &g_rxOneByte, 1);
    }
}

void ConsoleSendByte(uint8_t data)
{
    HAL_UART_Transmit(g_uartHdl, &data, 1, CONSOLE_TIMEOUT);
}

void ConsoleSendData(uint8_t *data, uint16_t len)
{
    HAL_UART_Transmit(g_uartHdl, data, len, CONSOLE_TIMEOUT);
}

// 数据必须是0x0d 0x0a结尾，linux以0x0a结尾
// console处理策略，是一行接收完整，则开始处理命令
// 如果超出缓冲区大小，则回显回车
// 这是中断回调处理，有发送函数可能会影响效率，考虑重构
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    uint16_t rxLen = GetRxLen();
    if (g_rxOneByte == RX_ASCII_BACKSPACE) {
        // receive one byte at least.
        if (rxLen > 0) {
            g_rxFlag--;
            rxLen--;
            g_rxBuf[rxLen] = 0;
            ConsoleSendByte(RX_ASCII_BACKSPACE);
        }
    } else if ((g_rxOneByte == RX_ASCII_ROLL_BACK) || (g_rxOneByte == RX_ASCII_NEW_LINE) || ((g_rxOneByte >= ' ') && (g_rxOneByte <= '~'))) {
        if (IsRxComplete() == 0) { // 接收未完成
            if (IsRxHaveRollBack()) {    // 接收到了\r
                if (g_rxOneByte != RX_ASCII_NEW_LINE) {
                    g_rxFlag = 0;   // 接收错误,重新开始
                } else {
                    g_rxBuf[rxLen] = 0;  // end char
                    SetRxComplete(); // 接收完成了
                }
            } else {    // 还没收到\r
                if (g_rxOneByte == RX_ASCII_ROLL_BACK) {
                    SetRxHaveRollBack();  // 当前收到\r
                } else if (g_rxOneByte == RX_ASCII_NEW_LINE) {
                    g_rxBuf[rxLen] = 0;  // end char
                    SetRxComplete();                  // 收到\n，完成接收
                } else {
                    g_rxBuf[rxLen++] = g_rxOneByte;
                    g_rxFlag++;
                    if (rxLen > (CONSOLE_BUFF_SIZE - 1)) {  // 超出最大size
                        g_rxFlag = 0;
                        ConsoleSendByte(RX_ASCII_ROLL_BACK);
                        ConsoleSendByte(RX_ASCII_NEW_LINE);
                    } else {
                        ConsoleSendByte(g_rxBuf[rxLen - 1]);
                    }
                }
            }
        }
    }
    if (g_rxFlag & RX_FLAG_COMPLETE) {    // 接收完成，调用处理函数
        ConsoleSendData("[OK]\r\n", 6);
        g_rxFlag = 0;
    }
    HAL_UART_Receive_IT(g_uartHdl, &g_rxOneByte, 1);
}

/*为确保没有从 C 库链接使用半主机的函数，因为不使用半主机，标准C库stdio.h中
有些使用半主机的函数要重新写,您必须为这些函数提供自己的实现*/
// support for using the function of "printf",then no need to use MicroLIB
#if 1
#pragma import(__use_no_semihosting)  //确保没有从C库链接使用半主机的函数
struct __FILE {                       //标准库需要的支持函数
    int handle;
};

FILE __stdout;
int _sys_exit(int x)
{  //避免使用半主机模式
    x = x;
    return 0;
}
// modifing define the fputc
int fputc(int ch, FILE *f)
{
    HAL_UART_Transmit(g_uartHdl, (uint8_t *)&ch, 1, 0xffff);
    return ch;
}

int fgetc(FILE *f)
{
    uint8_t ch = 0;
    HAL_UART_Receive(g_uartHdl, &ch, 1, 0xffff);
    return ch;
}
#endif
