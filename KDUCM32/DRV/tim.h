#ifndef __TIM_H__
#define __TIM_H__
#include "FCS152_KDU.h"

void LCDK_Init(void);
void TIM2_Init(void);
void BackLight_SetVal(char val);


/* Macro for switching global interrupts */
#define ENABLE_INT()	//__set_PRIMASK(0)	/* Enable global interrupt */
#define DISABLE_INT()	//__set_PRIMASK(1)	/* Disable global interrupts */

#define TMR_COUNT			 7	/* The number of software timers (timer ID range 0-3) */

#define TMR_PERIOD_2MS       	2
#define TMR_PERIOD_10MS     	10				  
#define TMR_PERIOD_15MS		15
#define TMR_PERIOD_20MS		20
#define TMR_PERIOD_30MS		30
#define TMR_PERIOD_40MS		40
#define TMR_PERIOD_50MS		50
#define TMR_PERIOD_80MS		80
#define TMR_PERIOD_100MS    	100
#define TMR_PERIOD_200MS    	200
#define TMR_PERIOD_300MS    	300
#define TMR_PERIOD_500MS    	500
#define TMR_PERIOD_1S    	1000
#define TMR_PERIOD_3S    	3000
#define TMR_PERIOD_8S    	8000
#define TMR_PERIOD_10MIN 	(1000*60*10)	//Out of range


/* Timer structure, the member variable must be volatile, otherwise there may be problems when the C compiler optimizes */
typedef enum
{
	TMR_ONCE_MODE = 0,		/* One-time working mode */
	TMR_AUTO_MODE = 1		/* Automatic timing working mode */
}TMR_MODE_E;

enum
{
	TMR_FLOW=0,			//Backlight sleep
	TMR_DELAY_FLASH,		//Refresh the main page interval
	TMR_DUAL_FLASH,			//Dual guard mode switching time interval
	TMR_RSSI_CTRL,			//Signal acquisition interval
	TMR_ASK,        		//Regular inquiry
    	TMR_REPLY,      		//Reply time limit
	
	
	TMR_SCAN_FLASH,			//
	TMR_VOLT_FLASH,
	
	TMR_WAIT_FLASH,
	
	TMR_FM_CTRL,
	TMR_SQl_CTRL,
	
	TMR_SLEEP
};

typedef struct
{
	volatile uint32_t Count;	/* counter */
	volatile uint32_t PreLoad;	/* Counter pre-installed value */
	volatile uint8_t  Mode;		/* Counter mode, 1 shot */
	volatile uint8_t  Flag;		/* Timed arrival sign  */
}SOFT_TMR;


void bsp_InitTimer(void);
int32_t bsp_GetRunTime(void);
void bsp_StartTimer(uint8_t _id, uint32_t _period);
void bsp_StartAutoTimer(uint8_t _id, uint32_t _period);
void bsp_StopTimer(uint8_t _id);
uint8_t bsp_CheckTimer(uint8_t _id);

void disposeAllTimeData(void);




#endif
