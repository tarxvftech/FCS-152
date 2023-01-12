#ifndef __ENCODER_H__
#define __ENCODER_H__
#include "FCS152_KDU.h"

void    Encoder_Init(void);
int     EncoderClickValidate(void);             //返回编码器点击有效值
uint8_t Encoder_Switch_Scan(uint8_t mode);

#endif
