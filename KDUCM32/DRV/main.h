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
念念不忘心已碎, 二人何时能相会
牛郎混进织女寺, 口吻力来刀枪对
双目非林心相许, 您若无心各自飞	
*/

/*
CF: 0为CHAN, 1为FREQ
VU:	0为VHF,  1为UHF
CHAN:		channel num
RS:			Receive CTCSS(Continuous Tone Coded Squelch System)
TS: 		Transmit CTCSS
POWER:		发射功率
GBW:		带宽: 0--窄, 1--宽
NN[7]:		别名
SCAN:		扫描标记
STEP_LEVEL:	步进选择
SQL=4:		静噪
AUD=0:		音频通道输出
MIC=4:		麦克风增益选择
ENC=0:		加密
BL=1:		背光
SC=0:		对比度
OP=0:		六针头电源输出
VOLUME:		音量
PRE_TONE:	前置提示音
END_TONE:	后置提示音
WFM:		收音机开关
VOLTAGE:	电压
RSSI:		信号强度
PTT_READ:	PTT开关;
A002_SQ_READ:信号接收状态
*/


//
extern char Home_Mode, 
			Flag_Main_Page , //主页显示模式
			SQL_MODE;		 //长静噪模式
extern uint8_t usart1_recv_end_flag;
extern iapfun jump;
extern const char prefix_buf[][16];


void KDU_Clear(void);		      		//主页界面格式化
void KDU_Reflash(void);	    			//主页界面刷新
int  Main_Event_KEY(u8 key);			//主界面处理按键事件		0：不做修改， 1：重载  2：保存
void MY_GLOBAL_FUN(unsigned char mode);	//全局功能函数
void VOL_Reflash(void);



//进入主页菜单
int  ShortCut_Menu(void);		
int ShortCut_MICGAIN_Select(void);
int  ShortCut_FM_Select(void);
int  ShortCut_CHAN_Select(void);
int  Lock_Screen_KeyBoard(void);	  	//快捷功能0：锁屏锁盘


//收发设置
int  RT_Menu(void);
void RT_Menu_Clear(void);
int  RT_FREQ_Set(int x, int y, double *vfo_freq_temp, int vu_switch);
int  RT_SubVoice_Set(int row, int subvoice);			      			//亚音设置
int  RT_SubVoice_Matrix_Menu_Select(int subvoice);		    			//矩阵亚音设置
int  RT_TX_POWER_Set(int power_temp);									//发射功率选择
int  RT_GBW_Set(int gbw_temp);						              		//带宽选择
int  RT_NICKNAME_Set(void); 											//别名设置
int  RT_CHAN_Switch(void);					           		 			//信道号选择


//按键2
void Light_Mode_Set(void);

//按键3
void Enter_Radio(void);

//按键5:初始化菜单
int Zeroize_All(void);
void Zero_Menu(void);


//按键7 OPTION菜单
void OPTION_Menu(void);
void Key_Test(void);			    		//测试按键

//按键8:
//按键8 PGM菜单
void PGM_Menu(void);
int  PGM_AUDIO_Select(u8 row);			//音频选通并设置咪灵敏度
int  PGM_SQL_Set(u8 row);			  	//静噪等级
int  PGM_STEP_Set(u8 row);				//步进
int  PGM_TOT_Set(u8 row);			    //发射限时
int  PGM_LAMP_TIME_Set(u8 row);			//背光唤醒时间
int  PGM_POWEROUT_Set(u8 row);			//六针头电源输出
int  PGM_TONE_Select(u8 row);			//提示音设置

typedef enum
{
	tools_lcd_inverted=0,
	tools_prc117g,
	tools_prc148,
	tools_prc152,
	tools_update,
	tools_about
}tools_select;

//按键TOOLS
void TOOLS_Menu(void);			
int  checkAbout(void);
int  LCD_INVERTED_Set(u8 row);

//IAP
void waitToUpdate(void);
void bootToUpgrade(void);				//启动检测升级

//


void loadData(void);

int receiveProcess(void);

void sendTo152(u8 process);	

void sendCommand(u8 cmd);
u8   waitToReceive(int seconds);					//一些返回的地方增加等待数据处理

void wait152StartUp(void);






#endif


