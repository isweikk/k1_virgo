/**
 * this usart is using to transfer data to bluetooths.
 */
#include "usart3.h"
#include "motion_control.h"

/**************************************************************************
函数功能：串口3初始化
入口参数： bound:波特率
返回  值：无
**************************************************************************/
void Usart3Init(u32 bound)
{  	 
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//使能UGPIOB时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//使能USART3时钟
    USART_DeInit(USART3);
    //USART3_TX  
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB.10
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    //USART3_RX	  
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PB11
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    //Usart3 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
    //USART 初始化设置
    USART_InitStructure.USART_BaudRate = bound;//串口波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
    USART_Init(USART3, &USART_InitStructure);     //初始化串口3
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启串口接受中断
    USART_Cmd(USART3, ENABLE);                    //使能串口3 
}

/**************************************************************************
函数功能：串口3接收中断
入口参数：无
返回  值：无
**************************************************************************/
void USART3_IRQHandler(void)
{	
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) //接收到数据
    {	  
        static int uart_receive = 0;//蓝牙接收相关变量
        uart_receive=USART_ReceiveData(USART3); 
        if (uart_receive==0x00) {
            MoveDirection = EmBodyStop;
        } else if (uart_receive==0x01) {
            MoveDirection = EmBodyGoForward;
        } else if (uart_receive==0x05) {
            MoveDirection = EmBodyGoBackward;
        } else if (uart_receive==0x02||uart_receive==0x03||uart_receive==0x04) {
            MoveDirection = EmBodyTurnLeft;
        } else if (uart_receive==0x06||uart_receive==0x07||uart_receive==0x08) {
            MoveDirection = EmBodyTrunRight;
        } else {
            MoveDirection = EmBodyStop;
        }
    }							 
} 

void Usart3SendByte(char byte)   //串口发送一个字节
{
    while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
    USART_SendData(USART3, byte);        //通过库函数  发送数据
    while(USART_GetFlagStatus(USART3,USART_FLAG_TC) == RESET);  
    //等待发送完成。   检测 USART_FLAG_TC 是否置1；    //见库函数 P359 介绍
}

void Usart3SendBuf(char *buf, u16 len)
{
    u16 i;
    for(i=0; i<len; i++)
        Usart3SendByte(*buf++);
}
void Usart3SendStr(char *str)
{
    u16 i,len;
    len = strlen(str);
    for(i=0; i<len; i++)
        Usart3SendByte(*str++);
}

