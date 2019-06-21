/**
 * the encoder driver of the motors.
 */

#ifndef __ENCODER_H__
#define __ENCODER_H__
#include "sys.h"

#define ENCODER_TIM_PERIOD (u16)(65535)   //103的定时器是16位 2的16次方最大是65536

int EncoderInit(void);
int ReadEncoder(u8 unit_time);

#endif
