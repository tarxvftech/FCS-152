/*
    bsp_Timer.h
*/
#ifndef __BSP_TIMER_H__
#define __BSP_TIMER_H__

#include "FCS152_KDU.h"

/*  The timer ISR has an execution time of 5.5 micro-seconds(us).
    Therefore, a timer period less than 5.5 us will cause trigger the interrupt watchdog.
    7 us is a safe interval that will not trigger the watchdog. No need to customize it.
*/
#define TIMER_INTR_US           7                                                   // Execution time of each ISR interval in micro-seconds
#define TIMER_DIVIDER           16
#define SEC_TO_MICRO_SEC(x)    ((x) / 1000 / 1000)                                  // Convert second to micro-second
#define TIMER_TICKS            (TIMER_BASE_CLK / TIMER_DIVIDER)                     // TIMER_BASE_CLK = APB_CLK = 80MHz
#define ALARM_VAL_US           SEC_TO_MICRO_SEC(TIMER_INTR_US * TIMER_TICKS)        // Alarm value in micro-seconds

#define AMP_DAC                 80                                                 // Amplitude of DAC voltage. If it's more than 256 will causes dac_output_voltage() output 0.
#define DAC_CHAN                DAC_CHANNEL_2
//The number of output wave points.
#define DAC_SAMPLE_2K           (int)(1000000 / (TIMER_INTR_US * 2000) + 0.5)       //dac2K采样数据
#define DAC_SAMPLE_1_5K         (int)(1000000 / (TIMER_INTR_US * 1500) + 0.5)       //dac1.5K采样数据

#define CONST_PERIOD_2_PI      6.2832



//TIMER0
void    bsp_Timer0_Init(void);
void    timer0_cb(void* arg);
//TIMER1
void    bsp_Timer1_Init(void);
void    timer1_cb(void);

void    bsp_Timer2_Init(void);

#endif
