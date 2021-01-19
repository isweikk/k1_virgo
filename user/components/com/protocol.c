/*
 * @Copyright: Copyright (C) 2021-2021 Kevin group. All rights reserved.
 * @Description: 协议解析总入口
 * @Author: Kevin
 * @Email: weikaiup@163.com
 * @Date: 2021-01-06
 */

#include "protocol.h"

#include <stdlib.h>
#include <string.h>

#include "typedef.h"
// #include "bsp_gpio.h"
// #include "bsp_motor.h"
// #include "bsp_servo.h"
// #include "bsp_timer.h"

#define 	MOT_RUN_CHAR     '1' // 按键前
#define 	MOT_BACK_CHAR    '2' // 按键后
#define 	MOT_LEFT_CHAR    '3' // 按键左
#define 	MOT_RIGHT_CHAR   '4' // 按键右
#define 	MOT_STOP_CHAR    '0' // 按键停

int g_motionState = MOTION_STOP; //  1前2后3左4右0停止

int ParseKV2Val(const char *kv_str, const char *key)
{
    char val[32] = {0};
    char *ptr = NULL;
    int i = strlen(key);
    
    if ((ptr = strchr(kv_str, ',')) != NULL
        || (ptr = strchr(kv_str, '#')) != NULL) {
        strncpy(val, kv_str + i, ptr - (kv_str + i));
    } else {
        return VAL_ERROR;
    }
    i = atoi(val);
    return i;
}

CmdKeyword CmdKeywordMap[] = {
    { "MODE", CMD_VAL_INT },
    { "PID", CMD_VAL_FLOAT },
};

uint32_t ParseOptTypeFromCmdStr(const char *cmdStr, uint32_t len, uint8_t *optType)
{
    // 格式"A00" "A01" "A02"
    if (len != 3) {
        return RET_ERR;
    }

    uint8_t key1 = cmdStr[0];
    uint8_t key2 = (cmdStr[1] - '0') * 10 + cmdStr[2] - '0';
    if (key2 >= CMD_OPT_BUTT) {
        return RET_ERR;
    }
    
    *optType = key2;
    return RET_OK;
}

uint32_t ParseKeyword(const char *keyword, uint8_t len, uint16_t *key)
{
    uint16_t keywordMax = sizeof(CmdKeywordMap) / sizeof(CmdKeyword);
    for (uint16_t i = 0; i < keywordMax; i++) {
        if (strncmp(keyword, CmdKeywordMap[i].keyword, len) == 0) {
            *key = i;
            return RET_OK;
        }
    }
    return RET_ERR;
}

uint32_t ParseKeyValFromCmdStr(const char *cmdStr, uint32_t len, uint16_t *key, uint32_t *val)
{
    char keyword[16] = {0};
    char valStr[16] = {0};
    char *ptr = NULL;

    if ((ptr = strchr(cmdStr, PROT_KEY_VAL_SPLIT_CHAR)) == NULL) {
        return VAL_ERROR;
    }

    if (strncpy(keyword, cmdStr, ptr - cmdStr) == NULL) {
        return VAL_ERROR;
    }

    if (strncpy(valStr, ptr + 1, len - (ptr - cmdStr)) == NULL) {
        return VAL_ERROR;
    }

    uint32_t ret = ParseKeyword(keyword, ptr - cmdStr, key);
    if (ret != RET_OK) {
        return ret;
    }
    
    if (CmdKeywordMap[*key].valType == CMD_VAL_INT) {
        *val = atoi(valStr);
    } else {
        *(float *)val = atof(valStr);
    }
    return RET_OK;
}

int ParseCmdString(const uint8_t *cmdStr, uint32_t len, ProtoCmdPrm *cmd)
{
    char protoStr[PROT_CMD_LEN_MAX] = {0};
    char *ptr = NULL;
    char *kvTmp = NULL;
    
    cmd->cmdCnt = 0;
    cmd->optType = CMD_OPT_BUTT;
    memcpy(protoStr, cmdStr, len + 1);

    // 解析操作类型 "$A01,"，取出A01
    kvTmp = protoStr + 1;   // 跳过起始符
    if ((ptr = strchr(kvTmp, ',')) == NULL) {
        return VAL_ERROR;
    }
    uint32_t ret = ParseOptTypeFromCmdStr(kvTmp, ptr - kvTmp, &cmd->optType);
    if (ret != RET_OK) {
        return ret;
    }
    kvTmp = ptr + 1;

    // 循环解析数据单元
    for (uint8_t i = 0; i < PROT_CMD_UNIT_MAX; i++) {
        if ((ptr = strchr(kvTmp, ',')) != NULL || (ptr = strchr(kvTmp, '#')) != NULL) {
            ret = ParseKeyValFromCmdStr(kvTmp, ptr - kvTmp, &cmd->cmd[i].key, (uint32_t *)cmd->cmd[i].val);
            if (ret != RET_OK) {
                return ret;
            }
            cmd->cmdCnt++;

            if (*ptr == '#') {
                break;
            }
            kvTmp = ptr + 1;
        } else {
            return RET_ERR;
        }
    }

    return RET_OK;
}

