#include "main.h"

//extern UART_HandleTypeDef huart1; 			//串口1的handler
//extern DMA_HandleTypeDef hdma_usart1_rx;		//DMA  的handler

iapfun jump;
uint32_t addr_now = KDU_RCV_ADDR;
uint8_t  Pagedata_write[SECTOR_SIZE] = {0};    //大容量芯片一页2K
int 	 TakeDataNum=0;


//32位数据进行位到序   如0111-->1110 , 返回:倒叙后的数据
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

//通过STM32内部自带CRC32计算模块求CRC, 
// pData指向数据源
// uLen为数据长度 
// BigLittle数据格式大下端,1大端，0小端
// 输出计算的CRC32数值
uint32_t CRC32_ForWords(uint32_t *pData, uint32_t uLen, uint32_t BigLittle)  
{  
    u32 i = 0,uData = 0;  
    if((RCC->AHBPCLKEN & (1<<6)) == 0)  	//check the  CRC clock, if it is disable
    {  
        RCC->AHBPCLKEN |= 1<<6;  		 	//Enable the CRC clock
    }   
    /* Reset CRC generator */  
    CRC->CRC32CTRL = CRC32_CTRL_RESET;  			//Use 0x000000001 to Reset the CRC Data register
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
	//写入 CRC 计算器的新数据时，作为输入寄存器;读取时返回 CRC 计算的结果
    return revbit(CRC->CRC32DAT)^0xFFFFFFFF;  
} 
//



/*******************************************************************************
* 函数名    : TakeValidData_1K
* 描述	   : 从串口取有效数据
* 输入参数  : num为提取次数，满2次，则可以写flash 或者不足2K数据时
* 返回参数  : 有效数据长度
*备注：数据格式如下 ：
					数据长度(2B) 数据(1KB，不足上位机自动填充0xFF) 序号(2B) [CRC(4B)]
					data_len_L  data_len_H  data(no more than 1K)   index_L index_H CRC
					参与校验数据为：数据长度(2B) 数据(1KB，不足上位机自动填充0xFF) 序号(2B)
*******************************************************************************/
uint16_t TakeValidData_1K(int num)
{
	uint32_t CRC32value, CRC32Calvalue;
	uint16_t temp = 0,i;
	uint16_t data_len = 0;	
	uint8_t data_write[1024] = {0};														//数据缓冲
	CRC32value=rx1_buf[1031]<<24 | rx1_buf[1030]<<16 | rx1_buf[1029]<<8 | rx1_buf[1028];			
	CRC32Calvalue=CRC32_ForWords((u32 *)rx1_buf,(1024+4)>>2,0);
	
	if(CRC32value==CRC32Calvalue)//数据校验正确
	{
		data_len = (uint16_t)rx1_buf[1] << 8 | rx1_buf[0];
		for(temp = 2; temp < data_len + 2; temp++)								//从第二位开始拷贝数据
		{
			data_write[temp - 2] = rx1_buf[temp];
		}
		num=num%2;
		for(i=0;i<data_len;i++)
			 Pagedata_write[num*1024+i]=data_write[i];
	}
	else//错误,返回特殊命令，让上位机重发
		data_len=0xffff;
	
	return data_len; 
}
//

void EN_Recv(void)	//启动接收
{
	memset(rx1_buf, 0, USART1_BUF_SIZE);
	//HAL_UART_Receive_DMA(&huart1, (unsigned char*)rx1_buf, USART1_BUF_SIZE);
	USART_ConfigInt(USART1, USART_INT_RXDNE, ENABLE);
	rx1_len = 0;
	usart1_recv_end_flag=0;	
}
//升级数据
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
					printf("不升级，退出\n");
					EN_Recv();
					//usart1_recv_end_flag = 0;
				return;
			}
		}
		if(usart1_recv_end_flag)//收到数据
		{
			//usart1_recv_end_flag=0;	
			if(rx1_len<1024)				   			//接收长度小于1024判断为命令
			{
				if(strstr(rx1_buf, "LineKDU"))
				{
					lock=1;
					EN_Recv();
					printf("%cReady", 5);					//准备完毕，可以接收数据
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
					FLASH_WriteUser(KDU_FLAG_ADDR,   		 &ReadDat, 1);	//写入0x55标志位
					while(rx1_buf[i] != '\n')
					{
						datatemp[j]=rx1_buf[i];
						i++, j++;
					}
					ReadDat = atoi(datatemp);								//升级数据大小
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
			else 												//收到数据和8位校验	或者其他数据
			{
				writeLen = TakeValidData_1K(TakeDataNum++);		//实际烧写数据长度
				if(writeLen==0xffff)		 					//校验错误，重新下载,continue等待数据
				{
					TakeDataNum--;
					EN_Recv();
					printf("%cAgain", 5);
					continue;
				}
				else													  //校验成功
				{
					if(writeLen<1024 || TakeDataNum>=2)
					{
						if(writeLen%2 != 0)								//防止单数下载故+1
							writeLen += 1;
						if(TakeDataNum == 2)							//如果这是一页的下半页
							writeLen += 1024;
						
						if(FLASH_WriteUser(addr_now, (uint32_t*)Pagedata_write, writeLen/FlashByte) != FLASH_OK)
							while(1);
						
						addr_now += writeLen;					//烧写位置
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

//跳转app
void Iap_load(uint32_t addr)
{	
	if(((*(vu32*)addr) & 0x2FFE0000) == 0x20000000)			//检查栈顶地址是否合法.
	{
//		D_printf("检查栈顶合法!%x\n", *(vu32*)addr);			//用户代码区第二个字为程序开始地址(复位地址)
		jump = (iapfun) *(vu32*)(addr + 4);					//强制转化为函数 
//		D_printf("复位地址合法!%x\n", *(vu32*)(addr + 4));
		MSR_MSP(*(vu32*)addr);								//初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
		D_printf("初始化APP堆栈指针！跳转到APP！\n");
		jump();
	}
	else
	{
//		D_printf("检查栈顶出错！%x\n", *(vu32*)addr);	
//		D_printf("复位地址出错！%x\n", *(vu32*)(addr + 4));
		D_printf("%x\n", ((*(vu32*)addr) & 0x2FFE0000));
		printf("Enter app error\n");
		ResetSystem();
	}
	return;	
}
//
void KDUCheckRunMaster(uint32_t addr)
{
	//检测是否被刷入152中,如果是的话则关机
//	if(((*(vu32*)addr) & 0x2FFE0000) == 0x20000000)
	{
		NVIC_SystemReset();
	}
}
//



















