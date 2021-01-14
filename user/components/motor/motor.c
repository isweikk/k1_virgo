/*
 * @Copyright: Copyright (C) 2021-2021 Kevin group. All rights reserved.
 * @Description: the encoder driver, the PWM for the motors
 * @Author: Kevin
 * @Email: weikaiup@163.com
 * @Date: 2021-01-13
 */
#include "motor.h"

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_tim.h"
#include "stm32f1xx_hal_gpio.h"

#include "bsp/tim.h"
#include "components/util/util_sys.h"
/**************************************************************************
功能描述：光电编码器采样
硬件资源：
    TIM2 - 1号电机
        PA0 - TIM2_CH1
        PA1 - TIM2_CH2
    TIM4 - 2号电机
        PB6 - TIM4_CH1
        PB7 - TIM4_CH2

功能描述：PWM输出
硬件资源：
    TIM1_CH4 - PA11 - 1号电机
    TIM1_CH1 - PA8 - 2号电机

功能描述：电机方向控制
硬件资源：
    1号电机
        IN1 - PB13
        IN2 - PB12
    2号电机
        IN1 - PB14
        IN2 - PB15
控制逻辑：
    IN1  IN2  控制效果
     0    0     停止
     0    1     反转，逆时针
     1    0     正转，顺时针
     1    1     制动
**************************************************************************/
#define MOTOR_AIN1_PIN GPIO_PIN_13
#define MOTOR_AIN2_PIN GPIO_PIN_12
#define MOTOR_BIN1_PIN GPIO_PIN_14
#define MOTOR_BIN2_PIN GPIO_PIN_15

static TIM_HandleTypeDef *hdlTim1 = NULL;
static TIM_HandleTypeDef *hdlTim2 = NULL;
static TIM_HandleTypeDef *hdlTim4 = NULL;

// 设置IN1和IN2的电平
void MotorSetInLevel(uint8_t motor, uint8_t in1, uint8_t in2)
{
    if (motor == MOTOR_LEFT) {
        HAL_GPIO_WritePin(GPIOB, MOTOR_AIN1_PIN, in1 ? GPIO_PIN_SET : GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOB, MOTOR_AIN2_PIN, in2 ? GPIO_PIN_SET : GPIO_PIN_RESET);
    } else {
        HAL_GPIO_WritePin(GPIOB, MOTOR_BIN1_PIN, in1 ? GPIO_PIN_SET : GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOB, MOTOR_BIN2_PIN, in2 ? GPIO_PIN_SET : GPIO_PIN_RESET);
    }
}

// 配置PWM通道
void MotorSetPwm(uint8_t motor, int16_t pwm)
{
    if (hdlTim1 == NULL) {
        ulogd("hdlTim1 is NULL");
        return;
    }
    ulogd("motor=%d, pwm=%d", motor, pwm);
    if (motor == MOTOR_LEFT) {
        __HAL_TIM_SET_COMPARE(hdlTim1, TIM_CHANNEL_4, pwm);    // 修改比较值，即占空比
    } else {
        __HAL_TIM_SET_COMPARE(hdlTim1, TIM_CHANNEL_1, pwm);
    }
}

// 设置电机方向
void MotorSetDirect(uint8_t motor, uint8_t direct)
{
    ulogd("motor=%d, direct=%d", motor, direct);
    switch (direct) {
        case MOTOR_STOP:
        case MOTOR_BRAKE:
            MotorSetInLevel(motor, 1, 1);
            break;
        case MOTOR_FORWARD:
            MotorSetInLevel(motor, 1, 0);
            break;
        case MOTOR_REVERSE:
            MotorSetInLevel(motor, 0, 1);
            break;
        default:
            break;
    }
}

// 设置电机速度， -7200 ~ 7200
void MotorSetSpeed(uint8_t motor, int16_t speed)
{
    // PWM满幅是7200 限制在7000
    if(speed == 0) {
        MotorSetDirect(motor, MOTOR_STOP);
    } else if(speed < 0) {
        MotorSetDirect(motor, MOTOR_REVERSE);
    } else {
        MotorSetDirect(motor, MOTOR_FORWARD);
    }
    speed = abs(speed);
    MotorSetPwm(motor, (speed > 7000) ? 7000 : speed);
}

void EncoderInit(void)
{
    hdlTim2 = GetHandleOfTim2();
    hdlTim4 = GetHandleOfTim4();
    if ((hdlTim2 == NULL || hdlTim4 == NULL)) {
        uloge("Get the tim handle fail");
        return;
    }
    __HAL_TIM_SET_COUNTER(hdlTim2, 0);
    __HAL_TIM_SET_COUNTER(hdlTim4, 0);
    HAL_TIM_Encoder_Start_IT(hdlTim2, TIM_CHANNEL_1);
    HAL_TIM_Encoder_Start_IT(hdlTim4, TIM_CHANNEL_1);
}

// 初始化电机相关参数
void MotorInit(void)
{
    hdlTim1 = GetHandleOfTim1();
    if (hdlTim1 == NULL) {
        uloge("Get the tim handle fail");
        return;
    }

    MotorSetSpeed(MOTOR_LEFT, MOTOR_STOP);
    MotorSetSpeed(MOTOR_RIGHT, MOTOR_STOP);
    HAL_TIM_PWM_Start(hdlTim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(hdlTim1, TIM_CHANNEL_4);
    EncoderInit();
    ulog("Init Motor device OK");
}

/*
 * 函数功能：单位时间读取编码器计数
 * 入口参数：传感器序号
 * 返回  值：速度值
 */
uint16_t GetEncoderVal(uint8_t encoder)
{
    if ((hdlTim2 == NULL || hdlTim4 == NULL)) {
        ulogd("hdlTim2 or hdlTim4 is NULL");
        return 0;
    }
    int timCnt;
    switch(encoder) {
        case ENCODER_LEFT:
            timCnt = __HAL_TIM_GET_COUNTER(hdlTim2); // 编码器计数值
            __HAL_TIM_SET_COUNTER(hdlTim2, 0);
            __HAL_TIM_IS_TIM_COUNTING_DOWN(hdlTim2); // 编码器方向
            break;
        case ENCODER_RIGHT:
            timCnt = __HAL_TIM_GET_COUNTER(hdlTim4);
            __HAL_TIM_SET_COUNTER(hdlTim4, 0);
            __HAL_TIM_IS_TIM_COUNTING_DOWN(hdlTim4);
            break;
        default:
            timCnt=0;
    }
    return (uint16_t)timCnt;
}

uint8_t GetEncoderDir(uint8_t encoder)
{
    if ((hdlTim2 == NULL || hdlTim4 == NULL)) {
        ulogd("hdlTim2 or hdlTim4 is NULL");
        return 0;
    }

    switch(encoder) {
        case ENCODER_LEFT:
            return __HAL_TIM_IS_TIM_COUNTING_DOWN(hdlTim2); // 编码器方向
        case ENCODER_RIGHT:
            return __HAL_TIM_IS_TIM_COUNTING_DOWN(hdlTim4);
        default: break;
    }
    return 0;
}
