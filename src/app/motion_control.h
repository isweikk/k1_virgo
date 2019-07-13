/**
 * motion controller
 */
#ifndef __MOTION_CONTROL_H__
#define __MOTION_CONTROL_H__

#include "sys.h"

#define PI 3.14159265

enum {
    EmBodyStop = 0,
    EmBodyGoForward,
    EmBodyGoBackward,
    EmBodyTurnLeft,
    EmBodyTrunRight 
};
enum EnumBodyPosture{
    EmPostureNone = 0,  //立正
    EmPostureFall,
    EmPostureHang
};

extern u8 MoveDirection;
extern u8 BodyPosture;

void MotorGetConstEuler(float *out_pitch, float *out_roll, float *out_yaw);

void EXTI9_5_IRQHandler(void);
int BalanceKeepUp(float pitch, float mid_val, float gyro);
int VelocityKeep(int encoder_left,int encoder_right);
int TurnKeep(int encoder_left, int encoder_right, float gyro);
#endif
