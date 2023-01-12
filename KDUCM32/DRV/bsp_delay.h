#ifndef __BSP_DELAY_H__
#define __BSP_DELAY_H__

#include "FCS152_KDU.h"


enum
{
    SYSCLK_PLLSRC_HSI,
    SYSCLK_PLLSRC_HSIDIV2,
    SYSCLK_PLLSRC_HSI_PLLDIV2,
    SYSCLK_PLLSRC_HSIDIV2_PLLDIV2,
    SYSCLK_PLLSRC_HSE,
    SYSCLK_PLLSRC_HSEDIV2,
    SYSCLK_PLLSRC_HSE_PLLDIV2,
    SYSCLK_PLLSRC_HSEDIV2_PLLDIV2,
};

void Sys_Init(void);
void delay_init(void);

void delay_us(int nus); 
void delay_ms(int nms); 
void delay_s(int ns); 


#endif

