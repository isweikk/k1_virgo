/*
 *the Peripherals use to communicate data.
 */

#ifndef __BSP_SOFT_IIC_H__
#define __BSP_SOFT_IIC_H__

#include "sys.h"

//iic func
int I2cWriteSingle(u8 devAdd, u8 regAdd, u8 data);
int I2cWriteMultiple(u8 devAdd, u8 regAdd, u32 len, const u8 *dataBuffer);
int I2cReadSingle(u8 devAdd, u8 regAdd, u8 *data);
int I2cReadMultiple(u8 devAdd, u8 regAdd, u32 len, u8 *dataBuffer);


#endif
