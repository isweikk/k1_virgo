/*
 * @Copyright: Copyright (C) 2021-2021 Kevin group. All rights reserved.
 * @Description: 命令协议解析总入口
 * @Author: Kevin
 * @Email: weikaiup@163.com
 * @Date: 2021-01-06
 */

#ifndef COM_PROTOCOL_H
#define COM_PROTOCOL_H

#include "typedef.h"

#define PROT_CMD_LEN_MAX  128   // 一条命令字符串长度
#define PROT_CMD_UNIT_MAX  10   // 一条命令中数据单元最大值
#define PROT_CMD_START_CHAR '$' // 起始符
#define PROT_CMD_END_CHAR '#'   // 结束符
#define PROT_KEY_VAL_SPLIT_CHAR ':'   // 键值对分割符

// 运行状态枚举
enum MotionState {
  MOTION_STOP = 0,
  MOTION_RUN,
  MOTION_BACK,
  MOTION_LEFT,
  MOTION_RIGHT,
  MOTION_TLEFT,
  MOTION_TRIGHT
};

// 命令操作类型
enum CmdOptType {
    CMD_OPT_RSP,
    CMD_OPT_SET,
    CMD_OPT_GET,
    CMD_OPT_BUTT
};

// 数据关键字索引
enum CmdKeyType {
    CMD_KEY_MODE,
};

// 数据类型
enum CmdValType {
    CMD_VAL_INT,
    CMD_VAL_FLOAT
};

// 数据关键字字符串，内部使用，定义协议关键字，通过CmdKeyType索引，所以两者需对应
typedef struct {
    char *keyword;
    uint8_t valType;
} CmdKeyword;

// 数据单元结构，支持int和float
typedef struct {
    uint16_t key;
    uint8_t val[4]; // 4字节数据空间
} ProtoCmdUnit;

typedef struct {
    uint8_t optType;
    uint8_t cmdCnt;
    ProtoCmdUnit cmd[PROT_CMD_UNIT_MAX];
} ProtoCmdPrm;

int ParseCmdString(const uint8_t *str, uint32_t len, ProtoCmdPrm *cmd);

#endif
