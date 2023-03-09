#include "main.h"

//extern UART_HandleTypeDef huart1; 			//Handler of serial port 1
//extern DMA_HandleTypeDef hdma_usart1_rx;		//DMA  handler

iapfun jump;
uint32_t addr_now = KDU_RCV_ADDR;
uint8_t  Pagedata_write[SECTOR_SIZE] = {0};    		//Large-capacity chip 2K per page
int 	 TakeDataNum=0;


//32-bit data is bit-to-sequence, such as 0111-->1110, return: data after flashback
static uint32_t revbit(u32 uData)  
{  
		 uint32_t uRevData = 0,uIndex = 0;  
		 uRevData |= ((uData >> uIndex) & 0x01);  
		 for(uIndex = 1;uIndex < 32;uIndex++)  
		 {     
			uRevData <<= 1; 
			uRevData |= ((uData >> uIndex) & 0x01);  
		 }
		 return uRevData;  
}

//Find CRC through the CRC32 calculation module that comes with STM32,
//"pData" points to the data source
//"uLen" is the data length
//"BigLittle" data format big lower end, 1 big end, 0 small end
//Output the calculated CRC32 value
uint32_t CRC32_ForWords(uint32_t *pData, uint32_t uLen, uint32_t BigLittle)  
{  
    u32 i = 0,uData = 0;  
    if((RCC->AHBPCLKEN & (1<<6)) == 0)  		//check the  CRC clock, if it is disable
    {  
        RCC->AHBPCLKEN |= 1<<6;  		 	//Enable the CRC clock
    }   
    /* Reset CRC generator */  
    CRC->CRC32CTRL = CRC32_CTRL_RESET;  		//Use 0x000000001 to Reset the CRC Data register
    for (i=0; i<uLen; i++)  
    {  
		//Change the data to small-endian data
		if(BigLittle)      
			uData = __REV(*(pData + i));  
		else  
			uData = *(pData + i);  		
      
		//Reverse the entire data and put in the CRC Data register
        CRC->CRC32DAT = revbit(uData);  
    } 
	//When writing new data to the CRC calculator, it is used as an input register; the result of the CRC calculation is returned when reading
    return revbit(CRC->CRC32DAT)^0xFFFFFFFF;  
} 
//



/*******************************************************************************
* Function name    : TakeValidData_1K
* Description: Take valid data from the serial port
* Input parameters: num is the number of extractions, full 2 times, you can write flash or less than 2K data
* Return parameter: valid data length
* Remarks: The data format is as follows ：
	Data length (2B) Data (1KB, insufficient, the host computer automatically fills in 0xFF) serial number (2B) [CRC(4B)]
	data_len_L  data_len_H  data (no more than 1K)   index_L index_H CRC
	The data participating in the verification is: data length (2B) data (1KB, insufficient for the host computer to automatically fill in 0xFF) serial number (2B)
*******************************************************************************/
uint16_t TakeValidData_1K(int num)
{
	uint32_t CRC32value, CRC32Calvalue;
	uint16_t temp = 0,i;
	uint16_t data_len = 0;	
	uint8_t data_write[1024] = {0};									//Data buffering
	CRC32value=rx1_buf[1031]<<24 | rx1_buf[1030]<<16 | rx1_buf[1029]<<8 | rx1_buf[1028];			
	CRC32Calvalue=CRC32_ForWords((u32 *)rx1_buf,(1024+4)>>2,0);
	
	if(CRC32value==CRC32Calvalue)									//Correct data verification
	{
		data_len = (uint16_t)rx1_buf[1] << 8 | rx1_buf[0];
		for(temp = 2; temp < data_len + 2; temp++)			//Copy data from the second place - this function describes incrementation of "temp" variable?
		{
			data_write[temp - 2] = rx1_buf[temp];
		}
		num=num%2;
		for(i=0;i<data_len;i++)
			 Pagedata_write[num*1024+i]=data_write[i];
	}
	else									//Error, return a special command, let the host computer resend
		data_len=0xffff;
	
	return data_len; 
}
//

