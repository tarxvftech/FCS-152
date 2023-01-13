#ifndef __TIM_INT_H__
#define __TIM_INT_H__
#include "bsp_timer.h"  


/* 开关全局中断的宏 */
#define ENABLE_INT()            /* 使能全局中断 */
#define DISABLE_INT()           /* 禁止全局中断 */  

#define TMR_COUNT           12  /* 软件定时器的个数 （定时器ID范围 0 - 3) */

#define TMR_PERIOD_2MS       2  
#define TMR_PERIOD_10MS     10  
#define TMR_PERIOD_15MS     15  
#define TMR_PERIOD_20MS     20  
#define TMR_PERIOD_30MS     30  
#define TMR_PERIOD_40MS     40  
#define TMR_PERIOD_50MS     50  
#define TMR_PERIOD_80MS     80  
#define TMR_PERIOD_100MS    100 
#define TMR_PERIOD_200MS    200 
#define TMR_PERIOD_300MS    300 
#define TMR_PERIOD_500MS    500 
#define TMR_PERIOD_1S       1000    
#define TMR_PERIOD_2S       2000 
#define TMR_PERIOD_3S       3000 
#define TMR_PERIOD_8S       8000 

/* 定时器结构体，成员变量必须是 volatile, 否则C编译器优化时可能有问题 */
typedef enum
{
    TMR_ONCE_MODE = 0,      /* 一次工作模式 */
    TMR_AUTO_MODE = 1       /* 自动定时工作模式 */
}TMR_MODE_E;

enum
{
    TMR_FLOW=0,			//背光睡眠
    TMR_KEY_SCAN,		//按键扫描间隔
    TMR_KEY_SAME,		//按相同键间隔
    TMR_RSSI_CTRL,		//刷新信号强度间隔
    TMR_VOLT_REFRESH,	//刷新电量间隔
    TMR_DUAL_REFRESH,	//双守模式 正常切换 时间间隔
    TMR_FM_CTRL,		//收音机回复时间
    TMR_OUT_CTRL,		//退出设置定时
    TMR_WAIT_KDU,		//检测KDU插入
    TMR_POS_BLINK,		//编辑光标闪烁
    TMR_ANY				//单一随处使用
};

typedef struct
{
    volatile uint32_t Count;	/* 计数器 */
    volatile uint32_t PreLoad;	/* 计数器预装值 */
    volatile uint8_t  Mode;		/* 计数器模式，1次性 */
    volatile uint8_t  Flag;		/* 定时到达标志  */
}SOFT_TMR;


void    Timer_Init(void);

void    bsp_InitTimer(void);
int32_t bsp_GetRunTime(void);
void    bsp_StartTimer(uint8_t _id, uint32_t _period);
void    bsp_StartAutoTimer(uint8_t _id, uint32_t _period);
void    bsp_StopTimer(uint8_t _id);
uint8_t bsp_CheckTimer(uint8_t _id);

void    ReloadOutCal(void);
void    RDA5807_ResumeImmediately(void); 
#endif


