#include "bsp_flash.h"

u32 FLASH_BUF[SECTOR_SIZE/FlashByte];	//Buffer, the size of a page of bytes, cannot exceed 2K bytes
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
//Read
uint32_t FLASH_ReadUser(uint32_t addr, uint32_t *data, int32_t NumToRead)
{
	for(int i=0; i<NumToRead; i++)
	{
		data[i] = FLASH_ReadWord(addr);			//Read FlashByte bytes
		addr+=FlashByte;				//Offset FlashByte bytes	
	}
	return FLASH_OK;
}
//Erase
uint32_t FLASH_EraseUser(uint32_t faddr)
{
	uint32_t pageAddr = faddr;
	uint32_t max_page = 0;
	FLASH_Unlock();
	switch(faddr)
	{
		case KDU_FLAG_ADDR:				//User logo interval
			 max_page = USER_PAGE_SIZE/SECTOR_SIZE;
		break;
		
		case KDU_RUN_ADDR:				//APP running interval
		case KDU_RCV_ADDR:				//Upgrade the APP receiving interval
			max_page = APP_PAGE_SIZE/SECTOR_SIZE;
		break;
		
		default:					//For other addresses, round down and erase one page
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
//Write
uint32_t FLASH_WriteUser(uint32_t addr, uint32_t *data, int32_t NumToWrite)
{
	//Illegal address
	if(addr<FLASH_BASE || (addr >= (FLASH_BASE + SECTOR_SIZE*FLASH_PAGE_SIZE)))
		return FLASH_WRITING_ERROR;
	
	uint32_t secpos;					//Page address
	uint32_t secoff;					//In-page offset address
	uint32_t secremain;					//Remaining addresses in the page	   
 	uint32_t i;						//Iterator
	uint32_t offaddr;					//Remove the address after 0x08000000
	uint32_t status = FLASH_OK;				//Erase status
	uint32_t OPTAddr = 0;
	
	FLASH_Unlock();							//Unlock
	offaddr   = addr-FLASH_BASE;					//Actual offset address
	secpos    = offaddr/SECTOR_SIZE;				//Page offset address
	secoff    = (offaddr%SECTOR_SIZE)/FlashByte;			//The offset address in the page (FlashByte bytes are the basic unit)
	secremain = SECTOR_SIZE/FlashByte - secoff;			//The size of the remaining space in the sector 
	
	if(NumToWrite <= secremain)					//The required write length is smaller than the remaining space
		secremain = NumToWrite;					//Count the remaining space according to the write length
	
	while(1)
	{
		OPTAddr = FLASH_BASE+secpos*SECTOR_SIZE;
		FLASH_ReadUser(OPTAddr, FLASH_BUF, SECTOR_SIZE/FlashByte);		//Read out a whole page of content
		for(i=0; i<secremain; i++)						//Verify the erasure status in the remaining space
		{
			if(FLASH_BUF[secoff+i] != 0XFFFFFFFF)				//The address to write data is not empty 		
				break;
		}
		if(i<secremain)								//Need to be erased
		{
			OPTAddr = FLASH_BASE+secpos*SECTOR_SIZE;
			if (FLASH_COMPL != FLASH_EraseOnePage(OPTAddr))			//Erase this sector
			{
				status = FLASH_ERASE_ERROR;
				break;
			}
			else								//Erase successfully
			{
				for(i=0; i<secremain; i++)				
				{
					FLASH_BUF[secoff+i] = data[i];			//Appends the content to be written to the buffer
				}
				for(i=0; i<secoff+secremain; i++)			//Data writing and verification
				{
					OPTAddr = FLASH_BASE + secpos*SECTOR_SIZE + i*FlashByte;
					//Write
					if(FLASH_ProgramWord(OPTAddr, FLASH_BUF[i]) == FLASH_COMPL)
					{
						if(FLASH_ReadWord(OPTAddr) != FLASH_BUF[i])
						{	//Verification error
							status = FLASH_WRITING_ERROR;
							break;
						}
					}
					else
					{		//Write error
						status = FLASH_WRITING_ERROR;
						break;
					}
				}
				if(status != FLASH_OK)
					break;
			}
		}
		else									//No need to erase
		{
			for(i=0; i<secremain; i++)					//Direct data writing and verification
			{
				OPTAddr = FLASH_BASE + secpos*SECTOR_SIZE + secoff*FlashByte + i*FlashByte;
				//Write
				if(FLASH_ProgramWord(OPTAddr, data[i]) == FLASH_COMPL)
				{
					if(FLASH_ReadWord(OPTAddr) != data[i])
					{	//Verification error
						status = FLASH_WRITING_ERROR;
						break;
					}
				}
				else
				{		//Write error
					status = FLASH_WRITING_ERROR;
					break;
				}
			}
		
		}
		//Processing data offset vector
		if(NumToWrite == secremain)			//The remaining write bytes are equal to the remaining space, indicating that the data writing is complete
			break;					
		else						//Update vector
		{
			secpos ++;							//Page address increased by 1	√
			secoff = 0;							//The offset in the page is cleared to 0	√
			data  += secremain;						//Data pointer offset	√
			addr  += secremain*FlashByte;					//Address offset
			NumToWrite -= secremain;					//Decreasing write length	√ Used to count the remaining space below	
			
			if(NumToWrite > (SECTOR_SIZE/FlashByte))
				secremain = SECTOR_SIZE/FlashByte;			//I still can't finish writing the next sector
			else 
				secremain = NumToWrite;					//The next sector can be finished
		}
	}
	
	FLASH_Lock();		//Locked
	return status;
}
//


//The THUMB instruction does not support assembly inline
//The following method is used to execute the assembly instruction WFI  
void WFI_SET(void)
{
	__ASM volatile("wfi");		  
}
//Turn off all interrupts
void INTX_DISABLE(void)
{		  
	//__ASM volatile("cpsid i");
	__disable_irq();
}
//Turn on all interrupts
void INTX_ENABLE(void)
{
	//__ASM volatile("cpsie i");	
	__enable_irq();	
}
//Set the top address of the stack
//addr: Top address
#ifdef COMPLIER_6
void MSR_MSP(u32 addr) 
{
    __asm("MSR MSP, r0"); 			//set Main Stack value
    __asm("BX r14");
}
#else
__asm void MSR_MSP(u32 addsr) 
{
    MSR MSP, r0 				//set Main Stack value
    BX r14
}
#endif


