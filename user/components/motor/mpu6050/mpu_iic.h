/*
 * @Copyright: Copyright (C) 2021-2021 Kevin group. All rights reserved.
 * @Description: the iic driver of the MPU6050 chip
 * @Author: Kevin
 * @Email: weikaiup@163.com
 * @Date: 2021-01-08
 */
#ifndef __MPUIIC_H__
#define __MPUIIC_H__

#include "typedef.h"

// IIC所有操作函数
void I2C_Init(void);                                                            // 初始化IIC的IO口
void I2C_Start(void);                                                           // 发送IIC开始信号
void I2C_Stop(void);                                                            // 发送IIC停止信号
void I2C_SendByte(uint8_t txd);                                                 // IIC发送一个字节
uint8_t I2C_ReadByte(uint8_t ack);                                              // IIC读取一个字节
uint8_t I2C_WaitAck(void);                                                      // IIC等待ACK信号
void I2C_SendAck(void);                                                         // IIC发送ACK信号
void I2C_SendNAck(void);                                                        // IIC不发送ACK信号
uint8_t I2C_SendLongData(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf); // IIC连续写
uint8_t I2C_ReadLongData(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf); // IIC连续读

#endif
