/*
 * @Copyright: Copyright (C) 2021-2021 Kevin group. All rights reserved.
 * @Description: IIC驱动，已经去除MPU6050部分，可用于所有IIC设备
 * @Author: Kevin
 * @Email: weikaiup@163.com
 * @Date: 2021-01-08
 */
#include "mpu_iic.h"

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_gpio.h"

#include "components/util/util_sys.h"

// IIC端口定义
#define I2C_IO_HIGH GPIO_PIN_SET
#define I2C_IO_LOW  GPIO_PIN_RESET
#define I2C_SCL_SET(level) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, (level) ? GPIO_PIN_SET : GPIO_PIN_RESET)
#define I2C_SDA_SET(level) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, (level) ? GPIO_PIN_SET : GPIO_PIN_RESET)
#define I2C_SDA_GET() HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3)

// 设置IIC SDA的IO方向，0输入，1输出
void I2C_SetSdaDir(uint8_t dir)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = GPIO_PIN_3;
    if (dir) {
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    } else {
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    }
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void I2C_Init(void)
{
    // 按道理bsp中gpio已经初始化，但是这里必须要初始化为输出才正常
    I2C_SetSdaDir(1);
}

// IIC 延时函数
void I2C_Delay(void)
{
    //vTaskDelay(1);
    for (int i = 0; i < 10; i++);
}

/************************** end config ******************************/

// 产生IIC起始信号
void I2C_Start(void)
{
    I2C_SDA_SET(I2C_IO_HIGH);
    I2C_SCL_SET(I2C_IO_HIGH);
    I2C_Delay();
    I2C_SDA_SET(I2C_IO_LOW);  // START:when CLK is high,DATA change form high to low
    I2C_Delay();
    I2C_SCL_SET(I2C_IO_LOW);  // 钳住I2C总线，准备发送或接收数据
}

// 产生IIC停止信号
void I2C_Stop(void)
{
    I2C_SCL_SET(I2C_IO_LOW);
    I2C_SDA_SET(I2C_IO_LOW);  // STOP:when CLK is high DATA change form low to high
    I2C_Delay();
    I2C_SCL_SET(I2C_IO_HIGH);
    I2C_SDA_SET(I2C_IO_HIGH);  // 发送I2C总线结束信号
    I2C_Delay();
}

// 等待应答信号到来，SDA低电平即应答信号
// 返回值：1，接收应答失败， 0，接收应答成功
uint8_t I2C_WaitAck(void)
{
    uint8_t errCnt = 0;
    I2C_SCL_SET(I2C_IO_LOW);
    I2C_SetSdaDir(0);
    I2C_Delay();
    I2C_SCL_SET(I2C_IO_HIGH);
    I2C_Delay();
    while (I2C_SDA_GET() == I2C_IO_HIGH) {
        errCnt++;
        if (errCnt > 50) {
            I2C_SCL_SET(I2C_IO_LOW);
            I2C_SetSdaDir(1);
            I2C_Stop();
            return RET_ERR;
        }
    }
    I2C_SCL_SET(I2C_IO_LOW);
    I2C_SetSdaDir(1);
    return RET_OK;
}

//产生ACK应答
void I2C_SendAck(void)
{
    I2C_SCL_SET(I2C_IO_LOW);
    I2C_SDA_SET(I2C_IO_LOW);
    I2C_Delay();
    I2C_SCL_SET(I2C_IO_HIGH);
    I2C_Delay();
    I2C_SCL_SET(I2C_IO_LOW);
}

// 不产生ACK应答
void I2C_SendNAck(void)
{
    I2C_SCL_SET(I2C_IO_LOW);
    I2C_SDA_SET(I2C_IO_HIGH);
    I2C_Delay();
    I2C_SCL_SET(I2C_IO_HIGH);
    I2C_Delay();
    I2C_SCL_SET(I2C_IO_LOW);
}

// IIC发送一个字节
// 返回从机有无应答，1，有应答；0，无应答
void I2C_SendByte(uint8_t txd)
{
    uint8_t i;
    I2C_SCL_SET(I2C_IO_LOW);  // 拉低时钟开始数据传输
    for (i = 0; i < 8; i++) {
        I2C_SDA_SET((txd & 0x80) >> 7);
        txd <<= 1;
        I2C_SCL_SET(I2C_IO_HIGH);
        I2C_Delay();
        I2C_SCL_SET(I2C_IO_LOW);
        I2C_Delay();
    }
}

// 读1个字节，ack=1时，发送ACK，ack=0，发送nACK
uint8_t I2C_ReadByte(uint8_t ack)
{
    uint8_t i, receive = 0;
    I2C_SCL_SET(I2C_IO_LOW);
    I2C_SetSdaDir(0);
    for (i = 0; i < 8; i++) {
        I2C_SCL_SET(I2C_IO_LOW);
        I2C_Delay();
        I2C_SCL_SET(I2C_IO_HIGH);
        receive <<= 1;
        if (I2C_SDA_GET() == I2C_IO_HIGH) {
            receive++;
        }
        I2C_Delay();
    }
    I2C_SCL_SET(I2C_IO_LOW);
    I2C_SetSdaDir(1);
    if (!ack) {
        I2C_SendNAck();
    } else {
        I2C_SendAck();
    }
    return receive;
}

// IIC连续写
// addr:器件地址
// reg:寄存器地址
// len:写入长度
// buf:数据区
// 返回值:0=正常，其他=错误代码
uint8_t I2C_SendLongData(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
    uint8_t i;
    I2C_Start();
    I2C_SendByte((addr << 1) | 0);  // 发送器件地址+写命令
    if (I2C_WaitAck() != RET_OK) {
        I2C_Stop();
        return RET_ERR;
    }

    I2C_SendByte(reg);  // 写寄存器地址
    if (I2C_WaitAck() != RET_OK) {
        I2C_Stop();
        return RET_ERR;
    }

    for (i = 0; i < len; i++) {
        I2C_SendByte(buf[i]);  // 发送数据
        if (I2C_WaitAck() != RET_OK) {
            I2C_Stop();
            return RET_ERR;
        }
    }
    I2C_Stop();
    return RET_OK;
}

// IIC连续读
// addr:器件地址
// reg:要读取的寄存器地址
// len:要读取的长度
// buf:读取到的数据存储区
// 返回值:0=正常，其他=错误代码
uint8_t I2C_ReadLongData(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
    I2C_Start();
    I2C_SendByte((addr << 1) | 0);  // 发送器件地址+写命令
    if (I2C_WaitAck() != RET_OK) {
        I2C_Stop();
        return RET_ERR;
    }

    I2C_SendByte(reg);  // 写寄存器地址
    if (I2C_WaitAck() != RET_OK) {
        I2C_Stop();
        return RET_ERR;
    }

    I2C_Start();
    I2C_SendByte((addr << 1) | 1);  // 发送器件地址+读命令
    if (I2C_WaitAck() != RET_OK) {
        I2C_Stop();
        return RET_ERR;
    }

    while (len) {
        if (len == 1) {
            *buf = I2C_ReadByte(0);  // 读数据,发送nACK
        } else {
            *buf = I2C_ReadByte(1);  // 读数据,发送ACK
        }
        len--;
        buf++;
    }

    I2C_Stop();  // 产生一个停止条件
    return RET_OK;
}
