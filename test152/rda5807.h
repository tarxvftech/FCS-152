#ifndef _RDA5807_H_
#define _RDA5807_H_
//

#define RDA5807_READ        0x23  //读RDA5807 
#define RDA5807_WRITE       0x22  //写RDA5807 
#define RDA5807_R00         0X00  //芯片ID寄存器 0x58 

#define RDA5807_R02         0X02  //DHIZ[15],DMUTE[14]静音,MONO[13]声道,BASS[12]重低音,SEEKUP[9],SEEK[8],SKMODE[7], 
#define RDA5807_R03         0X03  //CLK_MODE[6:4]时钟源选择,SOFTRESET[1]软复位,ENABLE[0]电源使能 
#define RDA5807_R04         0X04  //CHAN[15:6],TUNE[4],BAND[3:2],SPACE[1:0] 设置频率 带宽 步长 
#define RDA5807_R05         0X05  //STCIEN[14],DE[11],I2Senable[6],
//INT_MODE[15],SEEKTH[14:8](设定自动搜索信号强度阀值),LNA_PORT_SEL[7:6]=0b10,
//LNA_ICSEL_BIT[5:4],VOLUME[3:0]音量;

#define RDA5807_R0A         0X0A  //STC[14]seek complete SF[13]seek fail readchan[9:0]当前频道 
#define RDA5807_R0B         0X0B  //RSSI[15:9],FM TRUE[8]当前频道是一个节目台 

short RDA5807_ReadReg(unsigned  char addr);
void  RDA5807_WriteReg(unsigned char addr,short val);

void RDA_Power(unsigned char off_on);
//
void RDA5807_Set_Volume(unsigned char vol);
short RDA5807_RSSI(void);
void RDA5807_Set_Freq(short freq);
void RDA5807_Init(char off_on);

void Radio_Freq_Show(int fm_freq, int mode);
int FM_Freq_Set_Show(int x,int y,int * result);
void Enter_Radio(void);

#endif
//
