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
#include "bsp_device.h"      //Sleep Init
#include "bsp_MatrixKeyBoard.h"

#include "lcd.h"
#include "key.h"                //key:Independent Key, MatrixKeyBoard, EncoderClick/Spin
#include "analog.h"             //ADC DAC PWM
#include "encoder.h"
#include "tim_int.h"            //Timing
#include "controller.h"



#if FM_EN
    #include "rda5807.h"
    extern volatile u8 WFM;                 //FM开关
#endif


extern int TIMES;                   //记录编码器操作
// extern int  UART1_getRcvFlag(void);                //判断接收标志位
// extern int  UART1_dataPreProcess(void);            //数据预处理

void VFO_Load_Data(void);
void VFO_Clear(void);               //主界面初始化
void VFO_Refresh(void);             //主页界面刷新

void Encoder_process(u8 operate);
u8 Event_Matrix(u8 matrix_key);     //主界面矩阵按键触发事件检测 0：不做修改， 1：重载  2：保存
void Argument_process(u8 key_pro_ret);

void MY_GLOBAL_FUN(void);           //全局处理函数
void PTT_Control(void);             //按下或松开PTT后只执行一次的代码
void SQ_Read_Control(void);
void SQUELCH_Contol(void);
void SendALL(void);                 //发送全部数据
int  KDUCheck(void);                //
int  KDU_Processor(void);           //KDU数据交互
void VOL_Reflash(void);             //音量设置
void A20_CALLBACK(void);            //A20数据交互,必须处理了才有a20数据返回
void Switch_Dual_Chan(void);        //双守模式下双信道切换
void SetNowChanSql0(u8 on);         //开关常静噪状态
//主界面功能选择
void ShortCut_Menu(void); 			//主界面快捷设置  
void ShortCut_MICGAIN_Select(void);	//主界面快捷设置mic灵敏度
void ShortCut_FM_Select(void);		//主界面快捷开关收音机
void ShortCut_CHAN_Select(void);	//主界面信道切换
 int Lock_Screen_KeyBoard(void);	//锁屏锁盘	


//收发设置
void RT_Menu(void);
void RT_Menu_Clear(void);
int  RT_FREQ_Set(int x, int y, double *vfo_freq_temp, int vu_switch);
int  RT_SubVoice_Set(int row, int subvoice);			      			//亚音设置
int  RT_SubVoice_Matrix_Menu_Select(int subvoice);						//矩阵亚音设置
int  RT_TX_POWER_Set(int power_temp);				          			//发射功率选择
int  RT_GBW_Set(int gbw_temp);											//带宽选择            
void RT_NICKNAME_Set(u8 current_channel, unsigned char nn_temp[7]); 	//别名设置
void RT_CHAN_Switch(void);	           		 							//信道号切换


//按键2
void  Light_Mode_Set(void);

//按键5:初始化菜单
int Zeroize_All(void);
void Zero_Menu(void);


//按键7 OPTION菜单
void OPTION_Menu(void);
void Key_Test(void);			    	//测试按键

//按键8 PGM菜单
void PGM_Menu(void);

int  PGM_AUDIO_Select(u8 row);			//音频选通并设置咪灵敏度
int  PGM_SQL_Set(u8 row);			  	//静噪等级
int  PGM_STEP_Set(u8 row);				//步进
int  PGM_ENCRPY_Set(u8 row);   			//加密
int  PGM_TOT_Set(u8 row);			    //发射限时
int  PGM_LAMP_TIME_Set(u8 row);			//背光灯时长
int  PGM_POWEROUT_Set(u8 row);			//六针头电源输出
int  PGM_TONE_Select(u8 row);			//提示音设置


void SHUT(void);			        //关闭所有功能
//void Update_Check(void);			//IAP

//uint16_t Get_JTAG_ID(void);

#endif


