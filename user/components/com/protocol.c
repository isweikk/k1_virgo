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
#include "bsp_gpio.h"
#include "bsp_motor.h"
#include "bsp_servo.h"
#include "bsp_timer.h"

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

int ParseString(const char *str, StCmdParam * prm)
{
    int len = strlen(str);
    char proto_str[128] = {0};
    char *kv_tmp = NULL;
    int val = 0;
    int valid_flag = 0;
    
    memcpy(proto_str, str, len + 1);
    
    if ((kv_tmp = strstr(proto_str, "4WD")) != NULL) { //关键字命令
        
        //如果是模式选择，则只解析模式
        if ((kv_tmp = strstr(proto_str, "MODE")) != NULL) {
            val = ParseKV2Val(kv_tmp, "MODE");
            if (val != VAL_ERROR && val > 0) {
                prm->mode = val;
                valid_flag++;
                return EVN_OK;
            }
        }
        //如:$4WD,PTZ180# 舵机转动到180度
        if ((kv_tmp = strstr(proto_str, "PTZ")) != NULL) {
            val = ParseKV2Val(kv_tmp, "PTZ");
            if (val != VAL_ERROR) {
                prm->ptz = val;
                valid_flag++;
            }
        }
        //如:$4WD,CLR255,CLG0,CLB0# 七彩灯亮红色
        if ((kv_tmp = strstr(proto_str, "CLR")) != NULL) {
            val = ParseKV2Val(kv_tmp, "CLR");
            if (val != VAL_ERROR) {
                prm->color = val;
                valid_flag++;
            }
        }
        //
        if (valid_flag) {
            prm->type = 2;
        }
    } else {    //通用控制命令
        //如:$1,0,0,0,0,0,0,0,0,0#    小车前进
        val = atoi(&proto_str[3]);
        if (val > 0) {
            prm->spin = val;
            valid_flag++;
        } else {
            val = atoi(&proto_str[1]);
            prm->steer = val;
            valid_flag++;
        }
        val = atoi(&proto_str[7]);
        if (val > 0) {
            prm->gearbox = val;
            valid_flag++;
        }
        
        if (valid_flag) {
            prm->type = 1;
        }
    }
    return EVN_OK;
}

