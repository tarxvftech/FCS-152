#ifndef __BSP_FLASH_H__
#define __BSP_FLASH_H__

#include "FCS152_KDU.h"

#define FLASH_PAGE_SIZE		64				//64页
#define SECTOR_SIZE 		2048 			  //每页2K
#define FlashByte			sizeof(uint32_t)//无符号整型-->4字节
	
enum 
{
  FLASH_OK = 0,
  FLASH_ERASE_ERROR,
  FLASH_WRITING_ERROR,
  FLASH_PROTECTION_ERRROR
};

void FLASH_Init(void);														        //初始化，对Flash进行操作之前需要调用
uint32_t FLASH_ReadWord(uint32_t faddr);									//读取一个字长度的数据
uint32_t FLASH_ReadUser(uint32_t addr, uint32_t *data, int32_t length);		//读出
uint32_t FLASH_EraseUser(uint32_t faddr);									//擦除使用的片区
uint32_t FLASH_WriteUser(uint32_t addr, uint32_t *data, int32_t length);	//写入



//以下为汇编函数
void WFI_SET(void);		//执行WFI指令
void INTX_DISABLE(void);//关闭所有中断
void INTX_ENABLE(void);	//开启所有中断
void MSR_MSP(u32 addr);	//设置堆栈地址

#endif
