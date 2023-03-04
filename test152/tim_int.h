#ifndef __TIM_INT_H__
#define __TIM_INT_H__
#include "bsp_timer.h"


/* Macro for switching global interrupts */
#define ENABLE_INT()            /* Enable global interrupt */
#define DISABLE_INT()           /* Disable global interrupts */

#define TMR_COUNT           12  /* The number of software timers (timer ID range 0-3) */

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

/* Timer structure, the member variable must be volatile, otherwise there may be problems when the C compiler optimizes */
typedef enum {
    TMR_ONCE_MODE = 0,      /* One-time working mode */
    TMR_AUTO_MODE = 1       /* Automatic timing working mode */
} TMR_MODE_E;

enum {
    TMR_FLOW=0,         //Backlight sleep
    TMR_KEY_SCAN,       //Key scan interval
    TMR_KEY_SAME,       //Press the same key interval
    TMR_RSSI_CTRL,      //Refresh signal strength interval
    TMR_VOLT_REFRESH,   //Refresh battery interval
    TMR_DUAL_REFRESH,   //Normal switching time interval for dual-guard mode
    TMR_FM_CTRL,        //Radio response time
    TMR_OUT_CTRL,       //Exit setting timing
    TMR_WAIT_KDU,       //Detect KDU insertion
    TMR_POS_BLINK,      //Edit cursor flashes
    TMR_ANY             //Single use everywhere
};

typedef struct {
    volatile uint32_t Count;    /* counter */ 
    volatile uint32_t PreLoad;  /* Counter pre-installed value */
    volatile uint8_t  Mode;     /* Counter mode, 1 shot */
    volatile uint8_t  Flag;     /* Timed arrival sign  */
} SOFT_TMR;


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


