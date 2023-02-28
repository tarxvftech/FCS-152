#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_
#include "bsp_conio.h"



void EN_GPIO_Init(void);            //Enable pin initialization
void A002_Init(void);               //A20 Control pin initialization

uint8_t Select_Power(void);
void VDO_SWITCH(unsigned char on_off);          //6-pin power output - for dynamic (boom) microphone
void MIC_SWITCH(char mic_temp, char on_off);    //MIC setting on/off
void SPK_SWITCH(char spk_temp, char on_off);    //SPK setting on/off

#endif
