#ifndef __BSP_LCD_H__
#define __BSP_LCD_H__
#include "FCS152_KDU.h"


#define _LCD		 		LCD12864

#define  LCD_GPIO         	GPIOB
#define  LCD_CS_PIN 		GPIO_PIN_2
#define  LCD_RST_PIN      	GPIO_PIN_10
#define  LCD_RS_PIN       	GPIO_PIN_11
#define  LCD_SCL_PIN      	GPIO_PIN_12
#define  LCD_SDA_PIN      	GPIO_PIN_13

#define  LCD_CS_CLR   		LCD_GPIO->POD &=~LCD_CS_PIN
#define  LCD_CS_SET   		LCD_GPIO->POD |= LCD_CS_PIN
		
#define  LCD_RST_CLR  		LCD_GPIO->POD &=~LCD_RST_PIN
#define  LCD_RST_SET  		LCD_GPIO->POD |= LCD_RST_PIN	
		
#define  LCD_RS_CLR   		LCD_GPIO->POD &=~LCD_RS_PIN
#define  LCD_RS_SET   		LCD_GPIO->POD |= LCD_RS_PIN	
		
#define  LCD_SCL_CLR  		LCD_GPIO->POD &=~LCD_SCL_PIN
#define  LCD_SCL_SET  		LCD_GPIO->POD |= LCD_SCL_PIN	
		
#define  LCD_SDA_CLR  		LCD_GPIO->POD &=~LCD_SDA_PIN
#define  LCD_SDA_SET  		LCD_GPIO->POD |= LCD_SDA_PIN

void LCD_GPIO_Init(void);



#endif
