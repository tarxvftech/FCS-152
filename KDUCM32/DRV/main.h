/*
	#include "main.h"
*/
#ifndef __MAIN_H__
#define __MAIN_H__
#include "FCS152_KDU.h"

#include "bsp_uart.h"
#include "bsp_delay.h"
#include "bsp_flash.h"

#include "tim.h"
#include "lcd.h"
#include "font.h"
#include "key.h"
#include "iap.h"


extern volatile char FLAG_LINE;
extern char flagLineTemp;
extern volatile u8 BL;

/*
Never forget that my heart is broken, when will the two meet?
The cowherd mixed into the Weavers' Temple, and his tone was right with swords and guns.
Your eyes are not as promised by Lin Xin, if you have no intention of flying each other - WTF???  Is this a chinese literature?? :)))
*/

/*
CF: 		0 is CHAN, 1 is FREQ
VU: 		0 is VHF, 1 is UHF
CHAN:		Channel num
RS:		Receive CTCSS (Continuous Tone Coded Squelch System)
TS: 		Transmit CTCSS (Continuous Tone Coded Squelch System)
POWER:		Transmit power
GBW:		Bandwidth: 0-narrow, 1-wide
NN[7]:		Alias
SCAN:		Scan mark
STEP_LEVEL:	Step selection
SQL=4:		Squelch
AUD=0:		Audio channel output
MIC=4:		Microphone gain selection
ENC=0:		Encryption
BL=1:		Backlight
SC=0:		Contrast
OP=0:		Six-pin power output
VOLUME:		Volume
PRE_TONE:	Pre- tone
END_TONE:	End- tone
WFM:		Radio switch
VOLTAGE:	Voltage
RSSI:		Signal strength
PTT_READ:	PTT switch
A002_SQ_READ:   Signal reception status
*/


//
extern char Home_Mode, 
			Flag_Main_Page, 	//Homepage display mode
			SQL_MODE;		//Long squelch mode
extern uint8_t usart1_recv_end_flag;
extern iapfun jump;
extern const char prefix_buf[][16];


void KDU_Clear(void);		      		//Homepage interface formatting
void KDU_Reflash(void);	    			//Homepage interface refresh
int  Main_Event_KEY(u8 key);			//The main interface handles key events		0: No modification, 1: Overload, 2: Save
void MY_GLOBAL_FUN(unsigned char mode);		//Global function function
void VOL_Reflash(void);



//Go to the home menu
int  ShortCut_Menu(void);		
int ShortCut_MICGAIN_Select(void);
int  ShortCut_FM_Select(void);
int  ShortCut_CHAN_Select(void);
int  Lock_Screen_KeyBoard(void);	  	//Shortcut function 0: Lock screen, lock disk


//Send and receive settings
int  RT_Menu(void);
void RT_Menu_Clear(void);
int  RT_FREQ_Set(int x, int y, double *vfo_freq_temp, int vu_switch);
int  RT_SubVoice_Set(int row, int subvoice);			      		//Subsonic setting
int  RT_SubVoice_Matrix_Menu_Select(int subvoice);		    		//Matrix subsonic setting
int  RT_TX_POWER_Set(int power_temp);						//Transmit power selection
int  RT_GBW_Set(int gbw_temp);						        //Bandwidth selection
int  RT_NICKNAME_Set(void); 							//Alias setting
int  RT_CHAN_Switch(void);					           	//Channel number selection


//Button 2: Initialize the "Light mode" menu
void Light_Mode_Set(void);

//Button 3: Initialize the "FM Radio" menu
void Enter_Radio(void);

//Button 5: Initialize the "Zeroize" menu
int Zeroize_All(void);
void Zero_Menu(void);


//Button 7: Initialize the "Option" menu
void OPTION_Menu(void);
void Key_Test(void);			    		//Test button

//Button 8: Initialize the "PGM" menu
void PGM_Menu(void);
int  PGM_AUDIO_Select(u8 row);				//Audio output (IN, TOP, SIDE) and set microphone sensitivity
int  PGM_SQL_Set(u8 row);			  	//Squelch level
int  PGM_STEP_Set(u8 row);				//Step (5K, 6.25K, 10K, 12.5K)
int  PGM_TOT_Set(u8 row);			   	//Launch time limit
int  PGM_LAMP_TIME_Set(u8 row);				//Backlight wake-up time
int  PGM_POWEROUT_Set(u8 row);				//Six-pin power output
int  PGM_TONE_Select(u8 row);				//Tone setting

typedef enum
{
	tools_lcd_inverted=0,
	tools_prc117g,
	tools_prc148,
	tools_prc152,
	tools_update,
	tools_about
}tools_select;

//Key TOOLS
void TOOLS_Menu(void);			
int  checkAbout(void);
int  LCD_INVERTED_Set(u8 row);

//IAP
void waitToUpdate(void);
void bootToUpgrade(void);				//Start the detection upgrade

//


void loadData(void);

int receiveProcess(void);

void sendTo152(u8 process);	

void sendCommand(u8 cmd);
u8   waitToReceive(int seconds);			//Some returned places increase waiting for data processing

void wait152StartUp(void);






#endif


