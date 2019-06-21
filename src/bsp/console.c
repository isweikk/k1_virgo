/*
 *the Peripherals use to communicate data.
 */
#include "console.h"
#include <stdlib.h>
#include <string.h>


#define Uart1PrintUsage(a,b) ulog("usage:%s %s", a, b)
typedef struct {
    u8 num;
    u8 cmd[16];
    u8 info[48];
    u8 prm_num;
}StConsoleCmd;

const StConsoleCmd kConsoleCmdTab[] = {
    {0, "help", "[cmd][parm1][parm1]...", 1},
    
};

u8 kUart1RxBuf[UART1_BUFFER_SIZE];    //
//recieve status
//bit15,	recieve ok flag,0x0d
//bit14,	recieve 0x0a
//bit13~0,	the data length
u16 kUart1RxFlag = 0;
u16 kUart1EchoPtr = 0;

int String2Int(u8 *string) {
    u32 val = 0;
    u8  ptr = 0;
    while (1) {
        if (*(string + ptr) == NULL) {
            return val;
        } else if ((*(string + ptr) >= '0') && (*(string + ptr) <= '9')) {
            val *= 10;
            val += (*(string + ptr)) - '0';
            ptr++;
        } else {
            return EVN_ERROR;
        }
    }
}

int Uart1ConsoleStrHdl(const u8 *string)
{
    u8 cmd_num = 0, cmd_ptr = 0;
    u8 is_cmd_ok = 0;
    u8 prm_num, prm_list[5][12] = {0}; //5 parameter, 11 byte mostest int every one.
    u8 i;
    
    if ((*string < 'a') || (*string > 'z'))  //limit the first word.
        return EVN_ERROR;
    for (; cmd_num < 100; cmd_num++) {
        if (*string != kConsoleCmdTab[cmd_num].cmd[0]) {
            continue;
        } else {
            if ((strncmp((char *)string, (char *)kConsoleCmdTab[cmd_num].cmd, strlen((char *)kConsoleCmdTab[cmd_num].cmd)) == NULL)) {
                if ((*(string + strlen((char *)kConsoleCmdTab[cmd_num].cmd)) != ' ')  //only partial same. not right
                    &&(*(string + strlen((char *)kConsoleCmdTab[cmd_num].cmd)) != NULL)) {
                    continue;
                } else {
                    is_cmd_ok = 1;
                    break;
                }
            } else {
                continue;
            }
        }
    }
    if (is_cmd_ok == 0)
        return EVN_ERROR;
    cmd_ptr += strlen((char *)kConsoleCmdTab[cmd_num].cmd);
    for (; prm_num < 5; ) {
        while (*(string + cmd_ptr) == ' '){   //ignore ' '
            cmd_ptr++;
        }
        i = 0;
        while ((*(string + cmd_ptr) != ' ') && (*(string + cmd_ptr) != NULL)) {
            prm_list[prm_num][i++] = *(string + cmd_ptr);
            cmd_ptr++;
            if (i > 12) {
                Uart1PrintUsage(kConsoleCmdTab[cmd_num].cmd, kConsoleCmdTab[cmd_num].info);
                return EVN_ERROR;
            }
        }
        if (i > 0)
            prm_num++;
        if (*(string + cmd_ptr) == NULL)
            break;
    }
    if (prm_num != kConsoleCmdTab[cmd_num].prm_num) {
        Uart1PrintUsage(kConsoleCmdTab[cmd_num].cmd, kConsoleCmdTab[cmd_num].info);
        return EVN_ERROR;
    }
    
    switch (cmd_num) {
        case 0: 
            //tmp_m0 = String2Int(prm_list[0]);
            //MotorSetPos(0, tmp_m0);
            break;
        case 1:
            break;
        default: break;
    }
    
    return EVN_OK;
}

