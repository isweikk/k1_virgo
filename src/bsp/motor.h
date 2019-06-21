/**
 * the driver of motor
 */
#ifndef __MOTOR_H__
#define __MOTOR_H__

#include <sys.h>	


#define MOTOR_PWMA   TIM1->CCR1  //PA8
#define MOTOR_PWMB   TIM1->CCR4  //PA11

#define MOTOR_AIN1   PBout(14)
#define MOTOR_AIN2   PBout(15)
#define MOTOR_BIN1   PBout(13)
#define MOTOR_BIN2   PBout(12)


void MotorInit(void);
void MotorSetPwm(int motor1,int motor2);
int abs(int a);
void MotorTurnOff(float angle, float voltage);
#endif
