/*
 *the Peripherals use to communicate data.
 */
#include "usart2.h"

//uart2
#define U2_RX_LEN    128
u8 kNewCmdFlag = 0; //cmd is ok
u16 kU2RxStat = 0;  //receive status
u8 kU2RxBuf[U2_RX_LEN] = {0};
u16 kU2RxLen = 0;

void Uart2Init(u32 bound)
{
      //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
     
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
     
    //USART2_TX   GPIOA.2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.2
    
    //USART2_RX	  GPIOA.3初始化
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA3
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.3 
    
    //Usart2 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2 ;//抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
    
    //USART 初始化设置
    
    USART_InitStructure.USART_BaudRate = bound;//串口波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
    
    USART_Init(USART2, &USART_InitStructure); //初始化串口2
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启串口接收中断
    USART_Cmd(USART2, ENABLE);                    //使能串口2

}

void Uart2SendByte(u8 data)
{
    while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);    //TX BUFFER empty,if no this line,we will lost first char
    USART_SendData(USART2, data);
    while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET); //tx over,if use TXE,we will lost last char.
}

/**
* Function       USART2_IRQHandler
* @author        liusen
* @date          2015.01.03    
* @brief         串口1中断服务程序
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   无
*/
void USART2_IRQHandler(void) {
    static u8 start_flag = 0;
    static u16 tmp_len = 0;
    u8 data = 0;

    if (USART_GetFlagStatus(USART2, USART_FLAG_ORE) != RESET) {//注意！不能使用if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)来判断  
        USART_ClearFlag(USART2, USART_FLAG_ORE); //读SR其实就是清除标志
           USART_ReceiveData(USART2);
    }
        
    if (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) != RESET) {
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
        data = USART_ReceiveData(USART2);
        if (start_flag == 0) {
            if (data == '$') {
                start_flag = 1;
                kU2RxBuf[0] = data;
                tmp_len = 1;
            }
        } else {
            kU2RxBuf[tmp_len] = data;
            tmp_len++;
            if (data == '#') {
                kNewCmdFlag = 1; 
                start_flag = 0;
                kU2RxLen = tmp_len;	
                kU2RxBuf[tmp_len] = '\0';
                tmp_len = 0;
            }
        }
        
        if (tmp_len >= U2_RX_LEN) {
            tmp_len = 0;
            start_flag = 0;
            kNewCmdFlag = 0;
        }
    }
}

u8 U2GetCmdFlag(void)
{
    return kNewCmdFlag;
}

void U2ClearCmdFlag(void)
{
    kNewCmdFlag = 0;
}

u16 U2GetCmdData(u8 *buf)
{
    if (kNewCmdFlag) {
        memcpy(buf, kU2RxBuf, kU2RxLen + 1);
        kNewCmdFlag = 0;
        return kU2RxLen;
    }
    return 0;
}


