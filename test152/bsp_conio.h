#ifndef __BSP_CONIO_H__
#define __BSP_CONIO_H__
#include "FCS152_KDU.h"
#include "bsp_m62364.h"
#include "bsp_ch423.h"

//////////////////////////////////Analog////////////////////////////////////////////////////////
#define ADC_PIN                         GPIO_NUM_10
#define DAC_PIN                         GPIO_NUM_18
#define PWM_PIN                         GPIO_NUM_35

///////////////////////////////////独立按键//////////////////////////////////////////////////////
#define WAKE_UP_PIN                     GPIO_NUM_21

#define VOL_ADD_PIN                     GPIO_NUM_6
#define VOL_ADD_READ                    digitalRead(VOL_ADD_PIN)

#define VOL_SUB_PIN                     GPIO_NUM_7
#define VOL_SUB_READ                    digitalRead(VOL_SUB_PIN)

#define PTT_PIN                         GPIO_NUM_4
#define PTT_READ                        digitalRead(PTT_PIN)

#define SQUELCH_PIN                     GPIO_NUM_5
#define SQUELCH_READ                    digitalRead(SQUELCH_PIN)

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#define ENCODER_SPIN_L_PIN              GPIO_NUM_20
#define ENCODER_SPIN_L_READ             digitalRead(ENCODER_SPIN_L_PIN)

#define ENCODER_SPIN_R_PIN              GPIO_NUM_19
#define ENCODER_SPIN_R_READ             digitalRead(ENCODER_SPIN_R_PIN)

#define ENCODER_CLICK_PIN               WAKE_UP_PIN
#define ENCODER_CLICK_READ              digitalRead(ENCODER_CLICK_PIN)
////////////////////////////////电源////////////////////////////////////////////////////////

#define POWER_EN_PIN                    GPIO_NUM_33
#define POWER_EN_SET                    digitalWrite(POWER_EN_PIN, HIGH)
#define POWER_EN_CLR                    digitalWrite(POWER_EN_PIN, LOW)


#define POWER_EN_8_SET                  SET_POWER_EN_8_CHAN
#define POWER_EN_8_CLR                  CLR_POWER_EN_8_CHAN
#define POWER_EN_12_SET                 SET_POWER_EN_12_CHAN
#define POWER_EN_12_CLR                 CLR_POWER_EN_12_CHAN

/*************************FM运放**************************/

#define FM_AMP_EN_SET                   CLR_FM_AMP_EN_CHAN
#define FM_AMP_EN_CLR                   SET_FM_AMP_EN_CHAN

/************************FM电源***************************/
#define FM_S_EN_SET                     SET_FM_S_EN
#define FM_S_EN_CLR                     CLR_FM_S_EN

/************************6针电源输出使能****************/
#define VDO_SET                        SET_VDO_PIN
#define VDO_CLR                        CLR_VDO_PIN

/**********************A20初始化引脚**************************/

#define A002_SQ_PIN                     GPIO_NUM_15
#define A002_PD_PIN                     GPIO_NUM_16
#define A002_PTT_PIN                    GPIO_NUM_14

#define A002_SQ_READ                    digitalRead(A002_SQ_PIN)

#define A002_PD_SET                     digitalWrite(A002_PD_PIN,  HIGH)
#define A002_PD_CLR                     digitalWrite(A002_PD_PIN,  LOW)
#define A002_PTT_SET                    digitalWrite(A002_PTT_PIN, HIGH)
#define A002_PTT_CLR                    digitalWrite(A002_PTT_PIN, LOW)


/**************************音频出入使能引脚*****************************/

#define  MIC_IN_SET                     SET_MIC_IN_EN_PIN
#define MIC_OUT_SET                     CLR_MIC_OUT_EN_PIN
#define  SPK_IN_SET                     CLR_SPK_IN_EN_PIN
#define SPK_OUT_SET                     CLR_SPK_OUT_EN_PIN

#define  MIC_IN_CLR                     CLR_MIC_IN_EN_PIN
#define MIC_OUT_CLR                     SET_MIC_OUT_EN_PIN
#define  SPK_IN_CLR                     SET_SPK_IN_EN_PIN
#define SPK_OUT_CLR                     SET_SPK_OUT_EN_PIN


void Function_Key_Init(void);       //独立功能按键初始化
void Encoder_Click_Init(void);      //编码器按下初始化
void Encoder_Spin_init(void);       //编码器旋转初始化

void ControlGPIO_Init(void);            //使能引脚初始化
void bsp_A002_Init(void);           //A20控制引脚初始化

#endif

