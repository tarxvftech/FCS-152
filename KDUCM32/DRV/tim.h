#ifndef __TIM_H__
#define __TIM_H__
#include "FCS152_KDU.h"

void LCDK_Init(void);
void TIM2_Init(void);
void BackLight_SetVal(char val);


/* 开关全局中断的宏 */
#define ENABLE_INT()	//__set_PRIMASK(0)	/* 使能全局中断 */
#define DISABLE_INT()	//__set_PRIMASK(1)	/* 禁止全局中断 */

#define TMR_COUNT			 7		/* 软件定时器的个数 （定时器ID范围 0 - 3) */

#define TMR_PERIOD_2MS       2
#define TMR_PERIOD_10MS     10				  
#define TMR_PERIOD_15MS		15
#define TMR_PERIOD_20MS		20
#define TMR_PERIOD_30MS		30
#define TMR_PERIOD_40MS		40
#define TMR_PERIOD_50MS		50
#define TMR_PERIOD_80MS		80
#define TMR_PERIOD_100MS    100
#define TMR_PERIOD_200MS    200
#define TMR_PERIOD_300MS    300
#define TMR_PERIOD_500MS    500
#define TMR_PERIOD_1S    	1000
#define TMR_PERIOD_3S    	3000
#define TMR_PERIOD_8S    	8000
#define TMR_PERIOD_10MIN 	(1000*60*10)	//超过范围


/* 定时器结构体，成员变量必须是 volatile, 否则C编译器优化时可能有问题 */
typedef enum
{
	TMR_ONCE_MODE = 0,		/* 一次工作模式 */
	TMR_AUTO_MODE = 1		/* 自动定时工作模式 */
}TMR_MODE_E;

enum
{
	TMR_FLOW=0,		//背光睡眠
	TMR_DELAY_FLASH,//刷新主页面间隔
	TMR_DUAL_FLASH,	//双守模式切换时间间隔
	TMR_RSSI_CTRL,	//信号获取间隔
	TMR_ASK,        //定时询问
    TMR_REPLY,      //回复 限时
	
	
	TMR_SCAN_FLASH,	//
	TMR_VOLT_FLASH,
	
	TMR_WAIT_FLASH,
	
	TMR_FM_CTRL,
	TMR_SQl_CTRL,
	
	TMR_SLEEP
};

typedef struct
{
	volatile uint32_t Count;	/* 计数器 */
	volatile uint32_t PreLoad;	/* 计数器预装值 */
	volatile uint8_t  Mode;		/* 计数器模式，1次性 */
	volatile uint8_t  Flag;		/* 定时到达标志  */
}SOFT_TMR;


void bsp_InitTimer(void);
int32_t bsp_GetRunTime(void);
void bsp_StartTimer(uint8_t _id, uint32_t _period);
void bsp_StartAutoTimer(uint8_t _id, uint32_t _period);
void bsp_StopTimer(uint8_t _id);
uint8_t bsp_CheckTimer(uint8_t _id);

void disposeAllTimeData(void);




#endif
