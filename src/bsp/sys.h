/**
 * the init of system
 * the function is usefull to every api.
 */
#ifndef __BSP_SYS_H__
#define __BSP_SYS_H__

#include <stm32f10x.h>
#include <stdio.h>
#include <string.h>
#include "typedef.h"

#define SYS_CLK_PLL  9  //2-16,CLK = 8*pll

//位带操作，实现单端口的电平控制
//参考<<CM3权威指南>>第五章87-92.
//IO operate define
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO address define
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C 
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C 
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001180C 
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) //0x40011A0C    
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x40011E0C    

#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08 
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008 
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408 
#define GPIOE_IDR_Addr    (GPIOE_BASE+8) //0x40011808 
#define GPIOF_IDR_Addr    (GPIOF_BASE+8) //0x40011A08 
#define GPIOG_IDR_Addr    (GPIOG_BASE+8) //0x40011E08 
 
//IO口位操作，n值要小于16
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)
#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)

//JTAG模式设置
typedef enum{
    EmJtagAllDisable = 0x02,
    EmJtagOnlySwd = 0x01,
    EmJtagAllEnable = 0x00
}EnumJtagMode;

/////////////////////////////////////////////////////////////////  
void DelayInit(void);
void DelayMs(u16 ms);
void DelayUs(u32 us);

//vector
void SysConfig(void);
void SysNvicConfig(void);

void SysSoftReset(void);    //系统软复位
void SysStandby(void);      //睡眠模式
void JtagConfig(EnumJtagMode mode);

void SysRccConfig(void);
//void sysClockInit(void);    //Clock init

//////////////////////////////////////////////////////////////////////////////
//汇编函数
/*
void WFI_SET(void);		//run WFI command
void INTX_DISABLE(void);//disable all interrupt
void INTX_ENABLE(void);	//enable all interrupt
void MSR_MSP(u32 addr);	//set stack address
*/

#endif











