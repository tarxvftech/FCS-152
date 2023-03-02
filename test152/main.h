#ifndef __MAIN_H__
#define __MAIN_H__

#include "FCS152_KDU.h"

#include "bsp_dac.h"
#include "bsp_uart.h"
#include "bsp_ch423.h"
#include "bsp_conio.h"
#include "bsp_timer.h"
#include "bsp_m62364.h"
#include "bsp_storage.h"
#include "bsp_device.h"         //Sleep Init
#include "bsp_MatrixKeyBoard.h"

#include "lcd.h"                //LCD 
#include "key.h"                //key: Independent Key, MatrixKeyBoard, EncoderClick/Spin
#include "analog.h"             //ADC DAC PWM
#include "encoder.h"
#include "tim_int.h"            //Timing
#include "controller.h"



#if FM_EN
#include "rda5807.h"
extern volatile u8 WFM;             //FM switch. Enablig FM broadcast radio receiver
#endif


extern int TIMES;                   //Record encoder operation
// extern int  UART1_getRcvFlag(void);                //Determine the receiving flag
// extern int  UART1_dataPreProcess(void);            //Data preprocessing

void VFO_Load_Data(void);
void VFO_Clear(void);               //Main interface initialization
void VFO_Refresh(void);             //Homepage interface refresh

void Encoder_process(u8 operate);
u8 Event_Matrix(u8 matrix_key);     //The main interface matrix button trigger event detection 
                                    //0: No modification, 
                                    //1: Overload 
                                    //2: Save
void Argument_process(u8 key_pro_ret);

void MY_GLOBAL_FUN(void);           //Global processing function
void PTT_Control(void);             //Code that is executed only once after pressing or releasing PTT
void SQ_Read_Control(void);         //(?)
void SQUELCH_Contol(void);          //Code controlling squelch (?)
void SendALL(void);                 //Send all data
int  KDUCheck(void);                //KDU Check - is this part of code checking connection of radio with KDU(?) Not find source code yet.
int  KDU_Processor(void);           //KDU Data interaction
void VOL_Reflash(void);             //Volume setting
void A20_CALLBACK(void);            //A20 Data interaction, A20 data must be processed before it can be returned
void Switch_Dual_Chan(void);        //Dual-channel switching in dual-guard mode
void SetNowChanSql0(u8 on);         //Switch normally squelch state

//Main interface function selection
void ShortCut_Menu(void);           //Quick settings of the main interface
void ShortCut_MICGAIN_Select(void); //The main interface quickly sets the mic sensitivity
void ShortCut_FM_Select(void);      //The main interface quickly switches the radio
void ShortCut_CHAN_Select(void);    //Main interface channel switching
int Lock_Screen_KeyBoard(void);     //Lock screen lock panel


//Send and receive settings
void RT_Menu(void);
void RT_Menu_Clear(void);
int  RT_FREQ_Set(int x, int y, double * vfo_freq_temp, int vu_switch);
int  RT_SubVoice_Set(int row, int subvoice);                            //Subtone setting (?) - Maybe my incorrect translation. I think it's all about subtone, but maybe its all about subsonic (?)
int  RT_SubVoice_Matrix_Menu_Select(int subvoice);                      //Matrix subsonic setting - same thing !
int  RT_TX_POWER_Set(int power_temp);                                   //Transmit power selection
int  RT_GBW_Set(int gbw_temp);                                          //Bandwidth selection (WIDE/NAROW)
void RT_NICKNAME_Set(u8 current_channel, unsigned char nn_temp[7]);     //Alias setting - It's all about of naming received channels (?)
void RT_CHAN_Switch(void);                                              //Channel number switching


//Button *2(LT)*: Initialize the backlight menu
void  Light_Mode_Set(void);

//Button *5(Zero)*: Initialize the zeroize menu
int Zeroize_All(void);                  //Deletting all of settings, including all channel memory
void Zero_Menu(void);                   //Zeroize menu


//Button *7(OPT)*: Initialize the option menu
void OPTION_Menu(void);
void Key_Test(void);                    //Test button option

//Button *8(PGM)*: Initialize another bunch of option menu
void PGM_Menu(void);

int  PGM_AUDIO_Select(u8 row);          //Audio strobe and set microphone sensitivity. In actual version is only audio output menu exist. I can't find any mic sensitivity option. 
int  PGM_SQL_Set(u8 row);               //Squelch level
int  PGM_STEP_Set(u8 row);              //Step set
int  PGM_ENCRPY_Set(u8 row);            //Encryption set. In actual firmware version this option doesn't exist.
int  PGM_TOT_Set(u8 row);               //Launch time limit. Setting the limit of continuously transmission by radio.
int  PGM_LAMP_TIME_Set(u8 row);         //Backlight duration
int  PGM_POWEROUT_Set(u8 row);          //Six-pin power output. Enabling dynamic mic amplifier.
int  PGM_TONE_Select(u8 row);           //Tone setting. Enabling/disabling pre- or end- tone setting.


void SHUT(void);                    //Turn off all functions
//void Update_Check(void);          //IAP

//uint16_t Get_JTAG_ID(void);

#endif


