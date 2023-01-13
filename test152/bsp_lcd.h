/*
    bsp_lcd.h
*/
#ifndef __BSP_LCD_H__
#define __BSP_LCD_H__

#include "FCS152_KDU.h"

#define  _LCD           LCD12832

#define LCD_CS_PIN      36
#define LCD_CS_SET      digitalWrite(LCD_CS_PIN, 1)
#define LCD_CS_CLR      digitalWrite(LCD_CS_PIN, 0)

#define LCD_RST_PIN     37
#define LCD_RST_SET     digitalWrite(LCD_RST_PIN, 1)
#define LCD_RST_CLR     digitalWrite(LCD_RST_PIN, 0)

#define LCD_RS_PIN      38
#define LCD_RS_SET      digitalWrite(LCD_RS_PIN, 1)
#define LCD_RS_CLR      digitalWrite(LCD_RS_PIN, 0)

#define LCD_SDA_PIN     39
#define LCD_SDA_SET     digitalWrite(LCD_SDA_PIN, 1)
#define LCD_SDA_CLR     digitalWrite(LCD_SDA_PIN, 0)

#define LCD_SCL_PIN     40
#define LCD_SCL_SET     digitalWrite(LCD_SCL_PIN, 1)
#define LCD_SCL_CLR     digitalWrite(LCD_SCL_PIN, 0)


void LCD_GPIO_Init(void);



#endif
