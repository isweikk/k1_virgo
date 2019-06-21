/**
* @par Copyright (C): 2010-2019, Shenzhen Yahboom Tech
* @file         bsp_servo.c	
* @author       liusen
* @version      V1.0
* @date         2017.07.17
* @brief        6个舵机控制驱动源文件
* @details      
* @par History  见如下说明
*                 
* version:		liusen_20170717
*/

#include "bsp_servo.h"

#include "pub_sys.h"

#include "bsp.h"

typedef struct {
    int angle;
    int pw;
}StServoPrm;

/*角度范围 0~180*/
StServoPrm kServoPrm[kEmServoIDMax] ={0};

int ServoGetGpioInfo(int id, GPIO_TypeDef **port, uint16_t *pin)
{
    switch(id){
        case kEmServoID1:
            *port = SERVO_1_PORT;
            *pin = SERVO_1_PIN;
            break;
        case kEmServoID2:
            *port = SERVO_2_PORT;
            *pin = SERVO_2_PIN;
            break;
        case kEmServoID3:
            *port = SERVO_3_PORT;
            *pin = SERVO_3_PIN;
            break;
        case kEmServoID4:
            *port = SERVO_4_PORT;
            *pin = SERVO_4_PIN;
            break;
        case kEmServoID5:
            *port = SERVO_5_PORT;
            *pin = SERVO_5_PIN;
            break;
        case kEmServoID6:
            *port = SERVO_6_PORT;
            *pin = SERVO_6_PIN;
            break;
        default:return EVN_ERROR;
    }
    return EVN_OK;
}

/**
* Function       ServoInit
* @author        liusen
* @date          2017.07.10        
* @brief         需要用到的舵机初始化接口
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   无
*/

void ServoInit(void)
{		
   	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;

    /*设置引脚模式为通用推挽输出*/
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	/*设置引脚速率为50MHz */   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

#ifdef USE_SERVO_J1

	/*开启外设时钟*/
	RCC_APB2PeriphClockCmd(SERVO_1_RCC, ENABLE); 
	/*选择要控制的引脚*/															   
  	GPIO_InitStructure.GPIO_Pin = SERVO_1_PIN;	
	/*调用库函数，初始化PORT*/
  	GPIO_Init(SERVO_1_PORT, &GPIO_InitStructure);		  
#endif

#ifdef USE_SERVO_J2
	
	/*开启外设时钟*/
	RCC_APB2PeriphClockCmd(SERVO_2_RCC, ENABLE); 
	/*选择要控制的引脚*/															   
  	GPIO_InitStructure.GPIO_Pin = SERVO_2_PIN;	
	/*调用库函数，初始化PORT*/
  	GPIO_Init(SERVO_2_PORT, &GPIO_InitStructure);		  
#endif

#ifdef USE_SERVO_J3

	/*开启外设时钟*/
	RCC_APB2PeriphClockCmd(SERVO_3_RCC, ENABLE); 
	/*选择要控制的引脚*/															   
  	GPIO_InitStructure.GPIO_Pin = SERVO_3_PIN;	
	/*调用库函数，初始化PORT*/
  	GPIO_Init(SERVO_3_PORT, &GPIO_InitStructure);		  
#endif

#ifdef USE_SERVO_J4

	/*开启外设时钟*/
	RCC_APB2PeriphClockCmd(SERVO_4_RCC, ENABLE); 
	/*选择要控制的引脚*/															   
  	GPIO_InitStructure.GPIO_Pin = SERVO_4_PIN;	
	/*调用库函数，初始化PORT*/
  	GPIO_Init(SERVO_4_PORT, &GPIO_InitStructure);		  
#endif

#ifdef USE_SERVO_J5

	/*开启外设时钟*/
	RCC_APB2PeriphClockCmd(SERVO_5_RCC, ENABLE); 
	/*选择要控制的引脚*/															   
  	GPIO_InitStructure.GPIO_Pin = SERVO_5_PIN;	
	/*调用库函数，初始化PORT*/
  	GPIO_Init(SERVO_5_PORT, &GPIO_InitStructure);		  
#endif

#ifdef USE_SERVO_J6
	/*开启外设时钟*/
	RCC_APB2PeriphClockCmd(SERVO_6_RCC, ENABLE); 
	/*选择要控制的引脚*/															   
  	GPIO_InitStructure.GPIO_Pin = SERVO_6_PIN;	
	/*调用库函数，初始化PORT*/
  	GPIO_Init(SERVO_6_PORT, &GPIO_InitStructure);		  
#endif
 
}

/**
* Function       ServoSetAngle
* @author        liusen
* @date          2017.07.17    
* @brief         舵机控制函数
* @param[in]     angle_val 角度：0~180°
* @param[out]    void
* @retval        int
* @par History   无
*/

int ServoSetAngleBlock(int id, int angle_val)
{
    GPIO_TypeDef *port;
    uint16_t pin;
    int pulsewidth;    						//定义脉宽变量
    
    if (ServoGetGpioInfo(id, &port, &pin) < 0) {
        return EVN_ERROR;
    }

	pulsewidth = (angle_val * 11) + 500;	//将角度转化为500-2480 的脉宽值

	GPIO_SetBits(port, pin);		//将舵机接口电平置高
	UsDelay(pulsewidth);    		//延时脉宽值的微秒数
	GPIO_ResetBits(port, pin);	//将舵机接口电平置低
	MsDelay(20 - pulsewidth/1000);			//延时周期内剩余时间
    
    kServoPrm[id].angle = angle_val;
    kServoPrm[id].pw = pulsewidth;
    
    return EVN_OK;
}

int ServoSetAngle(int id, int angle_val)
{
    if (id < kEmServoIDMax && angle_val > 0 && angle_val < 180) {
        kServoPrm[id].angle = angle_val;
        kServoPrm[id].pw = (angle_val * 11) + 500;	//将角度转化为500-2480 的脉宽值
        return EVN_OK;
    }

    return EVN_ERROR;
}

int ServoGetAngle(int id)
{
    if (id < kEmServoIDMax) {
        return kServoPrm[id].angle;
    }
    return EVN_ERROR;
}

int ServoGetPw(int id)
{
    if (id < kEmServoIDMax) {
        return kServoPrm[id].pw;
    }
    return EVN_ERROR;
}

/**
* Function       front_detection
* @author        liusen
* @date          2017.07.17    
* @brief         云台舵机向前
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   无
*/
void front_detection(void)
{
	int i = 0;
  	//此处循环次数减少，为了增加小车遇到障碍物的反应速度
  	for(i=0; i <= 15; i++) {						//产生PWM个数，等效延时以保证能转到响应角度
    	ServoSetAngle(1, 90);						//模拟产生PWM
  	}
}

/**
* Function       left_detection
* @author        liusen
* @date          2017.07.17    
* @brief         云台舵机向左
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   无
*/
void left_detection(void)
{
	int i = 0;
	for(i = 0; i <= 15; i++) {						//产生PWM个数，等效延时以保证能转到响应角度
		ServoSetAngle(1, 175);					//模拟产生PWM
	}
}

/**
* Function       right_detection
* @author        liusen
* @date          2017.07.17    
* @brief         云台舵机向右
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   无
*/
void right_detection(void)
{
	int i = 0;
	for(i = 0; i <= 15; i++) {						//产生PWM个数，等效延时以保证能转到响应角度
		ServoSetAngle(1, 5);						//模拟产生PWM
	}
}

