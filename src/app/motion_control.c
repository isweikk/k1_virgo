/*
 * @Descripttion: motion controller
 * @Author: Kevin
 * @Email: weikk90@163.com
 * @Date: 2019-06-15 02:37:14
 * @LastEditors: Kevin
 * @LastEditTime: 2019-07-14 20:31:51
 */

#include "motion_control.h"

#include "mpu6050/mpu6050.h"
#include "mpu6050/eMPL/inv_mpu.h"
#include "bsp.h"
#include "general_api.h"

/**************************************************************************
函数功能：所有的控制代码都在这里面
         5ms定时中断由MPU6050的INT引脚触发
         严格保证采样和数据处理的时间同步	
        在MPU6050的采样频率设置中，设置成100HZ，即可保证6050的数据是10ms更新一次。
        读者可在imv_mpu.h文件第26行的宏定义进行修改(#define DEFAULT_MPU_HZ  (100))
**************************************************************************/
#define METHOD_GET_ANGLE   1   //获取角度的算法，1：四元数  2：卡尔曼  3：互补滤波
//基础数据
int EncoderLeftCnt = 0, EncoderRightCnt = 0;    //左右编码器的脉冲计数
float pitch, roll, yaw; 					    //欧拉角(姿态角)
short aacx, aacy, aacz;					        //加速度传感器原始数据
short gyrox, gyroy, gyroz;						//陀螺仪原始数据
int Motor1Pwm = 0, Motor2Pwm = 0;               //当前电机的PWM数据

//PID 计算数据
int BalancePwm = 0, VelocityPwm = 0, TurnPwm = 0;

float MechanicalMidVal = 8.3; // 机械平衡中值。

float BalanceUpKp = 300; 	 // 小车直立环KP
float BalanceUpKd = 1;

float VelocityKp = 80;  //速度环
float VelocityKi = 0.4;  //VelocityKp/200;

float TurnKp = 1;  //转向环
float TurnKd = 1/12;

//
u8 MoveDirection = EmBodyStop;
u8 BodyPosture = EmPostureFall; //身体姿态

int MotorCheckErr(void);

void EXTI9_5_IRQHandler(void) 
{    
    if (PBin(5) == 0) {		
        EXTI->PR = 1<<5;                                          //===清除LINE5上的中断标志位   
        mpu_dmp_get_data(&pitch, &roll, &yaw);					//===得到欧拉角（姿态角）的数据
        MPU_Get_Gyroscope(&gyrox, &gyroy, &gyroz);				//===得到陀螺仪数据
        EncoderLeftCnt = -ReadEncoder(EmEncoderLeft);                          //===读取编码器的值，因为两个电机的旋转了180度的，所以对其中一个取反，保证输出极性一致
        EncoderRightCnt = ReadEncoder(EmEncoderRight);                          //===读取编码器的值
        
        BalancePwm = BalanceKeepUp(pitch, MechanicalMidVal, gyroy);     //===平衡环PID控制	
        VelocityPwm = VelocityKeep(EncoderLeftCnt, EncoderRightCnt);    //===速度环PID控制	 
        TurnPwm = TurnKeep(EncoderLeftCnt, EncoderRightCnt, gyroz);     //===转向环PID控制

        Motor1Pwm = BalancePwm - VelocityPwm - TurnPwm;                 //===计算左轮电机最终PWM
        Motor2Pwm = BalancePwm - VelocityPwm + TurnPwm;                 //===计算右轮电机最终PWM
        MotorCheckErr();											//===检查异常
        MotorSetPwm(Motor1Pwm, Motor2Pwm);                           //===赋值给PWM寄存器  
    }
} 

