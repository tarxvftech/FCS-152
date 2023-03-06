#ifndef __BSP_FLASH_H__
#define __BSP_FLASH_H__

#include "FCS152_KDU.h"

#define FLASH_PAGE_SIZE		64				//64 pages
#define SECTOR_SIZE 		2048 			  	//2K per page
#define FlashByte		sizeof(uint32_t)		//Unsigned integer-->4 bytes
	
enum 
{
  FLASH_OK = 0,
  FLASH_ERASE_ERROR,
  FLASH_WRITING_ERROR,
  FLASH_PROTECTION_ERRROR
};

void FLASH_Init(void);								//Initialization, you need to call before operating on Flash
uint32_t FLASH_ReadWord(uint32_t faddr);					//Read data of one word length
uint32_t FLASH_ReadUser(uint32_t addr, uint32_t *data, int32_t length);		//Read out
uint32_t FLASH_EraseUser(uint32_t faddr);					//Erase the used area
uint32_t FLASH_WriteUser(uint32_t addr, uint32_t *data, int32_t length);	//Write



//The following is the assembly function
void WFI_SET(void);			//Execute WFI instruction
void INTX_DISABLE(void);		//Turn off all interrupts
void INTX_ENABLE(void);			//Turn on all interrupts
void MSR_MSP(u32 addr);			//Set the stack address

#endif
