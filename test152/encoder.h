#ifndef __ENCODER_H__
#define __ENCODER_H__
#include "FCS152_KDU.h"

void    Encoder_Init(void);
int     EncoderClickValidate(void);             //Return to the encoder and click on the valid value
uint8_t Encoder_Switch_Scan(uint8_t mode);

#endif
