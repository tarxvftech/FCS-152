#ifndef __BSP_M62364_H__
#define __BSP_M62364_H__
#include "FCS152_KDU.h"

#define TONE_OUT_CHAN               1       //音调使能通道->输出到A20_MIC
#define FM_S_EN_CHAN                2       //收音机电源是能通道
#define MIC_OUT_CHAN                3       //外置    MIC使能&增益控制  
#define MIC_IN_CHAN                 4       //内置_顶部MIC使能&增益控制   

#define WFM_LINE_CHAN               5       //收音机通道
#define A20_LINE_CHAN               6       //射频模块接收语音


#define SET_FM_S_EN                 M62364_SetSingleChannel(FM_S_EN_CHAN, 180)
#define CLR_FM_S_EN                 M62364_SetSingleChannel(FM_S_EN_CHAN, 0)



void M62364_Init(void);
void M62364_sendData(short iSdata);
void M62364_SetSingleChannel(unsigned char chan,unsigned char data);


#define M62364_LD_PIN           13
#define M62364_LD_SET           digitalWrite(M62364_LD_PIN, HIGH)
#define M62364_LD_CLR           digitalWrite(M62364_LD_PIN, LOW)

#define M62364_CLK_PIN          12
#define M62364_CLK_SET          digitalWrite(M62364_CLK_PIN, HIGH)
#define M62364_CLK_CLR          digitalWrite(M62364_CLK_PIN, LOW)

#define M62364_DATA_PIN         11
#define M62364_DATA_SET         digitalWrite(M62364_DATA_PIN, HIGH)
#define M62364_DATA_CLR         digitalWrite(M62364_DATA_PIN, LOW)


void bsp_M62364_Init(void);
void M62364Reset(void);


#endif
