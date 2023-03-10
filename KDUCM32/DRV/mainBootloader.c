#include "main.h"

int LAMP_TIME   = 10000;
volatile u8  BL = 100;

void DumpClock(const char* msg);
int main()
{
	//Serial port initialization	//115200
	log_init();
	D_printf("\n<<<<<<BootLoader>>>>>>\n");		//Open the serial port and initialize the printing information
							//Configure to enable the HSI clock, and the test found that the Flash operation will not get stuck only 
							//if the HSI clock is used.
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

    
	//Detect the new program, copy and jump if it exists; detect whether the APP is correct if it does not exist: 
	//jump correctly, enter the upgrade mode by mistake
	bootToUpgrade();
	

/**************************If there is no jump, enter upgrade mode*****************************/
	
	//If there is no upgrade, after restarting, according to the following flags, first detect whether there is a program in the memory of the running APP.
	//If you upgrade, after restarting, first detect whether there is A program in the memory of the RECEIVING APP according to the flag written after receiving IT.
	uint32_t ReadDat = RUN_APP;
    if(*(vu32*)KDU_FLAG_ADDR != RUN_APP)
        FLASH_WriteUser(KDU_FLAG_ADDR, &ReadDat,  1);
	
	//Configure SysTick initialization for parameters such as delay and frequency division			
	//Do not configure HSE, directly use the HSI enabled above to operate
	delay_init();
	
	//Serial port reception interrupt monitoring
	TIM6_Init();	

	//LCD12864 initialization
	LCD_Init();		
	//Backlight initialization
	LCDK_Init();	
	//Button light initialization
	LEDK_Init();
	BackLight_SetVal(BL);
	//Key keyboard initialization
	KEY_Init();
	
	Iap_Write();
	
	//Not here! - What is it?
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

void bootToUpgrade(void)						//Start the detection upgrade
{
///////////////////////////////////////////////////////////////////
	uint32_t ReadDat    = 0x57;					//Read data indirect variables
	uint32_t UpdaCnt    = 0;					//The size of new app
	uint32_t read2write[SECTOR_SIZE/FlashByte];			//Copy buffer
	__IO u32 addr1  	=  KDU_RUN_ADDR;			//The address where the app starts
	__IO u32 addr2  	=  KDU_RCV_ADDR;			//The address of new app where receive 
	__IO u32 useraddr 	=  KDU_FLAG_ADDR;			//The address of app flag
	
	
	
	FLASH_ReadUser(KDU_FLAG_ADDR, &ReadDat, 1);
	D_printf("%#x\n", ReadDat16);
	
	if(ReadDat == NEW_APP)						//Exam the flag if there is a new app
	{
		FLASH_EraseUser(KDU_RUN_ADDR);						//Erase running space
		FLASH_ReadUser(KDU_SIZE_ADDR, &UpdaCnt, 1);				//Read the size of the APP upgrade package
		D_printf("There is a new app, which data sizes=%d\r\n", UpdaCnt);
		
		addr1 = KDU_RUN_ADDR;
	    	addr2 = KDU_RCV_ADDR;
		
		D_printf("Copying program in progress....\r\n");
			
		 
		int runtime = round(UpdaCnt/SECTOR_SIZE+0.5);				//Number of readings, rounded up
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
	
	if(((*(vu32*)KDU_RUN_ADDR) & 0x2FFE0000) == 0x20000000)				//Check whether the top of the stack is legal
	{
INTX_DISABLE();
USART_Enable(USART1, DISABLE);
USART_DeInit(USART1);
TIM_Enable(TIM6, DISABLE);
TIM_DeInit(TIM6);
		
//		printf("The stack is legal! %x\n", *(vu32*)KDU_RUN_ADDR);			//The second word in the user code area is the program start address (reset address)
		jump = (iapfun) *(vu32*)(KDU_RUN_ADDR + 4);					//Force conversion to function
//		printf("The RESET address is legal! %x\n", *(vu32*)(KDU_RUN_ADDR + 4));
		MSR_MSP(*(vu32*)KDU_RUN_ADDR);							//Initialize the APP stack pointer (the first word in the user code area is used to store the top address of the stack)
//		printf("Initialize the APP stack pointer!Jump to APP!\n");
		ReadDat=RUN_APP;
		FLASH_WriteUser(KDU_FLAG_ADDR, &ReadDat, 1);					//Write the "Run app" command
		jump();
	}
	else
	{
		printf("Enter app error\n");
		return;
//		ReadDat16=WRO_APP;
//		STMFLASH_Write(KDU_FLAG_ADDR, &ReadDat16, 1);					//Write the "Error app" command
	}
//
}
//

void DumpClock(const char* msg)
{
	RCC_ClocksType RCC_ClockFreq;
    log_init(); 							// should reinit after sysclk changed
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
