/**
 * the iic driver of the MPU6050 chip
 */
#include "mpu_iic.h"

#include "mpu6050.h"
/**
 * 以下应当根据具体引脚进行配置 
 */
//IO方向设置
#define MPU_IIC_SDA_DIR_IN()   {GPIOB->CRL&=0XFFFF0FFF;GPIOB->CRL|=8<<12;}
#define MPU_IIC_SDA_DIR_OUT()  {GPIOB->CRL&=0XFFFF0FFF;GPIOB->CRL|=3<<12;}


//#define SDA_IN()  {GPIOB->CRH&=0XFFFFFF0F;GPIOB->CRH|=8<<4;}
//#define SDA_OUT() {GPIOB->CRH&=0XFFFFFF0F;GPIOB->CRH|=3<<4;}

//IO操作函数	 
#define MPU_IIC_SCL_OUT     PBout(4) //SCL
#define MPU_IIC_SDA_OUT     PBout(3) //SDA	
#define MPU_IIC_SDA_IN   	PBin(3) //SDA	

//#define IIC_SCL    PBout(8) //SCL
//#define IIC_SDA    PBout(9) //SDA	 
//#define READ_SDA   PBin(9)  //输入SDA 

//初始化IIC
void MPU_IIC_Init(void)
{			
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); 	//使能PB端口时钟
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4;	//端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;      	//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     	//50M
    GPIO_Init(GPIOB, &GPIO_InitStructure);			      	//根据设定参数初始化GPIOB 
    GPIO_SetBits(GPIOB, GPIO_Pin_6);
    GPIO_SetBits(GPIOB, GPIO_Pin_7);
}

//MPU IIC 延时函数
void MPU_IIC_Delay(void)
{
    DelayUs(2);
}

/************************** end config ******************************/

