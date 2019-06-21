/*
 *the Peripherals use to communicate data.
 */
#include <stdlib.h>
#include "sys.h"

/**************************** iic driver *****************************/
//SDA direction:
#define I2C_SDA_IN()   {GPIOB->CRL &= ~(3<<(7*2));}	//PB7输入模式
#define I2C_SDA_OUT()  {GPIOB->CRL = (GPIOB->CRL & (~(3<<(7*2)))) | (1<<7*2);} //PB7输出模式
#define I2C_SCL_SET(a) {if(a==0) GPIOB->BRR = GPIO_Pin_6; \
                        else GPIOB->BSRR = GPIO_Pin_6;}
#define I2C_SDA_SET(a) {if(a==0) GPIOB->BRR = GPIO_Pin_7; \
                        else GPIOB->BSRR = GPIO_Pin_7;}
#define I2C_SDA_GET  (GPIOB->IDR & GPIO_Pin_7)
#define I2C_DELAY(a)    {DelayUs(2*a);}


void I2cInit(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    //GPIOB7 - SDA, B6 - SCL  初始化设置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化

    GPIO_SetBits(GPIOB, GPIO_Pin_6);
    GPIO_SetBits(GPIOB, GPIO_Pin_7);
}

void I2cStart(void)
{
    I2C_SDA_OUT();
    I2C_SDA_SET(1);
    I2C_SCL_SET(1);
    I2C_DELAY(1);
    I2C_SDA_SET(0);
    I2C_DELAY(1);
    I2C_SCL_SET(0);
    I2C_DELAY(1);
}

void I2cStop(void)
{
    I2C_SDA_OUT();
    I2C_SDA_SET(0);
    I2C_SCL_SET(1);
    I2C_DELAY(1);
    I2C_SDA_SET(1);
    I2C_DELAY(1);
    I2C_SCL_SET(0);
    I2C_DELAY(1);
}

//ack:1=ACK,0=NACK
void I2cSendAck(u8 ack)
{
    I2C_SDA_OUT();
    if(ack){
        I2C_SDA_SET(0);
    }else{
        I2C_SDA_SET(1);
    }
    I2C_DELAY(1);
    I2C_SCL_SET(1);
    I2C_DELAY(1);
    I2C_SCL_SET(0);
    I2C_DELAY(1);
}

//wait ack
//return :EVN_OK=recieve ack TRUE, EVN_ERROR=recieve ack false
int I2cWaitAck(void)
{
    u16 waitTimeout = 0;

    I2C_SDA_IN();
    I2C_SDA_SET(1);	//no this line,we can't get the data
    I2C_DELAY(1);
    I2C_SCL_SET(1);
    I2C_DELAY(1); 
    while(I2C_SDA_GET)
    {
        waitTimeout++;
        if(waitTimeout > 250)
        {
            I2C_SCL_SET(0);
            I2C_DELAY(1);
            I2cStop();
            return EVN_ERROR;
        }
    }
    I2C_SCL_SET(0);
    I2C_DELAY(1); 
    return EVN_OK;
}

void I2cSendByte(u8 data)
{                        
    u8 cnt = 0;   
    I2C_SDA_OUT();
    for(cnt=0; cnt<8; cnt++)
    {
        I2C_SDA_SET((data & 0x80)>>7);
        I2C_DELAY(1);
        I2C_SCL_SET(1);
        I2C_DELAY(1);
        data <<= 1;
        I2C_SCL_SET(0);	//set scl free.
        I2C_DELAY(1);
    }	 
} 

//recieve one byte ,ack:0=NACK,1=ACK
//get data from high bit to low bit
u8 I2cRecvByte(u8 ack)
{
    u8 cnt = 0, recvData = 0;

    I2C_SDA_IN();
    I2C_SDA_SET(1);	//no this line,we can't get the data
    for(cnt=0; cnt<8; cnt++)
    {
        I2C_DELAY(1);
        I2C_SCL_SET(1);
        I2C_DELAY(1);
        recvData <<= 1;
        if(I2C_SDA_GET)
            recvData++;
        I2C_SCL_SET(0);
        I2C_DELAY(1);
    }
    I2cSendAck(ack);
    return recvData;
}

int I2cWriteSingle(u8 devAdd, u8 regAdd, u8 data)
{
    I2cStart();
    I2cSendByte((devAdd<<1) | 0x00);  //send device address + write
    if(I2cWaitAck() != EVN_OK)
        return EVN_ERROR;
    
    I2cSendByte(regAdd);              //send register address
    if(I2cWaitAck() != EVN_OK)
        return EVN_ERROR;
    
    I2cSendByte(data);  //send device address + read
    if(I2cWaitAck() != EVN_OK)
        return EVN_ERROR;
    I2cStop();
    
    return  EVN_OK;
}

int I2cWriteMultiple(u8 devAdd, u8 regAdd, u32 len, const u8 *dataBuffer)
{
    u32 tmpCnt = 0;
    
    I2cStart();
    I2cSendByte((devAdd<<1) | 0x00);  //send device address + write
    if(I2cWaitAck() != EVN_OK)
        return EVN_ERROR;
    
    I2cSendByte(regAdd);              //send register address
    if(I2cWaitAck() != EVN_OK)
        return EVN_ERROR;
    
    for(tmpCnt=0; tmpCnt< len; tmpCnt++)
    {
        I2cSendByte(dataBuffer[tmpCnt]);  //send device address + read
        if(I2cWaitAck() != EVN_OK)
            return EVN_ERROR;
    }
    I2cStop();
    
    return  EVN_OK;
}

int I2cReadSingle(u8 devAdd, u8 regAdd, u8 *data)
{
    I2cStart();
    I2cSendByte((devAdd<<1) | 0x00);  //send device address + write
    if(I2cWaitAck() != EVN_OK)
        return EVN_ERROR;
    
    I2cSendByte(regAdd);              //send register address
    if(I2cWaitAck() != EVN_OK)
        return EVN_ERROR;
    
    I2cStart();
    I2cSendByte((devAdd<<1) | 0x01);  //send device address + read
    if(I2cWaitAck() != EVN_OK)
        return EVN_ERROR;

    *data = I2cRecvByte(0);
    I2cStop();
    
    return  EVN_OK;
}

int I2cReadMultiple(u8 devAdd, u8 regAdd, u32 len, u8 *dataBuffer)
{
    u32 tmpCnt = 0;

    I2cStart();
    I2cSendByte((devAdd<<1) | 0x00);  //send device address + write
    if(I2cWaitAck() != EVN_OK)
        return EVN_ERROR;
    
    I2cSendByte(regAdd);              //send register address
    if(I2cWaitAck() != EVN_OK)
        return EVN_ERROR;
    
    I2cStart();
    I2cSendByte((devAdd<<1) | 0x01);  //send device address + read
    if(I2cWaitAck() != EVN_OK)
        return EVN_ERROR;

    for(tmpCnt=0; tmpCnt< len-1; tmpCnt++)
    {
        dataBuffer[tmpCnt] = I2cRecvByte(1);
    }
    dataBuffer[tmpCnt] = I2cRecvByte(0);
    I2cStop();
    
    return  EVN_OK;
}

/**************************** iic end ********************************/

