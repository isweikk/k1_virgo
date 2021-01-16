/*
 * @Copyright: Copyright (C) 2021-2021 Kevin group. All rights reserved.
 * @Description: motion controller
 * @Author: Kevin
 * @Email: weikaiup@163.com
 * @Date: 2021-01-08
 */
#ifndef MOTION_CONTROLLER_H
#define MOTION_CONTROLLER_H

#include "typedef.h"

#define PI 3.14159265

enum {
    BODY_STOP = 0,
    BODY_GO_FORWARD,
    BODY_GO_BACKWARD,
    BODY_TURN_LEFT,
    BODY_TURN_RIGHT 
};
enum EnumBodyPosture{
    POSTURE_STAND = 0,  //立正
    POSTURE_FALL,
    POSTURE_HANG
};

extern u8 MoveDirection;
extern u8 BodyPosture;

void MotorGetConstEuler(float *out_pitch, float *out_roll, float *out_yaw);

void EXTI9_5_IRQHandler(void);
int BalanceKeepUp(float pitch, float mid_val, float gyro);
int VelocityKeep(int encoder_left,int encoder_right, uint32_t timeMs);
int TurnKeep(int encoder_left, int encoder_right, float gyro);

void MotionTask(void *argument);

#endif  // MOTION_CONTROLLER_H
