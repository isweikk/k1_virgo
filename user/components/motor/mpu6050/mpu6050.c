/*
 * @Copyright: Copyright (C) 2021-2021 Kevin group. All rights reserved.
 * @Description: mpu6050 driver
 * @Author: Kevin
 * @Email: weikaiup@163.com
 * @Date: 2021-01-08
 */
#include "mpu6050.h"
#include "mpu_iic.h"
#include "components/motor/mpu6050/eMPL/inv_mpu.h"
#include "components/util/util_sys.h"

// 初始化MPU6050
uint8_t MPU_Init(void)
{
    I2C_Init();
    if (mpu_dmp_init() != RET_OK) {
        ulog("mpu6050 init fail");
        return RET_ERR;
    }
    ulog("mpu6050 init OK");
    return RET_OK;
}
// MPU_GYRO_CFG_REG 设置MPU6050陀螺仪传感器满量程范围
// fsr:0,±250dps;1,±500dps;2,±1000dps;3,±2000dps


// MPU_ACCEL_CFG_REG 设置MPU6050加速度传感器满量程范围
// fsr:0,±2g;1,±4g;2,±8g;3,±16g

//设置MPU6050的数字低通滤波器
//lpf:数字低通滤波频率(Hz)

// 得到温度值，返回值扩大了100倍
short MPU_GetTemperature(void)
{
    uint8_t buf[2];
    short raw;
    float temp;
    I2C_ReadLongData(MPU_ADDR, MPU_TEMP_OUTH_REG, 2, buf);
    raw = ((uint16_t)buf[0] << 8) | buf[1];
    temp = 36.53 + ((double)raw) / 340;
    return temp * 100;
}

// 得到陀螺仪值(原始值)
// gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
uint8_t MPU_GetGyroscope(short *gx, short *gy, short *gz)
{
    uint8_t buf[6], res;
    res = I2C_ReadLongData(MPU_ADDR, MPU_GYRO_XOUTH_REG, 6, buf);
    if (res == 0) {
        *gx = ((uint16_t)buf[0] << 8) | buf[1];
        *gy = ((uint16_t)buf[2] << 8) | buf[3];
        *gz = ((uint16_t)buf[4] << 8) | buf[5];
    }
    return res;
}

// 得到加速度值(原始值)
// gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
uint8_t MPU_GetAccelerometer(short *ax, short *ay, short *az)
{
    uint8_t buf[6], res;
    res = I2C_ReadLongData(MPU_ADDR, MPU_ACCEL_XOUTH_REG, 6, buf);
    if (res == 0) {
        *ax = ((uint16_t)buf[0] << 8) | buf[1];
        *ay = ((uint16_t)buf[2] << 8) | buf[3];
        *az = ((uint16_t)buf[4] << 8) | buf[5];
    }
    return res;
}

uint16_t g_mpu6050Flag = 0;
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    g_mpu6050Flag = 1;
}
