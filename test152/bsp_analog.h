#ifndef __BSP_ANALOG_H__
#define __BSP_ANALOG_H__
#include "FCS152_KDU.h"



void bsp_PWM_Init(void);
void bsp_ADC_Init(void);
void bsp_DAC_Init(void);


void BackLight_SetVal(u8 val);
uint32_t Use_ADC(void);
void RingTone(int tone, int state);




#endif
