#ifndef __BSP_DEVICE_H__
#define __BSP_DEVICE_H__

void SHUT(void);
void ClearShut(void);
void Cal2Shut(void);

void Sys_Enter_Standby(void);       //The system enters standby mode
void Standby_Init(void);            //Initial standby and exit


void enterSecondSystem(void);           //IAP

void TakeDataFromMem(int page);
void WriteDataBackMem(int page);

#endif
