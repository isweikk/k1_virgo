/**
 * the init of system
 * the function is usefull to every api.
 */

#include "sys.h" 


/*----------------------------------------------------------------------------*/
static u8  kTicksPerUs=0;	//us延时倍乘基数，每us的tick数值
static u16 kTicksPerMs=0;	//ms

/******************************************************************************
  * @func    void Delay_init(u8 SysCLK);
  * @brief   init the delay function
  * @author  Eric  @date  08-April-2014
  * @info    SYSTICK的时钟固定为HCLK时钟SYSCLK/8,SysCLK为系统时8M的9倍频
*******************************************************************************/
/*------------------------------general model---------------------------------*/
void DelayInit(void) {
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);   //systick时钟源=sysclk/8
    kTicksPerUs = SystemCoreClock/8000000;
    kTicksPerMs = (u16)kTicksPerUs*1000;	//每个ms需要的systick计数
}								 

void DelayUs(u32 us) {		
    u32 temp;	    	 
    SysTick->LOAD = us * kTicksPerUs;	 
    SysTick->VAL =0x00;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk ;
    do {
        temp = SysTick->CTRL;
    } while ((temp & 0x01)&& !(temp &(1<<16)));
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    SysTick->VAL =0X00;
}

//SysTick->LOAD is 24bit,the max delay is
//nms<=0xffffff*8*1000/SysCLK
//SysCLK单位为Hz,nms单位为ms
//if sysclk is 72M, nms<=1864 
void DelayMs(u16 ms) {	 		  	  
    u32 temp, temp_ms;
    for (; ms>0;) {
        // support to delay longer time.
        if (ms >= 1500) {
            temp_ms = 1500;
            ms -= 1500;
        } else {
            temp_ms = ms;
            ms = 0;
        }
        SysTick->LOAD =(u32)temp_ms * kTicksPerMs;//load the top value(SysTick->LOAD is 24bit)
        SysTick->VAL =0x00;
        SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk ;         //start count
        do{
            temp = SysTick->CTRL;
        }while((temp & 0x01) &&! (temp &(1<<16)));//time reach
        SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;       //disable counter
        SysTick->VAL = 0X00;       //clear count
    }
}

/*******************************************************************************
 *sysConfig()
 *******************************************************************************/
void SysConfig(void) {
    SysRccConfig(); //system clock init.
    SysNvicConfig();
    JtagConfig(EmJtagOnlySwd);
}

//设置NVIC分组
//NVIC_Group:NVIC分组 0~4 总共5种方式
void SysNvicConfig(void) {   
#ifdef  VECT_TAB_RAM  
    /* Set the Vector Table base location at 0x20000000 */ 
    NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
#else  /* VECT_TAB_FLASH  */
    /* Set the Vector Table base location at 0x08000000 */ 
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
#endif

    /* Configure one bit for preemption priority */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//set the NVIC group 2, two preemptive ,two response 
}

//进入待机模式	  
void SysStandby(void) {
    SCB->SCR |= (1<<2);					 //使能SLEEPDEEP(SYS->CTRL)	   
    RCC->APB1ENR |= (1<<28);     //使能电源时钟	    
     PWR->CSR |= (1<<8);          //设置WKUP用于唤醒
    PWR->CR |= (1<<2);           //清除Wake-up 标志
    PWR->CR |= (1<<1);           //PDDS置位	
    //WFI;				 //执行WFI指令
}	     
//system soft reset
void SysSoftReset(void) {
    SCB->AIRCR = 0X05FA0000|(u32)0x04;	  
} 		 
//JTAG模式设置
//mode:jtag,swd模式设置;00,全使能;01,使能SWD;10,全关闭.
void JtagConfig(EnumJtagMode mode) {
    u32 temp;
    temp = mode;
    temp <<= 25;
    RCC->APB2ENR |= (1<<0);   //开启辅助时钟
    AFIO->MAPR &= 0XF8FFFFFF; //清除MAPR的[26:24]
    AFIO->MAPR |= temp;       //设置jtag模式
}

/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    : Configures the different system clocks.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SysRccConfig(void) {
    ErrorStatus HSEStartUpStatus;
    /* RCC system reset(for debug purpose) */
    RCC_DeInit();
    /* Enable HSE */
    RCC_HSEConfig(RCC_HSE_ON);
    /* Wait till HSE is ready */
    HSEStartUpStatus = RCC_WaitForHSEStartUp();
    if(HSEStartUpStatus == SUCCESS) {
        /* Enable Prefetch Buffer */
        FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
        /* Flash 2 wait state */
        FLASH_SetLatency(FLASH_Latency_2);
        /* HCLK = SYSCLK */
        RCC_HCLKConfig(RCC_SYSCLK_Div1); 
        /* PCLK2 = HCLK */
        RCC_PCLK2Config(RCC_HCLK_Div1); 
        /* PCLK1 = HCLK/2 */
        RCC_PCLK1Config(RCC_HCLK_Div2);
        /* PLLCLK = 8MHz * 9 = 72 MHz */
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
        /* Enable PLL */ 
        RCC_PLLCmd(ENABLE);
        /* Wait till PLL is ready */
        while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
        /* Select PLL as system clock source */
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
        /* Wait till PLL is used as system clock source */
        while(RCC_GetSYSCLKSource() != 0x08);
    }
}
/*
//系统时钟初始化函数
//pll:选择的倍频数，2-16	 
void SysClockInit(void)
{
    unsigned char temp =0;
    unsigned int clkPll = SYS_CLK_PLL;
    
    sysRccConfig();		  //复位并配置向量表
     RCC->CR |= 0x00010000;  //外部高速时钟使能HSEON
    while(!(RCC->CR >>17));//等待外部时钟就绪
    RCC->CFGR = 0X00000400; //APB1=DIV2;APB2=DIV1;AHB=DIV1;
    clkPll -= 2;//抵消2个单位
    RCC->CFGR |= (clkPll<<18);   //set PLL = 2~16
    RCC->CFGR |= (1<<16);	  //PLLSRC ON 
    FLASH->ACR |= 0x32;	  //FLASH 2个延时周期

    RCC->CR |= 0x01000000;  //PLLON
    while(!(RCC->CR >>25));//等待PLL锁定
    RCC->CFGR |= 0x00000002;//PLL作为系统时钟	 
    while(temp != 0x02)     //等待PLL作为系统时钟设置成功
    {   
        temp = (RCC->CFGR>>2);
        temp &= 0x03;
    }    
}
*/