void MotorGetConstEuler(float *out_pitch, float *out_roll, float *out_yaw)
{
    *out_pitch = pitch;
    *out_roll = roll;
    *out_yaw = yaw;
}
/**************************************************************************
函数功能：异常关闭电机
入口参数：
返回  值：0=ok, 1=error
**************************************************************************/
int MotorCheckErr(void)
{
    static int last_body_posture = EmPostureNone;
    float angle = pitch;
    int ret = 0;
    //倾角大于40度，电池低电压，USB充电 == 关闭电机
    if (angle < -40 || angle > 40 || CheckBatteryLowPwr()/*
        || GetChargingStatus()*/) {
        BodyPosture = EmPostureFall;
    } else if (Motor1Pwm > 7000 && gyroy > -1000 && gyroy < 1000) {
        //速度最大值，但加速度变化很小，则可能被悬空了
        BodyPosture = EmPostureHang;
    }
    if (last_body_posture == EmPostureNone) {
        if (BodyPosture != EmPostureNone) {
            last_body_posture = BodyPosture;
        }
    }
    if (last_body_posture == EmPostureFall) { //如果上次的姿态是倾倒，则等待姿态变为直立，再重启平衡环
        if (pitch >= -5 && pitch <= 5) {
            last_body_posture = BodyPosture = EmPostureNone;
        } else {
            Motor1Pwm = 0;
            Motor2Pwm = 0;
            ret = 1;
        }
    } else if (last_body_posture == EmPostureHang) {
        last_body_posture = EmPostureNone;
    }
    return ret;
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
int BalanceKeepUp(float pitch, float mid_val, float gyro)
{  
    float bias;
    int balance;
    bias = pitch - mid_val;    							 //===求出平衡的角度中值和机械相关
    balance = BalanceUpKp * bias + BalanceUpKd * gyro;   //===计算平衡控制的电机PWM  PD控制   kp是P系数 kd是D系数 
    return balance;
}

/**************************************************************************
函数功能：速度PI控制
入口参数：电机编码器的值
返回  值：速度控制PWM
**************************************************************************/
int VelocityKeep(int encoder_left, int encoder_right)
{  
    static float velocity_pwm, encoder_least, encoder_filter, encoder_integral;
    static float movement;

    if (MoveDirection == EmBodyGoForward) {
        movement = 50;
    } else if (MoveDirection == EmBodyGoBackward) {
        movement = -50;
    } else if(UltrasonicWaveWarning()) {
        movement = -20;		
    } else {	
        movement = 0;
    }
    //=============速度PI控制器=======================//	
    encoder_least = (encoder_left + encoder_right) - 0;                 //===获取最新速度偏差==测量速度（左右编码器之和）-目标速度（此处为零） 
    encoder_filter *= 0.7;		                                        //===一阶低通滤波器       
    encoder_filter += encoder_least * 0.3;	                            //===一阶低通滤波器    
    encoder_integral += encoder_filter;                                 //===积分出位移 积分时间：10ms
    encoder_integral = encoder_integral - movement;                     //===接收遥控器数据，控制前进后退
    if (encoder_integral > 10000) {										//===积分限幅
        encoder_integral = 10000;
    }
    if (encoder_integral < -10000)	{	//===积分限幅	
        encoder_integral = -10000;
    }
    velocity_pwm = encoder_filter * VelocityKp + encoder_integral * VelocityKi;        //===速度控制	
    if (MotorCheckErr()) {	//===电机异常后清除积分
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
int TurnKeep(int encoder_left, int encoder_right, float gyro)//转向控制
{
    static float turn_pwm = 0, turn_target = 0, encoder_temp, turn_convert = 3, turn_count;
    static long turn_bias_integral;
    int turn_bias, turn_amplitude = 1500/METHOD_GET_ANGLE + 800;

    //这一部分主要是根据旋转前的速度调整速度的起始速度，增加小车的适应性
    if (MoveDirection == EmBodyTurnLeft
        || MoveDirection == EmBodyTrunRight) {
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

    //速度缓冲
    if (MoveDirection == EmBodyTurnLeft) {
        turn_target += turn_convert;
    } else if (MoveDirection == EmBodyTrunRight) {
        turn_target -= turn_convert; 
    } else {
        turn_target = 0;
    }

    //===转向	速度限幅
    if (turn_target > turn_amplitude) {
        turn_target = turn_amplitude;
    } else if (turn_target < -turn_amplitude) {
        turn_target = -turn_amplitude;
    }

    turn_bias = encoder_left - encoder_right;           //===计算转向速度偏差  
    if (GetBodyStat() != EmPostureFall) {               //为了防止积分影响用户体验，只有电机开启的时候才开始积分
        turn_bias_integral += turn_bias;                //转向速度偏差积分得到转向偏差（请认真理解这句话）
        turn_bias_integral -= turn_target;              //获取遥控器数据
    } else {
        return 0;
    }
    //===积分限幅
    if(turn_bias_integral > 1800) {
        turn_bias_integral = 1800;
    } else if(turn_bias_integral < -1800) {
        turn_bias_integral = -1800;	
    }
    //===结合Z轴陀螺仪进行PD控制
    if (MoveDirection == EmBodyGoForward
        || MoveDirection == EmBodyGoBackward) {
        turn_pwm = turn_bias_integral*TurnKp;                              //转向的时候取消陀螺仪的纠正 有点模糊PID的思想
    } else {
        turn_pwm = turn_bias_integral*TurnKp + gyro*TurnKd;
    }
    return turn_pwm;
}
