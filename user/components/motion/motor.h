/*
 * @Copyright: Copyright (C) 2021-2021 Kevin group. All rights reserved.
 * @Description: the encoder driver, the PWM for the motors
 * @Author: Kevin
 * @Email: weikaiup@163.com
 * @Date: 2021-01-13
 */
#ifndef BSP_MOTOR_H
#define BSP_MOTOR_H

#include "typedef.h"	

#define ENCODER_TIM_PERIOD (u16)(65535)   //103的定时器是16位 2的16次方最大是65536

enum MotorNum {
    MOTOR_LEFT,
    MOTOR_RIGHT
};

enum MotorDirect {
    MOTOR_STOP,     // 停止
    MOTOR_BRAKE,    // 制动
    MOTOR_FORWARD,  // 正转
    MOTOR_REVERSE   // 反转
};

enum EncoderNum {
    ENCODER_LEFT = 0,
    ENCODER_RIGHT
};

// 读取编码器的值，方向
int16_t GetEncoderVal(uint8_t encoder);
uint8_t GetEncoderDir(uint8_t encoder);

void MotorInit(void);
// 设置电机速度， -7200 ~ 7200
void MotorSetSpeed(uint8_t motor, int16_t speed);

#endif  // BSP_MOTOR_H
