#ifndef __BSP_STORAGE_H__
#define __BSP_STORAGE_H__
#include "FCS152_KDU.h"

#ifdef DEBUG
#define RESET_VAL 0x24
#else
#define RESET_VAL 0x55
#endif

void    Init_Storage(void);

//标志位
void    set_Flag(int flag_number,uint8_t value);
uint8_t get_Flag(int flag_number);

//扫描添加
void    set_Scan(uint8_t channel, uint8_t scan);
uint8_t get_Scan(uint8_t channel);

void    save_ChanA(uint8_t chana);
uint8_t load_ChanA(void);
void    save_ChanB(uint8_t chanb);
uint8_t load_ChanB(void);

//信道号
void    save_CurrentChannel(uint8_t channel);
uint8_t load_CurrentChannel(void);

//信道参数
void save_ChannelParameter(uint8_t chan, CHAN_ARV S);
void load_ChannelParameter(uint8_t chan, CHAN_ARV_P L);
//
void load_ChannelParameterStr(uint8_t chan, char* L);
void save_ChannelParameterStr(uint8_t chan, char* S);

//数据初始化
void DATA_Init(void);

//音频输入输出选择
void    save_AudioSelect(uint8_t audio);
uint8_t load_AudioSelect(void);

//咪灵敏度
void    save_MicLevel(uint8_t mic);
uint8_t load_MicLevel(void);

//静噪
void    save_Sql(uint8_t sql);
uint8_t load_Sql(void);

//加密
void    save_ScramLevel(uint8_t scram);
uint8_t load_ScramLevel(void);

//步进
void    save_Step(uint8_t step);
uint8_t load_Step(void);

//发射延时
void    save_Tot(uint8_t tot);
uint8_t load_Tot(void);

//背光强度
void    save_Backlightness(uint8_t value);
uint8_t load_Backlightness(void);
//对比度
void    save_ScreenContrast(uint8_t value);
uint8_t load_ScreenContrast(void);

//背光模式
void    save_LampTime(uint8_t lamptime);
uint8_t load_LampTime(void);

//六针头输出
void    save_VDO(uint8_t vdo);
uint8_t load_VDO(void);


//全局音量
void    save_OverVolume(uint8_t volume);
uint8_t load_OverVolume(void);


//PTT前置提示音
void    save_PreTone(uint8_t pretone);
uint8_t load_PreTone(void);
//PTT结束提示音
void    save_EndTone(uint8_t endtone);
uint8_t load_EndTone(void);

//收音机频率//870~1080
void save_FMFreq(int fm_freq);
int  load_FMFreq(void);
#endif