//产生IIC起始信号
void MPU_IIC_Start(void)
{
    MPU_IIC_SDA_DIR_OUT();     //sda线输出
    MPU_IIC_SDA_OUT=1;	  	  
    MPU_IIC_SCL_OUT=1;
    MPU_IIC_Delay();
     MPU_IIC_SDA_OUT=0;//START:when CLK is high,DATA change form high to low 
    MPU_IIC_Delay();
    MPU_IIC_SCL_OUT=0;//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void MPU_IIC_Stop(void)
{
    MPU_IIC_SDA_DIR_OUT();//sda线输出
    MPU_IIC_SCL_OUT=0;
    MPU_IIC_SDA_OUT=0;//STOP:when CLK is high DATA change form low to high
     MPU_IIC_Delay();
    MPU_IIC_SCL_OUT=1;  
    MPU_IIC_SDA_OUT=1;//发送I2C总线结束信号
    MPU_IIC_Delay();							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 MPU_IIC_Wait_Ack(void)
{
    u8 ucErrTime=0;
    MPU_IIC_SDA_DIR_IN();      //SDA设置为输入  
    MPU_IIC_SDA_OUT=1;
    MPU_IIC_Delay();	   
    MPU_IIC_SCL_OUT=1;
    MPU_IIC_Delay();	 
    while (MPU_IIC_SDA_IN) {
        ucErrTime++;
        if (ucErrTime>250) {
            MPU_IIC_Stop();
            return 1;
        }
    }
    MPU_IIC_SCL_OUT=0;//时钟输出0 	   
    return 0;  
} 
//产生ACK应答
void MPU_IIC_Ack(void)
{
    MPU_IIC_SCL_OUT=0;
    MPU_IIC_SDA_DIR_OUT();
    MPU_IIC_SDA_OUT=0;
    MPU_IIC_Delay();
    MPU_IIC_SCL_OUT=1;
    MPU_IIC_Delay();
    MPU_IIC_SCL_OUT=0;
}
//不产生ACK应答		    
void MPU_IIC_NAck(void)
{
    MPU_IIC_SCL_OUT=0;
    MPU_IIC_SDA_DIR_OUT();
    MPU_IIC_SDA_OUT=1;
    MPU_IIC_Delay();
    MPU_IIC_SCL_OUT=1;
    MPU_IIC_Delay();
    MPU_IIC_SCL_OUT=0;
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void MPU_IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
    MPU_IIC_SDA_DIR_OUT(); 	    
    MPU_IIC_SCL_OUT=0;//拉低时钟开始数据传输
    for (t=0; t<8; t++) {
        MPU_IIC_SDA_OUT=(txd & 0x80)>>7;
        txd<<=1; 	  
        MPU_IIC_SCL_OUT=1;
        MPU_IIC_Delay(); 
        MPU_IIC_SCL_OUT=0;	
        MPU_IIC_Delay();
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 MPU_IIC_Read_Byte(u8 ack)
{
    u8 i, receive=0;
    MPU_IIC_SDA_DIR_IN();//SDA设置为输入
    for (i=0; i<8; i++ ) {
        MPU_IIC_SCL_OUT=0; 
        MPU_IIC_Delay();
        MPU_IIC_SCL_OUT=1;
        receive<<=1;
        if (MPU_IIC_SDA_IN)
            receive++;   
        MPU_IIC_Delay(); 
    }					 
    if (!ack)
        MPU_IIC_NAck();//发送nACK
    else
        MPU_IIC_Ack(); //发送ACK   
    return receive;
}


//IIC连续写
//addr:器件地址 
//reg:寄存器地址
//len:写入长度
//buf:数据区
//返回值:0,正常
//    其他,错误代码
u8 MPU_Write_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{
    u8 i; 
    MPU_IIC_Start(); 
    MPU_IIC_Send_Byte((addr<<1)|0);//发送器件地址+写命令	
    if (MPU_IIC_Wait_Ack()) {
        MPU_IIC_Stop();		 
        return 1;		
    }
    MPU_IIC_Send_Byte(reg);	//写寄存器地址
    MPU_IIC_Wait_Ack();		//等待应答
    for (i=0;i<len;i++) {
        MPU_IIC_Send_Byte(buf[i]);	//发送数据
        if (MPU_IIC_Wait_Ack()) {
            MPU_IIC_Stop();	 
            return 1;		 
        }		
    }    
    MPU_IIC_Stop();	 
    return 0;	
}

//IIC连续读
//addr:器件地址
//reg:要读取的寄存器地址
//len:要读取的长度
//buf:读取到的数据存储区
//返回值:0,正常
//    其他,错误代码
u8 MPU_Read_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{ 
    MPU_IIC_Start(); 
    MPU_IIC_Send_Byte((addr<<1)|0);//发送器件地址+写命令	
    if (MPU_IIC_Wait_Ack()) {
        MPU_IIC_Stop();		 
        return 1;		
    }
    MPU_IIC_Send_Byte(reg);	//写寄存器地址
    MPU_IIC_Wait_Ack();		//等待应答
    MPU_IIC_Start();
    MPU_IIC_Send_Byte((addr<<1)|1);//发送器件地址+读命令	
    MPU_IIC_Wait_Ack();		//等待应答 
    while (len) {
        if(len==1)
            *buf=MPU_IIC_Read_Byte(0);//读数据,发送nACK 
        else
            *buf=MPU_IIC_Read_Byte(1);		//读数据,发送ACK  
        len--;
        buf++; 
    }    
    MPU_IIC_Stop();	//产生一个停止条件 
    return 0;	
}

//IIC写一个字节 
//reg:寄存器地址
//data:数据
//返回值:0,正常
//    其他,错误代码
u8 MPU_Write_Byte(u8 reg,u8 data) 				 
{ 
    MPU_IIC_Start(); 
    MPU_IIC_Send_Byte((MPU_ADDR<<1)|0);//发送器件地址+写命令	
    if (MPU_IIC_Wait_Ack()) {
        MPU_IIC_Stop();		 
        return 1;		
    }
    MPU_IIC_Send_Byte(reg);	//写寄存器地址
    MPU_IIC_Wait_Ack();		//等待应答 
    MPU_IIC_Send_Byte(data);//发送数据
    if (MPU_IIC_Wait_Ack()) {
        MPU_IIC_Stop();	 
        return 1;		 
    }		 
    MPU_IIC_Stop();	 
    return 0;
}

//IIC读一个字节 
//reg:寄存器地址 
//返回值:读到的数据
u8 MPU_Read_Byte(u8 reg)
{
    u8 res;
    MPU_IIC_Start(); 
    MPU_IIC_Send_Byte((MPU_ADDR<<1)|0);//发送器件地址+写命令	
    MPU_IIC_Wait_Ack();		//等待应答 
    MPU_IIC_Send_Byte(reg);	//写寄存器地址
    MPU_IIC_Wait_Ack();		//等待应答
    MPU_IIC_Start();
    MPU_IIC_Send_Byte((MPU_ADDR<<1)|1);//发送器件地址+读命令	
    MPU_IIC_Wait_Ack();		//等待应答 
    res=MPU_IIC_Read_Byte(0);//读取数据,发送nACK 
    MPU_IIC_Stop();			//产生一个停止条件 
    return res;		
}
















