#include "tim_int.h"    
#include "bsp_conio.h"  
#include "rda5807.h"    

static volatile uint32_t s_uiDelayCount = 0;    
static volatile uint8_t s_ucTimeOutFlag = 0;    
static SOFT_TMR s_tTmr[TMR_COUNT];  
volatile int32_t g_iRunTime = 0;    

void Timer_Init(void)
{
    bsp_Timer0_Init();  //1ms
    bsp_Timer1_Init();  //100ms
    bsp_Timer2_Init();  //7us

    bsp_InitTimer();
    bsp_StartAutoTimer(TMR_KEY_SCAN,      TMR_PERIOD_20MS);
    bsp_StartAutoTimer(TMR_POS_BLINK,	  TMR_PERIOD_200MS);
    bsp_StartAutoTimer(TMR_RSSI_CTRL,     TMR_PERIOD_1S);
    bsp_StartAutoTimer(TMR_VOLT_REFRESH,  TMR_PERIOD_3S); //定时检测电压
}

void bsp_InitTimer(void)
{
    uint8_t i;

    /* 清零所有的软件定时器 */
    for (i = 0; i < TMR_COUNT; i++)
    {
        s_tTmr[i].Count = 0;
        s_tTmr[i].PreLoad = 0;
        s_tTmr[i].Flag = 0;
        s_tTmr[i].Mode = TMR_ONCE_MODE;	/* 缺省是一次性工作模式 */
    }
}

//获取运行次数
int32_t bsp_GetRunTime(void)
{
    int32_t runtime;

    DISABLE_INT();  	/*  */

    runtime = g_iRunTime;	/*  */

    ENABLE_INT();  		/*  */

    return runtime;
}

//启动"定时器"
void bsp_StartTimer(uint8_t _id, uint32_t _period)
{
    if (_id >= TMR_COUNT)
    {
        while(1); 
    }

    DISABLE_INT();  			

    s_tTmr[_id].Count = _period;		
    s_tTmr[_id].PreLoad = _period;		
    s_tTmr[_id].Flag = 0;				
    s_tTmr[_id].Mode = TMR_ONCE_MODE;	

    ENABLE_INT();  				
}

//启动自动填装重载值的"定时器"
void bsp_StartAutoTimer(uint8_t _id, uint32_t _period)
{
    if (_id >= TMR_COUNT)
    {
        while(1); 
    }

    DISABLE_INT();  		

    s_tTmr[_id].Count = _period;			
    s_tTmr[_id].PreLoad = _period;		/*  */
    s_tTmr[_id].Flag = 0;				/*  */
    s_tTmr[_id].Mode = TMR_AUTO_MODE;	/*  */

    ENABLE_INT();  			/*  */
}
//停止"定时器"
void bsp_StopTimer(uint8_t _id)
{
    if (_id >= TMR_COUNT)
    {
        /*  */
        while(1); /*  */
    }

    DISABLE_INT();  	/*  */

    s_tTmr[_id].Count = 0;				/*  */
    s_tTmr[_id].Flag = 0;				/*  */
    s_tTmr[_id].Mode = TMR_ONCE_MODE;	/*  */

    ENABLE_INT();  		/*  */
}
//
//在程序中检查"定时器"的标志是否已经达到flag
uint8_t bsp_CheckTimer(uint8_t _id)
{
    if (_id >= TMR_COUNT)
    {
        return 0;
    }

    if (s_tTmr[_id].Flag == 1)
    {
        s_tTmr[_id].Flag = 0;
        return 1;
    }
    else
    {
        return 0;
    }
}
//

//定时器中断中更新结构体数组的数值
static void bsp_SoftTimerDec(SOFT_TMR *_tmr)
{
    if (_tmr->Count > 0)
    {
        
        if (--_tmr->Count == 0)
        {
            _tmr->Flag = 1;

            if(_tmr->Mode == TMR_AUTO_MODE)
            {
                _tmr->Count = _tmr->PreLoad;
            }
        }
    }
}
//
void disposeAllTimeData(void)
{
    if (s_uiDelayCount > 0)
    {
        if (--s_uiDelayCount == 0)
        {
            s_ucTimeOutFlag = 1;
        }
    }

    for (uint8_t i = 0; i < TMR_COUNT; i++)
    {
        bsp_SoftTimerDec(&s_tTmr[i]);
    }

    g_iRunTime++;
    if (g_iRunTime == 0x7FFFFFFF)	
    {
        g_iRunTime = 0;
    }

}

void ReloadOutCal(void)
{
    s_tTmr[TMR_OUT_CTRL].Count = s_tTmr[TMR_OUT_CTRL].PreLoad;
}
void RDA5807_ResumeImmediately(void)
{
    if(s_tTmr[TMR_FM_CTRL].Count>0)
    {
        bsp_StopTimer(TMR_FM_CTRL);
        if(A002_SQ_READ)
            RDA5807_Init(ON);
    }
}


