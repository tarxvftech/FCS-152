#include "bsp_lcd.h"


void LCD_GPIO_Init(void)
{
	GPIO_InitType GPIO_InitStructure;
	RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB, ENABLE);
	
	GPIO_InitStruct(&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin  = LCD_CS_PIN | LCD_RST_PIN | LCD_RS_PIN| LCD_SCL_PIN | LCD_SDA_PIN;// | GPIO_PIN_9
	GPIO_InitStructure.GPIO_Mode 		= GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pull 		= GPIO_Pull_Up;
	GPIO_InitStructure.GPIO_Slew_Rate	= GPIO_Slew_Rate_High;
	GPIO_InitPeripheral(LCD_GPIO, &GPIO_InitStructure);
	//LCD_GPIO->POD |= GPIO_PIN_9;
}





















