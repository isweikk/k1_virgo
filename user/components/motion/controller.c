/*
 * @Copyright: Copyright (C) 2021-2021 Kevin group. All rights reserved.
 * @Description: motion controller
 * @Author: Kevin
 * @Email: weikaiup@163.com
 * @Date: 2021-01-08
 */
#include "controller.h"

#include "cmsis_os.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_tim.h"

#include "components/util/util_sys.h"
#include "mpu6050/mpu6050.h"
#include "mpu6050/eMPL/inv_mpu.h"
#include "motor.h"

/**************************************************************************
函数功能：运动控制器
    5ms定时中断由MPU6050的INT引脚触发
    严格保证采样和数据处理的时间同步	
    在MPU6050的采样频率设置中，设置成100HZ，即可保证6050的数据是10ms更新一次。
    读者可在imv_mpu.h文件第26行的宏定义进行修改(#define DEFAULT_MPU_HZ  (100))
**************************************************************************/
#define METHOD_GET_ANGLE   1   // 获取角度的算法，1：四元数  2：卡尔曼  3：互补滤波
// 基础数据
static float pitch, roll, yaw;          // 欧拉角(姿态角)
static int16_t gyrox, gyroy, gyroz;     // 陀螺仪原始数据

static float MechanicalMidVal = -1;    // 机械平衡中值。

static float BalanceUpKp = 300;     // 小车直立环KP
static float BalanceUpKd = 1;

static float VelocityKp = 90;   // 速度环
static float VelocityKi = 0.5;  // VelocityKp/200;

static float TurnKp = 1;    // 转向环
static float TurnKd = 1/12;

uint8_t MoveDirection = BODY_STOP;
uint8_t BodyPosture = POSTURE_FALL; //身体姿态

uint32_t CtrlCheckEnvFine(int32_t speed);

void MotorGetConstEuler(float *out_pitch, float *out_roll, float *out_yaw)
{
    *out_pitch = pitch;
    *out_roll = roll;
    *out_yaw = yaw;
}

int GetBodyStat(void)
{
    return BodyPosture;
}
/**************************************************************************
函数功能：直立PD控制
入口参数：角度、机械平衡角度（机械中值）、角速度
返回  值：直立控制PWM
**************************************************************************/
int32_t BalanceKeep(float pitch, float mid_val, float gyro)
{  
    float bias;
    int balance;
    bias = pitch - mid_val;    							 // 求出平衡的角度中值和机械相关
    balance = BalanceUpKp * bias + BalanceUpKd * gyro;   // 计算平衡控制的电机PWM  PD控制   kp是P系数 kd是D系数 
    return balance;
}

/**************************************************************************
函数功能：速度PI控制
入口参数：电机编码器的值
返回  值：速度控制PWM
**************************************************************************/
int32_t VelocityKeep(int32_t encoder_left, int32_t encoder_right, uint32_t timeMs)
{  
    static float velocity_pwm = 0, encoder_least = 0, encoder_filter = 0, encoder_integral = 0;
    static float movement;

    if (MoveDirection == BODY_GO_FORWARD) {
        movement = 50;
    } else if (MoveDirection == BODY_GO_BACKWARD) {
        movement = -50;
    // } else if(UltrasonicWaveWarning()) {
    //     movement = -20;		
    } else {	
        movement = 0;
    }
    // 速度PI控制器	
    encoder_least = (encoder_left + encoder_right) * 10 / (int32_t)timeMs - 0;                 // 获取最新速度偏差==测量速度（左右编码器之和）-目标速度（此处为零） 
    encoder_filter *= 0.7;		                                        // 一阶低通滤波器       
    encoder_filter += encoder_least * 0.3;	                            // 一阶低通滤波器    
    encoder_integral += encoder_filter;                                 // 积分出位移 积分时间：10ms
    encoder_integral = encoder_integral - movement;                     // 接收遥控器数据，控制前进后退
    if (encoder_integral > 10000) {										// 积分限幅
        encoder_integral = 10000;
    }
    if (encoder_integral < -10000)	{	// 积分限幅	
        encoder_integral = -10000;
    }
    velocity_pwm = encoder_filter * VelocityKp + encoder_integral * VelocityKi;        // 速度控制	
    // ulogd("VelocityKeep:%d, %d, %d, %f", encoder_left, encoder_right, timeMs, encoder_least);
    // ulogd("VelocityKeep:%d, %d, %f, %f", velocity_pwm, encoder_least, encoder_filter, encoder_integral);
    if (CtrlCheckEnvFine(0)) {	// 电机异常后清除积分
        encoder_filter = 0;
        encoder_integral = 0;
    }
    return velocity_pwm;
}

