#ifndef __BSP_STORAGE_H__
#define __BSP_STORAGE_H__
#include "FCS152_KDU.h"

#ifdef DEBUG
#define RESET_VAL 0x24
#else
#define RESET_VAL 0x55
#endif

void    Init_Storage(void);

//Flag
void    set_Flag(int flag_number,uint8_t value);
uint8_t get_Flag(int flag_number);

//Scan to add
void    set_Scan(uint8_t channel, uint8_t scan);
uint8_t get_Scan(uint8_t channel);

void    save_ChanA(uint8_t chana);
uint8_t load_ChanA(void);
void    save_ChanB(uint8_t chanb);
uint8_t load_ChanB(void);

//Channel number
void    save_CurrentChannel(uint8_t channel);
uint8_t load_CurrentChannel(void);

//Channel parameters
void save_ChannelParameter(uint8_t chan, CHAN_ARV S);
void load_ChannelParameter(uint8_t chan, CHAN_ARV_P L);
//
void load_ChannelParameterStr(uint8_t chan, char * L);
void save_ChannelParameterStr(uint8_t chan, char * S);

//Data initialization
void DATA_Init(void);

//Audio input and output selection
void    save_AudioSelect(uint8_t audio);
uint8_t load_AudioSelect(void);

//Microphone sensitivity
void    save_MicLevel(uint8_t mic);
uint8_t load_MicLevel(void);

//Squelch
void    save_Sql(uint8_t sql);
uint8_t load_Sql(void);

//Encryption
void    save_ScramLevel(uint8_t scram);
uint8_t load_ScramLevel(void);

//Step forward
void    save_Step(uint8_t step);
uint8_t load_Step(void);

//Launch delay (TOT)
void    save_Tot(uint8_t tot);
uint8_t load_Tot(void);

//Backlight intensity
void    save_Backlightness(uint8_t value);
uint8_t load_Backlightness(void);
//Contrast
void    save_ScreenContrast(uint8_t value);
uint8_t load_ScreenContrast(void);

//Backlight mode
void    save_LampTime(uint8_t lamptime);
uint8_t load_LampTime(void);

//Six-pin output
void    save_VDO(uint8_t vdo);
uint8_t load_VDO(void);


//Global volume
void    save_OverVolume(uint8_t volume);
uint8_t load_OverVolume(void);


//PTT pre-tone
void    save_PreTone(uint8_t pretone);
uint8_t load_PreTone(void);
//PTT end tone
void    save_EndTone(uint8_t endtone);
uint8_t load_EndTone(void);

//FM broadcasting radio frequency//87.0 ~ 108.0
void save_FMFreq(int fm_freq);
int  load_FMFreq(void);
#endif


