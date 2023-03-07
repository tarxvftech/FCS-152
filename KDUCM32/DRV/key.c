#include "key.h"
#include "tim.h"
#include "bsp_delay.h"

extern void LightBacklight(void);
extern u8  LCD_INVERTED;

void LEDK_Init()
{
	TIM_TimeBaseInitType htim4;
	OCInitType sConfigOC = {0};
	GPIO_InitType GPIO_InitStruct;
	
	RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_TIM4, ENABLE);
	RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB | RCC_APB2_PERIPH_AFIO, ENABLE);
	
	GPIO_InitStruct.Pin 			= LEDK_PIN;
	GPIO_InitStruct.GPIO_Pull 		= GPIO_No_Pull;
	GPIO_InitStruct.GPIO_Slew_Rate  = GPIO_Slew_Rate_High;
	GPIO_InitStruct.GPIO_Current 	= GPIO_DC_12mA;
	GPIO_InitStruct.GPIO_Mode 		= GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Alternate 	= GPIO_AF2_TIM4;
	GPIO_InitPeripheral(LEDK_GPIO, &GPIO_InitStruct);
	
	uint16_t PrescalerValue = 0;
	uint32_t tim4Freq		= 5000000;
	RCC_ClocksType RCC_ClockFreq;
	RCC_GetClocksFreqValue(&RCC_ClockFreq);
	
	/* Compute the prescaler value */
	//You need to judge the size of "HclkFreq". If it exceeds 27M, you need to divide the frequency. At the same time, the frequency doubling 
	//of the timer requires x2 when the frequency division coefficient is not 1.
	
	//SystemCoreClock
	PrescalerValue = (uint16_t)( (RCC_ClockFreq.HclkFreq>27000000?RCC_ClockFreq.Pclk1Freq*2:RCC_ClockFreq.Pclk1Freq) / tim4Freq) - 1;
	
	
	htim4.Prescaler = PrescalerValue;
	htim4.Period 	= 100;				//AutoReload
	htim4.ClkDiv    = 0;
	htim4.CntMode   = TIM_CNT_MODE_UP;
	TIM_InitTimeBase(TIM4, &htim4);
	
	sConfigOC.OcMode      = TIM_OCMODE_PWM1;
    sConfigOC.OutputState = TIM_OUTPUT_STATE_ENABLE;
    sConfigOC.Pulse       = 50;				//CCDAT4, Preload
    sConfigOC.OcPolarity  = TIM_OC_POLARITY_HIGH;
    TIM_InitOc4(TIM4, &sConfigOC);
	TIM_ConfigOc4Preload(TIM4, TIM_OC_PRE_LOAD_ENABLE);
 
    TIM_ConfigArPreload(TIM4, ENABLE);
    TIM_Enable(TIM4, ENABLE);
	
}
//
void KEY_Init(void)
{
	K_GPIO_2_CLK_EN;
	K_GPIO_1_CLK_EN;
	K_GPIO_0_CLK_EN;
	
	GPIO_InitType GPIO_InitStructure;
	GPIO_InitStruct(&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode 		= GPIO_Mode_Input;
	GPIO_InitStructure.GPIO_Pull 		= GPIO_Pull_Up;
	GPIO_InitStructure.GPIO_Slew_Rate	= GPIO_Slew_Rate_High;
	
	GPIO_InitStructure.Pin	 = 	K8_PIN | K9_PIN | PRE_SUB_PIN | K_ENT_PIN |
							KD_PIN | KC_PIN | KN_PIN;
	GPIO_InitPeripheral(K_GPIO_2, &GPIO_InitStructure);
	
	GPIO_InitStructure.Pin	 = 	K_TOOLS_PIN | VOL_SUB_PIN | K0_PIN | K1_PIN | 
							K2_PIN | K3_PIN | K4_PIN | K5_PIN; 
	GPIO_InitPeripheral(K_GPIO_1, &GPIO_InitStructure);
	
	GPIO_InitStructure.Pin	 =  VOL_ADD_PIN | K6_PIN | K7_PIN | KB_PIN |
							KA_PIN | K_CLR_PIN | PRE_ADD_PIN; 
	GPIO_InitPeripheral(K_GPIO_0, &GPIO_InitStructure); 	
}
//
u8 KEY_SCAN(u8 MODE)
{
	static u8 key_up=1;
	
	if(MODE)key_up=1;
	if(key_up && (K0_READ==0 		|| K1_READ==0 		|| K2_READ==0 		|| K3_READ==0 		|| K4_READ==0 		||
		      K5_READ==0 		|| K6_READ==0 		|| K7_READ==0 		|| K8_READ==0 		|| K9_READ==0 		||
		      PRE_ADD_READ==0 		|| PRE_SUB_READ==0 	|| K_TOOLS_READ==0 	|| K_ENT_READ==0 	|| K_CLR_READ==0 	||
		      KA_READ==0 		|| KB_READ==0 		|| KC_READ==0 		|| KD_READ==0 		|| KN_READ==0 ))
	 {
			LightBacklight();
		 	delay_ms(5);
			key_up=0;
			     if(K0_READ	    ==0)    return LCD_INVERTED?key_9:key_0;
			else if(K1_READ	    ==0)    return LCD_INVERTED?key_8:key_1;
			else if(K2_READ     ==0)    return LCD_INVERTED?key_7:key_2;
			else if(K3_READ     ==0)    return LCD_INVERTED?key_6:key_3;
			else if(K4_READ     ==0)    return LCD_INVERTED?key_5:key_4;
			else if(K5_READ     ==0)    return LCD_INVERTED?key_4:key_5;
			else if(K6_READ     ==0)    return LCD_INVERTED?key_3:key_6;
			else if(K7_READ     ==0)    return LCD_INVERTED?key_2:key_7;
			else if(K8_READ     ==0)    return LCD_INVERTED?key_1:key_8;
			else if(K9_READ     ==0)    return LCD_INVERTED?key_0:key_9;
		 
			else if(PRE_ADD_READ==0)    return LCD_INVERTED?key_pre_sub:key_pre_add;
			else if(PRE_SUB_READ==0)    return LCD_INVERTED?key_pre_add:key_pre_sub;
			else if(K_TOOLS_READ==0)    return key_tools;
			else if(K_ENT_READ  ==0)    return key_ent;
			else if(K_CLR_READ  ==0)    return key_clr;
			else if(KA_READ     ==0)    return LCD_INVERTED?key_d:key_a;
			else if(KB_READ	    ==0)    return LCD_INVERTED?key_c:key_b;
			else if(KC_READ     ==0)    return LCD_INVERTED?key_b:key_c;
			else if(KD_READ     ==0)    return LCD_INVERTED?key_a:key_d;
			else if(KN_READ     ==0)    return key_ent ;			//key_ent
			
	 }                         
	 else if(K0_READ && K1_READ && K2_READ && K3_READ && K4_READ && K5_READ && K6_READ && K7_READ && 
		 K8_READ && K9_READ && PRE_ADD_READ && PRE_SUB_READ && 
		 K_TOOLS_READ && K_ENT_READ && K_CLR_READ && KA_READ && KB_READ && KC_READ && KD_READ && KN_READ)
		
		key_up=1;
	 
 	return key_no;

}
//