void EN_Recv(void)	//Start receiving
{
	memset(rx1_buf, 0, USART1_BUF_SIZE);
	//HAL_UART_Receive_DMA(&huart1, (unsigned char*)rx1_buf, USART1_BUF_SIZE);
	USART_ConfigInt(USART1, USART_INT_RXDNE, ENABLE);
	rx1_len = 0;
	usart1_recv_end_flag=0;	
}
//Upgrade data
void Iap_Write()
{
	uint8_t  lock     = 0;
	uint32_t writeLen = 0;
	LCD_Clear(GLOBAL64);	
	LCD_ShowString0608(43, 4, "PROGRAM", 1, 128);
	FLASH_Init();
	while(1)
	{
		if(lock==0)
		{
			switch (KEY_SCAN(0))
			{
				case key_clr:
					printf("不升级，退出\n");		//The translation is: "Do not upgrade, exit"
					EN_Recv();
					//usart1_recv_end_flag = 0;
				return;
			}
		}
		if(usart1_recv_end_flag)				     //Received data
		{
			//usart1_recv_end_flag=0;	
			if(rx1_len<1024)				     //The receiving length is less than 1024 and it is judged as a command
			{
				if(strstr(rx1_buf, "LineKDU"))
				{
					lock=1;
					EN_Recv();
					printf("%cReady", 5);		     //Ready to receive data
				}
				else if(strstr(rx1_buf, "Endfile"))
				{
					lock=0;
//////////////////////////////////////////////////////////////////////////////////////////////
					char datatemp[10]={0};
					u8 i=7, j=0;
//					uint32_t 		num 	= 0;
					uint32_t 	  ReadDat 	= NEW_APP;
					__IO uint32_t useraddr  = KDU_FLAG_ADDR;
					
															
					FLASH_EraseUser(KDU_FLAG_ADDR);
					FLASH_WriteUser(KDU_FLAG_ADDR,   		 &ReadDat, 1);		//Write to the 0x55 flag
					while(rx1_buf[i] != '\n')
					{
						datatemp[j]=rx1_buf[i];
						i++, j++;
					}
					ReadDat = atoi(datatemp);						//Upgrade data size
					delay_ms(100);
					FLASH_WriteUser(KDU_SIZE_ADDR, 			 &ReadDat, 1);

//////////////////////////////////////////////////////////////////////////////////////////////////////	
					
					EN_Recv();
					printf("%cOK", 2);
					LCD_ShowString0608(37, 4, "UPGRADING", 1, 128);
					delay_ms(1000);
					ResetSystem();
				}
				else
				{
					lock=0;
					EN_Recv();
					printf("%cUNKNOW", 6);
					//UART1_Send_Message(rx1_buf, rx1_len);
				}
			}
			else 								//Received data and 8-digit verification or other data
			{
				writeLen = TakeValidData_1K(TakeDataNum++);		//Actual programming data length
				if(writeLen==0xffff)		 			//Verify the error, download again, continue to wait for the data
				{
					TakeDataNum--;
					EN_Recv();
					printf("%cAgain", 5);
					continue;
				}
				else							//Verification was successful
				{
					if(writeLen<1024 || TakeDataNum>=2)
					{
						if(writeLen%2 != 0)			//Prevent single-digit download, so +1
							writeLen += 1;
						if(TakeDataNum == 2)			//If this is the second half of a page
							writeLen += 1024;
						
						if(FLASH_WriteUser(addr_now, (uint32_t*)Pagedata_write, writeLen/FlashByte) != FLASH_OK)
							while(1);
						
						addr_now += writeLen;			//Programming location
						TakeDataNum=0;
					}
					EN_Recv();
					printf("%cNext", 4);
				}
				//
			}
			
			
		}
	}
}
//

//Jump to app
void Iap_load(uint32_t addr)
{	
	if(((*(vu32*)addr) & 0x2FFE0000) == 0x20000000)			//Check whether the address on the top of the stack is legal
	{
//		D_printf("检查栈顶合法!%x\n", *(vu32*)addr);		   //Translation is: "Check that the top of the stack is legal"
									//The second word in the user code area is the program start address (reset address)
		
		jump = (iapfun) *(vu32*)(addr + 4);			//Force conversion to function 
//		D_printf("复位地址合法!%x\n", *(vu32*)(addr + 4));	   //Translation is: "Reset address is legal"
		
		MSR_MSP(*(vu32*)addr);					//Initialize the APP stack pointer (the first word in the user code area is used to store the top address of the stack)
		D_printf("初始化APP堆栈指针！跳转到APP！\n");	      //Translation is: "Initialize the APP stack pointer! Jump to the APP!"
		jump();
	}
	else
	{
//		D_printf("检查栈顶出错！%x\n", *(vu32*)addr);		  //Translation is: "Check for errors on the top of the stack"
//		D_printf("复位地址出错！%x\n", *(vu32*)(addr + 4));	  //Translation is: "Reset address error"
		D_printf("%x\n", ((*(vu32*)addr) & 0x2FFE0000));
		printf("Enter app error\n");
		ResetSystem();
	}
	return;	
}
//
void KDUCheckRunMaster(uint32_t addr)
{
	//Detect whether it is flashed into 152, if yes, shut down		
//	if(((*(vu32*)addr) & 0x2FFE0000) == 0x20000000)
	{
		NVIC_SystemReset();
	}
}
//

