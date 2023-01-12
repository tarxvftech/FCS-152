#include "main.h"

int LAMP_TIME   = 10000;
volatile u8  BL = 100;

void DumpClock(const char* msg);
int main()
{
	//串口初始化		//115200
	log_init();
	D_printf("\n<<<<<<BootLoader>>>>>>\n");	//1.打开串口,初始化打印信息
	//配置使能HSI时钟，测试发现只有使用HSI时钟的情况下对Flash操作才不会卡死
	if(FLASH_HSICLOCK_DISABLE == FLASH_ClockInit())
    {
        printf("HSI oscillator not yet ready\r\n");
        while(1);
    }

//    if(!FLASH_GetReadOutProtectionSTS())
//    {
//        FLASH_Unlock();
//        FLASH_ReadOutProtectionL1(ENABLE);
//        FLASH_Lock();
//    }

    
	//检测新程序, 存在则复制并跳转; 不存在则检测APP是否正确:正确跳转，错误进入升级模式
	bootToUpgrade();
	

/**************************如果没跳转则进入升级模式*****************************/
	
	//如果没有升级，重启后根据以下标志位依旧		首先检测 运行APP 内存是否存在程序
	//如果升级，	   重启后根据接收完后写入的标志位	首先检测 接收APP 内存是否存在程序
	uint32_t ReadDat = RUN_APP;
    if(*(vu32*)KDU_FLAG_ADDR != RUN_APP)
        FLASH_WriteUser(KDU_FLAG_ADDR, &ReadDat,  1);
	
	//延时、分频等参数配置 SysTick初始化			//不配置HSE，直接使用上面使能的HSI进行操作
	delay_init();
	
	//串口接收中断监控
	TIM6_Init();	

	//LCD12864初始化
	LCD_Init();		
	//背光灯初始化
	LCDK_Init();	
	//按键灯初始化
	LEDK_Init();
	BackLight_SetVal(BL);
	//按键键盘初始化
	KEY_Init();
	
	Iap_Write();
	
	//不会到这！
	ResetSystem();

	
/*	
	int key_val = 0;
	while (1)
	{
		if(bsp_CheckTimer(TMR_ASK))
		{
			BackLight_SetVal(BL);
			BL = (BL+100)%200;
			printf("hello\n");
		}
//		printf("********************");
		key_val = KEY_SCAN(0);
		if(key_val != key_no)
			printf("key:%d\n", key_val);
		//UART1_Put_Char(key_val);
		//delay_ms(300);
	}
*/

}

void bootToUpgrade(void)//启动检测升级
{
///////////////////////////////////////////////////////////////////
	uint32_t ReadDat    = 0x57;						//读数据间接变量
	uint32_t UpdaCnt    = 0;						//The size of new app
	uint32_t read2write[SECTOR_SIZE/FlashByte];		//复制缓冲区
	__IO u32 addr1  	=  KDU_RUN_ADDR;			//The address where the app starts
	__IO u32 addr2  	=  KDU_RCV_ADDR;			//The address of new app where receive 
	__IO u32 useraddr 	=  KDU_FLAG_ADDR;			//The address of app flag
	
	
	
	FLASH_ReadUser(KDU_FLAG_ADDR, &ReadDat, 1);
	D_printf("%#x\n", ReadDat16);
	
	if(ReadDat == NEW_APP)//Exam the flag if there is a new app
	{
		FLASH_EraseUser(KDU_RUN_ADDR);							//擦除运行空间
		FLASH_ReadUser(KDU_SIZE_ADDR, &UpdaCnt, 1);				//读取APP升级包大小
		D_printf("There is a new app, which data sizes=%d\r\n", UpdaCnt);
		
		addr1 = KDU_RUN_ADDR;
	    addr2 = KDU_RCV_ADDR;
		
		D_printf("Copying program in progress....\r\n");
			
		 
		int runtime = round(UpdaCnt/SECTOR_SIZE+0.5);			//读取次数，向上取整
		for(int i=0; i<runtime; i++)
		{
			FLASH_ReadUser (addr2,  read2write,  SECTOR_SIZE/FlashByte);
			FLASH_WriteUser(addr1,  read2write,  SECTOR_SIZE/FlashByte);
			addr1 += SECTOR_SIZE;
			addr2 += SECTOR_SIZE;
			memset(read2write, 0xffffffff, SECTOR_SIZE/FlashByte);
		}
		D_printf("Program has been copied!!!\r\n");	  

		
		addr2 = KDU_RCV_ADDR;
		FLASH_EraseUser(addr2);
		D_printf("Erase the storage space, size:%dK....\r\n", APP_PAGE_SIZE/STM_SECTOR_SIZE);
	}
	//
	
	if(((*(vu32*)KDU_RUN_ADDR) & 0x2FFE0000) == 0x20000000)						//检查栈顶是否合法
	{
INTX_DISABLE();
USART_Enable(USART1, DISABLE);
USART_DeInit(USART1);
TIM_Enable(TIM6, DISABLE);
TIM_DeInit(TIM6);
		
//		printf("The stack is legal! %x\n", *(vu32*)KDU_RUN_ADDR);				//用户代码区第二个字为程序开始地址(复位地址)
		jump = (iapfun) *(vu32*)(KDU_RUN_ADDR + 4);								//强制转化为函数  
//		printf("The RESET address is legal! %x\n", *(vu32*)(KDU_RUN_ADDR + 4));
		MSR_MSP(*(vu32*)KDU_RUN_ADDR);											//初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
//		printf("Initialize the APP stack pointer!Jump to APP!\n");
		ReadDat=RUN_APP;
		FLASH_WriteUser(KDU_FLAG_ADDR, &ReadDat, 1);							//写入"运行app"命令
		jump();
	}
	else
	{
		printf("Enter app error\n");
		return;
//		ReadDat16=WRO_APP;
//		STMFLASH_Write(KDU_FLAG_ADDR, &ReadDat16, 1);							//写入"错误app"命令
	}
//
}
//

void DumpClock(const char* msg)
{
	RCC_ClocksType RCC_ClockFreq;
    log_init(); // should reinit after sysclk changed
    log_info("--------------------------------\n");
    log_info("%s:\n", msg);
    RCC_GetClocksFreqValue(&RCC_ClockFreq);
    log_info("SYSCLK: %d\n", RCC_ClockFreq.SysclkFreq);
    log_info("HCLK: %d\n",   RCC_ClockFreq.HclkFreq);
    log_info("PCLK1: %d\n",  RCC_ClockFreq.Pclk1Freq);
    log_info("PCLK2: %d\n",  RCC_ClockFreq.Pclk2Freq);
}

//void HardFault_Handler()
//{
////	printf("HardFault_Handler\n");
////	__asm volatile ("BKPT #1");
//	while(1);

//}
