/*
 * @Copyright: Copyright (C) 2021-2021 Kevin group. All rights reserved.
 * @Description: the api for the wireless
 * @Author: Kevin
 * @Email: weikaiup@163.com
 * @Date: 2021-01-14
 */
#include "wireless.h"

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_usart.h"
#include "cmsis_os.h"

#include "string.h"
#include "bsp/usart.h"
#include "components/util/util_sys.h"
#include "protocol.h"

#define WL_BUFF_SIZE 256
#define WL_USART_TIMEOUT 100

struct WlMsg {
    uint8_t rxBuf[WL_BUFF_SIZE];    // 缓存区以环形链表的方式读写数据
    uint8_t WritePtr;
    uint8_t readPtr;
};

static UART_HandleTypeDef *g_wlUartHdl = NULL;
static uint8_t g_rxOneByte = 0;    // 用于HAL层中断接收缓存，1个字节
static struct WlMsg g_wlMsg = {0};

uint32_t WirelessInit(void)
{
    if (g_wlUartHdl == NULL) {
        g_wlUartHdl = BspGetUsart3Hdl();
    }

    // 设置缓存区，开启接收中断
    if (g_wlUartHdl != NULL) {
        HAL_UART_Receive_IT(g_wlUartHdl, &g_rxOneByte, 1);
    }

    WirelessSendData("Hello, Virgo", strlen("Hello, Virgo"));
    return RET_OK;
}

uint32_t WirelessSendByte(uint8_t data)
{
    return HAL_UART_Transmit(g_wlUartHdl, &data, 1, WL_USART_TIMEOUT);
}

uint32_t WirelessSendData(uint8_t *data, uint16_t len)
{
    return HAL_UART_Transmit(g_wlUartHdl, data, len, WL_USART_TIMEOUT);
}

void WirelessUartRxCallback(void)
{
    if (g_wlMsg.WritePtr >= WL_BUFF_SIZE) {
        g_wlMsg.WritePtr = 0;
    }
    g_wlMsg.rxBuf[g_wlMsg.WritePtr] = g_rxOneByte;
    g_wlMsg.WritePtr++;
    if (g_wlMsg.WritePtr >= WL_BUFF_SIZE) {
        g_wlMsg.WritePtr = 0;
    }

    HAL_UART_Receive_IT(g_wlUartHdl, &g_rxOneByte, 1);
}

// 蓝牙传输任务，蓝牙地址 AB:37:1A:57:34:02
void WirelessTask(void *argument)
{
    ulog("WirelessTask start");

    uint32_t ret = WirelessInit();
    if (ret != RET_OK) {
        uloge("Wireless Init fail");
        return;
    }

    uint8_t cmdBuf[PROT_CMD_LEN_MAX + 1] = {0};
    uint8_t cmdLen = 0;
    ProtoCmdPrm cmdPrm = {0};
    while (1) {
        while (g_wlMsg.readPtr != g_wlMsg.WritePtr) {
            if (g_wlMsg.readPtr >= WL_BUFF_SIZE) {
                g_wlMsg.readPtr = 0;
                continue;   // 必须退出，如果WritePtr == 0，则实际上没有新数据
            }
            // 如果第一个字符不是$，则丢弃
            if (cmdLen == 0 && g_wlMsg.rxBuf[g_wlMsg.readPtr] != PROT_CMD_START_CHAR) {
                g_wlMsg.readPtr++;
                continue;
            }

            cmdBuf[cmdLen++] = g_wlMsg.rxBuf[g_wlMsg.readPtr++];
            // 接收到#，一条完整命令，送到协议解析器中
            if (cmdBuf[cmdLen - 1] == PROT_CMD_END_CHAR) {
                cmdBuf[cmdLen] = '\0';
                // process cmd
                ret = ParseCmdString(cmdBuf, cmdLen, &cmdPrm);
                if (ret != RET_OK) {
                    uloge("xxxxxxx err xxxxx");
                }
                ulogd("parse:opt=%d, cmdCnt=%d, cmd=%d:%d, cmd=%d:%f", cmdPrm.optType, cmdPrm.cmdCnt,
                    cmdPrm.cmd[0].key, *(uint32_t *)cmdPrm.cmd[0].val, cmdPrm.cmd[1].key, *(float *)cmdPrm.cmd[1].val);
                ulogd("ble rx: %s", cmdBuf);
                cmdLen = 0;
                continue;
            }
            // 命令超过最大长度，丢弃
            if (cmdLen >= PROT_CMD_LEN_MAX) {
                cmdLen = 0;
                continue;
            }
        }
        osDelay(5);
    }
}
