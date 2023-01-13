#include "bsp_lcd.h"

void LCD_GPIO_Init(void) {
    pinMode(LCD_SCL_PIN, OUTPUT);
    pinMode(LCD_SDA_PIN, OUTPUT);
    pinMode(LCD_RS_PIN, OUTPUT);
    pinMode(LCD_RST_PIN, OUTPUT);
    pinMode(LCD_CS_PIN, OUTPUT);

    LCD_CS_SET;
    LCD_RST_SET;
    LCD_RS_SET;
    LCD_SDA_SET;
    LCD_SCL_SET;
}


