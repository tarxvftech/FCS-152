#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_	 
#include "bsp_conio.h" 



void EN_GPIO_Init(void);			//使能引脚初始化
void A002_Init(void);				//A20控制引脚初始化

uint8_t Select_Power(void);
void VDO_SWITCH(unsigned char on_off);          //6针头电源输出
void MIC_SWITCH(char mic_temp, char on_off);	//MIC打开
void SPK_SWITCH(char spk_temp, char on_off);	//SPK打开

#endif
