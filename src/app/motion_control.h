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
};

extern u8 MoveDirection;
extern u8 BodyPosture;


void EXTI9_5_IRQHandler(void);
int balance_UP(float Angle,float Mechanical_balance,float Gyro);
int velocity(int encoder_left,int encoder_right);
int turn(int encoder_left,int encoder_right,float gyro);
#endif
