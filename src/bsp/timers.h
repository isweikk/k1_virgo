/**
 * the timers for every app.
 */
#ifndef __TIMERS_H__
#define __TIMERS_H__
#include "sys.h"


extern volatile u8 VoltageWaitTime;
extern volatile u8 UltrasonicWaveWaitTime;

void CommonTimerInit(void);
 
#endif
