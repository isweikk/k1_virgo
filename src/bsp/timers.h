/**
 * the timers for every app.
 */
#ifndef __TIMERS_H__
#define __TIMERS_H__
#include "sys.h"


extern volatile u32 VoltageWaitTime;
extern volatile u32 UltrasonicWaveWaitTime;

void CommonTimerInit(void);
 
#endif
