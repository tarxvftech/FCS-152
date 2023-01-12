#ifndef  __KEY_H__
#define  __KEY_H__
#include "FCS152_KDU.h"
//按键灯
#define LEDK_GPIO  				GPIOB 			
#define LEDK_PIN   				GPIO_PIN_9
#define LEDK_CLK_EN 			
//按键
#define K_GPIO_2				GPIOA
#define K8_PIN					GPIO_PIN_7
#define K9_PIN					GPIO_PIN_6
#define PRE_SUB_PIN 			GPIO_PIN_4
#define K_ENT_PIN   			GPIO_PIN_3
#define KD_PIN					GPIO_PIN_2
#define KC_PIN					GPIO_PIN_1
#define KN_PIN					GPIO_PIN_0
#define K_GPIO_2_CLK_EN 		RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);

#define K_GPIO_1				GPIOB
#define K_TOOLS_PIN 			GPIO_PIN_15
#define VOL_SUB_PIN  			GPIO_PIN_14
#define K0_PIN      			GPIO_PIN_3
#define K1_PIN      			GPIO_PIN_4
#define K2_PIN					GPIO_PIN_5
#define K3_PIN					GPIO_PIN_6
#define K4_PIN					GPIO_PIN_7
#define K5_PIN					GPIO_PIN_0
#define K_GPIO_1_CLK_EN 		RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB, ENABLE);
	
#define K_GPIO_0				GPIOC
#define VOL_ADD_PIN 			GPIO_PIN_6
#define K6_PIN					GPIO_PIN_5
#define K7_PIN					GPIO_PIN_4
#define KB_PIN					GPIO_PIN_3
#define KA_PIN					GPIO_PIN_2
#define K_CLR_PIN   			GPIO_PIN_1
#define PRE_ADD_PIN 			GPIO_PIN_0
#define K_GPIO_0_CLK_EN 		RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOC, ENABLE);	
		
//键态获取
#define K0_READ					(K_GPIO_1->PID &= K0_PIN)
#define K1_READ					(K_GPIO_1->PID &= K1_PIN)
#define K2_READ         		(K_GPIO_1->PID &= K2_PIN)
#define K3_READ         		(K_GPIO_1->PID &= K3_PIN)
#define K4_READ         		(K_GPIO_1->PID &= K4_PIN)
#define K5_READ         		(K_GPIO_1->PID &= K5_PIN)
#define K6_READ         		(K_GPIO_0->PID &= K6_PIN)
#define K7_READ         		(K_GPIO_0->PID &= K7_PIN)
#define K8_READ         		(K_GPIO_2->PID &= K8_PIN)
#define K9_READ         		(K_GPIO_2->PID &= K9_PIN)
#define VOL_ADD_READ    		(K_GPIO_0->PID &= VOL_ADD_PIN)
#define VOL_SUB_READ    		(K_GPIO_1->PID &= VOL_SUB_PIN)
#define PRE_ADD_READ    		(K_GPIO_0->PID &= PRE_ADD_PIN)
#define PRE_SUB_READ    		(K_GPIO_2->PID &= PRE_SUB_PIN)
#define K_TOOLS_READ    		(K_GPIO_1->PID &= K_TOOLS_PIN)
#define K_ENT_READ      		(K_GPIO_2->PID &= K_ENT_PIN)
#define K_CLR_READ      		(K_GPIO_0->PID &= K_CLR_PIN)
#define KA_READ         		(K_GPIO_0->PID &= KA_PIN)
#define KB_READ         		(K_GPIO_0->PID &= KB_PIN)
#define KC_READ         		(K_GPIO_2->PID &= KC_PIN)
#define KD_READ         		(K_GPIO_2->PID &= KD_PIN)
#define KN_READ         		(K_GPIO_2->PID &= KN_PIN) 
/*
#define key_0			0	 
#define key_1  		    1
#define key_2  		    2
#define key_3			3
#define key_4			4
#define key_5			5
#define key_6			6
#define key_7			7
#define key_8			8
#define key_9			9
#define key_vol_add     10
#define key_vol_sub     11
#define key_tools       12
#define key_pre_add     13
#define key_pre_sub     14
#define key_clr		    15
#define key_ent         16
#define key_a		    17	//上
#define key_b		    18	//左
#define key_c		    19	//右
#define key_d		    20	//下
#define key_n		    21	//中键确认
#define key_no        22
*/



enum KEY_RETURN
{
	key_0		 	,
	key_1  		 	,
	key_2  		 	,
	key_3			,
	key_4			,
	key_5			,
	key_6			,
	key_7			,
	key_8			,
	key_9			,
	key_pre_add		,
	key_pre_sub		,
	key_tools  		,
	key_ent    		,
	key_clr		 	,
	key_a		   	,
	key_b		   	,
	key_c		   	,	
	key_d		   	,
	key_n		   	,
	key_vol_add		,
	key_vol_sub		,
	key_no
};


void LEDK_Init(void);
void KEY_Init(void);
u8 KEY_SCAN(u8 MODE);
                                                                    
                                                                     
#endif