/**************************************************************************
函数功能：转向PD控制
入口参数：电机编码器的值、Z轴角速度
返回  值：转向控制PWM
**************************************************************************/
int32_t TurnKeep(int32_t encoder_left, int32_t encoder_right, float gyro)//转向控制
{
    static float turn_pwm = 0, turn_target = 0, encoder_temp, turn_convert = 3, turn_count = 0;
    static long turn_bias_integral;
    int32_t turn_bias, turn_amplitude = 1500/METHOD_GET_ANGLE + 800;

    // 这一部分主要是根据旋转前的速度调整速度的起始速度，增加小车的适应性
    if (MoveDirection == BODY_TURN_LEFT
        || MoveDirection == BODY_TURN_RIGHT) {
        if(++turn_count == 1)
            encoder_temp = abs(encoder_left + encoder_right);      
        turn_convert = 2000/encoder_temp;
        if(turn_convert < 3)
            turn_convert = 3;
        else if(turn_convert > 10)
            turn_convert = 10;
    } else {
        turn_convert = 3;
        turn_count = 0;
        encoder_temp = 0;
    }

    // 速度缓冲
    if (MoveDirection == BODY_TURN_LEFT) {
        turn_target += turn_convert;
    } else if (MoveDirection == BODY_TURN_RIGHT) {
        turn_target -= turn_convert; 
    } else {
        turn_target = 0;
    }

    // 转向	速度限幅
    if (turn_target > turn_amplitude) {
        turn_target = turn_amplitude;
    } else if (turn_target < -turn_amplitude) {
        turn_target = -turn_amplitude;
    }

    turn_bias = encoder_left - encoder_right;           // 计算转向速度偏差  
    if (GetBodyStat() != POSTURE_FALL) {                // 为了防止积分影响用户体验，只有电机开启的时候才开始积分
        turn_bias_integral += turn_bias;                // 转向速度偏差积分得到转向偏差（请认真理解这句话）
        turn_bias_integral -= turn_target;              // 获取遥控器数据
    } else {
        return 0;
    }
    // 积分限幅
    if(turn_bias_integral > 1800) {
        turn_bias_integral = 1800;
    } else if(turn_bias_integral < -1800) {
        turn_bias_integral = -1800;	
    }
    // 结合Z轴陀螺仪进行PD控制
    if (MoveDirection == BODY_GO_FORWARD
        || MoveDirection == BODY_GO_BACKWARD) {
        turn_pwm = turn_bias_integral*TurnKp;   // 转向的时候取消陀螺仪的纠正 有点模糊PID的思想
    } else {
        turn_pwm = turn_bias_integral*TurnKp + gyro*TurnKd;
    }
    return turn_pwm;
}

static uint16_t g_mpu6050Flag = false;

// 检查当前运行环境，对异常状态作出反应
uint32_t CtrlCheckEnvFine(int32_t speed)
{
    static int lastBodyPosture = POSTURE_STAND;
    float angle = pitch;
    uint32_t ret = RET_OK;

    // 倾角大于40度，电池低电压，USB充电 == 关闭电机
    if (angle < -60 || angle > 60 /*|| CheckBatteryLowPwr() || GetChargingStatus()*/) {
        BodyPosture = POSTURE_FALL;
    } else if (speed > 7000 && gyroy > -1000 && gyroy < 1000) {
        //速度最大值，但加速度变化很小，则可能被悬空了
        BodyPosture = POSTURE_HANG;
    }
    if (lastBodyPosture == POSTURE_STAND) {
        if (BodyPosture != POSTURE_STAND) {
            lastBodyPosture = BodyPosture;
        }
    }
    if (lastBodyPosture == POSTURE_FALL) {  // 如果上次的姿态是倾倒，则等待姿态变为直立，再重启平衡环
        if (pitch >= -5 && pitch <= 5) {
            lastBodyPosture = BodyPosture = POSTURE_STAND;
        } else {
            ret = RET_ERR;
        }
    } else if (lastBodyPosture == POSTURE_HANG) {
        lastBodyPosture = POSTURE_STAND;
    }
    return ret;
}

void CtrlAdjustFromCapData(void)
{
    // 采集数据
    mpu_dmp_get_data(&pitch, &roll, &yaw);					    // 得到欧拉角（姿态角）的数据
    MPU_GetGyroscope(&gyrox, &gyroy, &gyroz);				    // 得到陀螺仪数据
    int32_t EncoderLeftCnt = GetEncoderVal(ENCODER_LEFT) / 2;   // 读取编码器的值，因为两个电机的旋转了180度的，所以对其中一个取反，保证输出极性一致
    int32_t EncoderRightCnt = GetEncoderVal(ENCODER_RIGHT) / 2; // 读取编码器的值
    EncoderLeftCnt *= -1;
    
    // PID 计算数据
    static uint32_t lastTime = 0;
    if (lastTime == 0) {
        lastTime = GetRuntimeMs();
        return;
    }
    uint32_t nowTime = GetRuntimeMs();
    int32_t BalancePwm = BalanceKeep(pitch, MechanicalMidVal, gyroy);       // 平衡环PID控制	
    int32_t VelocityPwm = VelocityKeep(EncoderLeftCnt, EncoderRightCnt, nowTime - lastTime);    // 速度环PID控制	 
    int32_t TurnPwm = TurnKeep(EncoderLeftCnt, EncoderRightCnt, gyroz);     // 转向环PID控制
    lastTime = nowTime;

    // 计算电机转速结果
    int32_t Motor1Pwm = BalancePwm - VelocityPwm - TurnPwm;                 // 计算左轮电机最终PWM
    int32_t Motor2Pwm = BalancePwm - VelocityPwm + TurnPwm;                 // 计算右轮电机最终PWM
    if (CtrlCheckEnvFine(Motor1Pwm) != RET_OK) {							// 检查异常，并作出反应
        Motor1Pwm = 0;
        Motor2Pwm = 0;
        lastTime = 0;
    }
    MotorSetSpeed(MOTOR_LEFT, Motor1Pwm);
    MotorSetSpeed(MOTOR_RIGHT, Motor2Pwm);
}

// MPU6050中断处理函数，因I2C耗时长，所以用flag标记，由处理函数接管
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    g_mpu6050Flag = true;
}

#include "components/misc/dev_misc.h"
void MotionTask(void *argument)
{
    ulog("MotionTask start");
    MPU_Init();
    MotorInit();
    while(1) {
        if (KeyScan() == KEY_ONE_CLICK) {
            break;
        }
        osDelay(5);
    }
    while (1) {
        if (g_mpu6050Flag) {
            ulog("time=%d", GetRuntimeMs());
            CtrlAdjustFromCapData();
            g_mpu6050Flag = false;
        }
        osDelay(1);
    }
    ulog("MotionTask exit");
}

