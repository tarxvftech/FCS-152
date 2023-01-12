#ifndef __BSP_DEVICE_H__
#define __BSP_DEVICE_H__

void SHUT(void);
void ClearShut(void);
void Cal2Shut(void);

void Sys_Enter_Standby(void); 		//系统进入待机模式
void Standby_Init(void);	    	//初始待机及退出


void enterSecondSystem(void);			//IAP

void TakeDataFromMem(int page);
void WriteDataBackMem(int page);

#endif
