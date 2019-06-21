

#ifndef __BSP_H__
#define __BSP_H__

#include "sys.h"
#include "console.h"
#include "adc.h"
#include "encoder.h"
#include "exti.h"
#include "misc_dev.h"
#include "motor.h"
#include "oled.h"
#include "soft_iic.h"
#include "timers.h"
#include "ultrasonic_wave.h"
#include "usart2.h"
#include "usart3.h"


/*所有外设宏定义开关，根据需要可以开关所有外设驱动*/

/*定义需要初始化的舵机宏定义开关*/
#define USE_SERVO_J1

/*巡线开关*/
#define USE_LINE_L1
#define USE_LINE_L2
#define USE_LINE_R1
#define USE_LINE_R2

/*红外避障开关*/
#define USE_IRAVOID_L1
#define USE_IRAVOID_R1

/*七彩探照灯*/
#define USE_COLOR_RED
#define USE_COLOR_GREEN
#define USE_COLOR_BLUE

/*寻光传感器*/
#define USE_LIGHTSEEKING_L1
#define USE_LIGHTSEEKING_R1

/*超声波传感器*/
#define USE_ULTRASONIC

/*灰度传感器*/
#define USE_GS


#endif
