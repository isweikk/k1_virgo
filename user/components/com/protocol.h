/*
 * @Copyright: Copyright (C) 2021-2021 Kevin group. All rights reserved.
 * @Description: 命令协议解析总入口
 * @Author: Kevin
 * @Email: weikaiup@163.com
 * @Date: 2021-01-06
 */

#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

// 运行状态枚举
enum {
  MOTION_STOP = 0,
  MOTION_RUN,
  MOTION_BACK,
  MOTION_LEFT,
  MOTION_RIGHT,
  MOTION_TLEFT,
  MOTION_TRIGHT
} MotionState;

typedef struct {
    int type;   // 0= unvalid cmd, 1 = common command, 2= key-value
    int mode;
    int spin;
    int steer;
    int gearbox;
    int ptz;
    int color;
} MotCmdParam;

int ParseString(const char *str, MotCmdParam *prm);

#endif
