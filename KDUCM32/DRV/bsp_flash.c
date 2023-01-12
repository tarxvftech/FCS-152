#include "bsp_flash.h"

u32 FLASH_BUF[SECTOR_SIZE/FlashByte];	//缓冲区，一页字节大小，不能超过2K字节.
/**
 * @brief  clear flash errors
 * @param  None
 * @retval None
 */
void FLASH_Init(void) {
	
	FLASH_ClockInit();
	/* Unlock the Program memory */
	FLASH_Unlock();

	/* Clear all FLASH flags */
	FLASH_ClearFlag(
			FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR
					| FLASH_FLAG_EVERR | FLASH_FLAG_PVERR);
	/* Unlock the Program memory */
	FLASH_Lock();
}
//
uint32_t FLASH_ReadWord(uint32_t faddr)
{
	return *(vu32*)faddr; 
}
//读取
uint32_t FLASH_ReadUser(uint32_t addr, uint32_t *data, int32_t NumToRead)
{
	for(int i=0; i<NumToRead; i++)
	{
		data[i] = FLASH_ReadWord(addr);			//读取FlashByte个字节.
		addr+=FlashByte;						//偏移FlashByte个字节.	
	}
	return FLASH_OK;
}
//擦除
uint32_t FLASH_EraseUser(uint32_t faddr)
{
	uint32_t pageAddr = faddr;
	uint32_t max_page = 0;
	FLASH_Unlock();
	switch(faddr)
	{
		case KDU_FLAG_ADDR:		//用户标志区间
			 max_page = USER_PAGE_SIZE/SECTOR_SIZE;
		break;
		
		case KDU_RUN_ADDR:		//APP运行区间
		case KDU_RCV_ADDR:		//升级APP接收区间
			max_page = APP_PAGE_SIZE/SECTOR_SIZE;
		break;
		
		default:				//其他地址，则向下取整，擦除一页
			max_page = 1;
			faddr = ((uint32_t)(faddr/SECTOR_SIZE))*SECTOR_SIZE;	
			break;
	}
	
	for(int i = 0; i < max_page ; i++)
	{
		pageAddr = faddr+i*SECTOR_SIZE;
		if (FLASH_COMPL != FLASH_EraseOnePage(pageAddr)) 
		{
			FLASH_Lock();
			return FLASH_ERASE_ERROR;
		}
	}
	FLASH_Lock();
	return FLASH_OK;
}
//写入
uint32_t FLASH_WriteUser(uint32_t addr, uint32_t *data, int32_t NumToWrite)
{
	//非法地址
	if(addr<FLASH_BASE || (addr >= (FLASH_BASE + SECTOR_SIZE*FLASH_PAGE_SIZE)))
		return FLASH_WRITING_ERROR;
	
	uint32_t secpos;					//页   地址
	uint32_t secoff;					//页内 偏移地址
	uint32_t secremain;					//页内 剩余地址	   
 	uint32_t i;							//迭代器
	uint32_t offaddr;					//去掉0X08000000后的地址
	uint32_t status = FLASH_OK;			//擦写状态
	uint32_t OPTAddr = 0;
	
	FLASH_Unlock();										//解锁
	offaddr   = addr-FLASH_BASE;						//实际	偏移地址.
	secpos    = offaddr/SECTOR_SIZE;					//页	偏移地址
	secoff    = (offaddr%SECTOR_SIZE)/FlashByte;		//页内	偏移地址(FlashByte个字节为基本单位.)
	secremain = SECTOR_SIZE/FlashByte - secoff;			//扇区剩余空间大小 
	
	if(NumToWrite <= secremain)							//需要写入长度比剩余空间小
		secremain = NumToWrite;							//按照写入长度统计剩余空间
	
	while(1)
	{
		OPTAddr = FLASH_BASE+secpos*SECTOR_SIZE;
		FLASH_ReadUser(OPTAddr, FLASH_BUF, SECTOR_SIZE/FlashByte);	//读出一整页的内容
		for(i=0; i<secremain; i++)						//校验剩余空间内的擦除状况
		{
			if(FLASH_BUF[secoff+i] != 0XFFFFFFFF)		//要写入数据的地址不为空 		
				break;
		}
		if(i<secremain)									//需要擦除
		{
			OPTAddr = FLASH_BASE+secpos*SECTOR_SIZE;
			if (FLASH_COMPL != FLASH_EraseOnePage(OPTAddr))		//擦除这个扇区
			{
				status = FLASH_ERASE_ERROR;
				break;
			}
			else										//擦除成功
			{
				for(i=0; i<secremain; i++)				
				{
					FLASH_BUF[secoff+i] = data[i];		//将要写入的内容追加在 缓冲区后面
				}
				for(i=0; i<secoff+secremain; i++)		//数据写入和校验
				{
					OPTAddr = FLASH_BASE + secpos*SECTOR_SIZE + i*FlashByte;
					//写入
					if(FLASH_ProgramWord(OPTAddr, FLASH_BUF[i]) == FLASH_COMPL)
					{
						if(FLASH_ReadWord(OPTAddr) != FLASH_BUF[i])
						{	//校验出错
							status = FLASH_WRITING_ERROR;
							break;
						}
					}
					else
					{		//写入出错
						status = FLASH_WRITING_ERROR;
						break;
					}
				}
				if(status != FLASH_OK)
					break;
			}
		}
		else											//不需要擦除
		{
			for(i=0; i<secremain; i++)					//数据直接写入和校验
			{
				OPTAddr = FLASH_BASE + secpos*SECTOR_SIZE + secoff*FlashByte + i*FlashByte;
				//写入
				if(FLASH_ProgramWord(OPTAddr, data[i]) == FLASH_COMPL)
				{
					if(FLASH_ReadWord(OPTAddr) != data[i])
					{	//校验出错
						status = FLASH_WRITING_ERROR;
						break;
					}
				}
				else
				{		//写入出错
					status = FLASH_WRITING_ERROR;
					break;
				}
			}
		
		}
		//处理数据偏移矢量
		if(NumToWrite == secremain)						//剩余写入字节 等于 剩余空间，表示数据写入完成
			break;					
		else	//更新矢量
		{
			secpos ++;									//页   地址 增1	√
			secoff = 0;									//页内 偏移 清0	√
			data  += secremain;							//数据指针偏移	√
			addr  += secremain*FlashByte;				//地址偏移
			NumToWrite -= secremain;					//写入长度递减	√用于下面统计剩余空间	
			
			if(NumToWrite > (SECTOR_SIZE/FlashByte))
				secremain = SECTOR_SIZE/FlashByte;		//下一个扇区还是写不完
			else 
				secremain = NumToWrite;					//下一个扇区可以写完了
		}
	}
	
	FLASH_Lock();//上锁
	return status;
}
//


//THUMB指令不支持汇编内联
//采用如下方法实现执行汇编指令WFI  
void WFI_SET(void)
{
	__ASM volatile("wfi");		  
}
//关闭所有中断
void INTX_DISABLE(void)
{		  
	//__ASM volatile("cpsid i");
	__disable_irq();
}
//开启所有中断
void INTX_ENABLE(void)
{
	//__ASM volatile("cpsie i");	
	__enable_irq();	
}
//设置栈顶地址
//addr:栈顶地址
#ifdef COMPLIER_6
void MSR_MSP(u32 addr) 
{
    __asm("MSR MSP, r0"); 			//set Main Stack value
    __asm("BX r14");
}
#else
__asm void MSR_MSP(u32 addsr) 
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}
#endif