void Uart1ConsoleTask(void) {                       
    if (kUart1EchoPtr < (kUart1RxFlag & 0x3fff)) {   //have not echoed over.
        for (; kUart1EchoPtr < (kUart1RxFlag & 0x3fff); kUart1EchoPtr++) {
            Uart1SendByte(kUart1RxBuf[kUart1EchoPtr]);
        }
    } else if ((kUart1RxFlag & 0x8000) > 0) {  //receive ok
        printf("\n");
        if(Uart1ConsoleStrHdl(kUart1RxBuf) == EVN_OK) {
            printf(" <ok>\n");
        } else if ((kUart1RxFlag & 0x3fff) > 0){
            printf(" <error>\n");
        }
        printf("stm32: ");
        kUart1RxFlag = 0;
        kUart1EchoPtr = 0;
    }
}

void Uart1Init(u32 bound) {
   //GPIO config
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 |
        RCC_APB2Periph_GPIOA, ENABLE);//enable clock of USART1
    USART_DeInit(USART1);
	
	 //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
   //USART1 config
	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8bit
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//stop 1bit
	USART_InitStructure.USART_Parity = USART_Parity_No;//no parity
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//none hardware control
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART1, ENABLE);
}

/*为确保没有从 C 库链接使用半主机的函数，因为不使用半主机，标准C库stdio.h中
有些使用半主机的函数要重新写,您必须为这些函数提供自己的实现*/
//support for using the function of "printf",then no need to use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)    //确保没有从C库链接使用半主机的函数
struct __FILE {   //标准库需要的支持函数
	int handle; 
}; 

FILE __stdout;       
int _sys_exit(int x) {    //避免使用半主机模式
	x = x;
	return 0;
} 
//modifing define the fputc
int fputc(int ch, FILE *f) { 	
    Uart1SendByte(ch);
	return ch;
}
#endif

void USART1_IRQHandler(void) {
	u8 recv_byte;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {  //数据必须是0x0d 0x0a结尾，linux以0x0a结尾
        recv_byte = USART_ReceiveData(USART1);//(USART1->DR)
        //kUart1RxBuf[kUart1RxFlag & 0X3FFF] = recv_byte ;
        //kUart1RxFlag++;
        if (recv_byte == 0x08) { //backspace
            if ((kUart1RxFlag & 0x3FFF) > 0) {  //receive one byte at least.
                kUart1RxFlag--;
                kUart1EchoPtr--;
                kUart1RxBuf[kUart1RxFlag & 0X3FFF] = 0;
                Uart1SendByte(0x08);
            }
            //goto USART1_IRQ_END;
        } else if ((recv_byte == 0x0D) || (recv_byte == 0x0A) || ((recv_byte >= ' ') && (recv_byte <= '~'))) {
            //兼容linux以\n结尾，Windows以\r\n结尾，\r和\n都不会放入buffer中。
            if ((kUart1RxFlag & 0x8000) == 0) {  //接收未完成
                if (kUart1RxFlag & 0x4000) { //接收到了0x0d
                    if (recv_byte != 0x0a) {
                        kUart1RxFlag = 0;//接收错误,重新开始
                    } else {
                        kUart1RxBuf[kUart1RxFlag & 0X3FFF] = 0;    //end char
                        kUart1RxFlag |= 0x8000;	//接收完成了 
                    }
                } else {
                    //还没收到0X0D
                    if (recv_byte == 0x0d) {
                        kUart1RxFlag |= 0x4000;  //当前收到0X0D
                    }else if(recv_byte == 0x0a) {
                        kUart1RxBuf[kUart1RxFlag & 0X3FFF] = 0;    //end char
                        kUart1RxFlag |= 0x8000;  //收到0a，完成接收
                    } else {
                        kUart1RxBuf[kUart1RxFlag & 0X3FFF] = recv_byte ;
                        kUart1RxFlag++;
                        if (kUart1RxFlag > (UART1_BUFFER_SIZE-1)) { //超出最大size
                            kUart1RxFlag = 0;
                        }
                    }
                }            
            }
        }
    } 
}

void Uart1SendByte(u8 data) {
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);    //TX BUFFER empty,if no this line,we will lost first char
    USART_SendData(USART1, data);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET); //tx over,if use TXE,we will lost last char.
}


