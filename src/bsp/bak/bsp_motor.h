
#ifndef __BSP_MOTOR_H__
#define __BSP_MOTOR_H__

#include "stm32f10x.h"




void MotorGpioInit(void);
void MotorPwmInit(u16 arr, u16 psc, u16 arr2, u16 psc2);
void LeftMotorForward(void);
void LeftMotorBack(void);
void LeftMotorStop(void);
void RightMotorForward(void);
void RightMotorBack(void);
void RightMotorStop(void);
void LeftMotorPWM(int pw);
void RightMotorPWM(int pw);
void BodyChangeSpeed(int up_down);

#endif
