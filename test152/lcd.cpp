#include "lcd.h"
#include "bsp_lcd.h"                //Driver pin initialization
#include "bsp_delay.h"

#define INVERTED_OFFSET 4
u8 LCD_INVERTED = OFF;              //LCD upside down display

void LCD_Init(void) {
    LCD_GPIO_Init();
#if _LCD == LCD12864
    LCD_CS_SET;
    LCD_RST_CLR;
    delay_ms(10); //hard reset for 10ms
    //Physical reset
    LCD_RST_SET;
    delay_ms(10);
    //Software reset
    LCD_Write(0xE2,0);
    delay_ms(10);//Soft Reset

    LCD_Write(0xA2,0); //Duty bias set  0xA2 is light 0xA3 is dark
    //LCD_Write(0xA7,0); //0xA7 reverse 0xA6 normal default normal

    LCD_Write(0xA0,0); //ADC select S0->S131(from S1-S128)  a0: Left->right a1: Reverse, right to left
    LCD_Write(0xC8,0); //com1 --> com64                     C8: Normal sequence C0: Reverse scan

    //Coarse contrast    //0x20-0x27 Internal resistance ratio
    LCD_Write(0x20+(5&0x07), 0); //V0 Voltage Regulator Internal Resistor Ratio Set 0x20~0x27//1
    delay_ms(2);

    //Fine-tune contrast       //0-0x3F Internal resistance fine-tuning
    LCD_Write(0x81,0); //V0 voltage set first cmd
    LCD_Write(36 & 0x3F, 0); //following V0 voltage set 0x00~0x3F
    delay_ms(2);

    //Boost multiple 00:4   01:5   10:6
    LCD_Write(0xF8,0); //Booster Ratio Select Mode Set first cmd
    LCD_Write(0x00 & 0x03,0); //following Booset Ratio Register Set 0x00~0x03
    delay_ms(2);

    //Select the internal voltage supply operation mode 0010 Lower four digits：1 VB VR VF
    //LCD_Write(0x2F,0); //power control all on
    LCD_Write(0x28+(0x07&7),0);
    delay_ms(2);

    //Set the display initial line of the display memory, which can be set to 0x40~0x7F, which represents lines 0~63 respectively.
    LCD_Write(0x40 + (0x3f&0), 0); //0x40 + Display Start Line(0)

    //Clear screen
    LCD_Clear(GLOBAL64);

    //Turn on the display
    LCD_Write(0xAF,0); //Display ON
#else
    LCD_RST_CLR;
    delay_us(10);
    LCD_RST_SET;
    delay_us(10);

    LCD_WriteCmd(0xe2);     //Software reset
    delay_ms(10);

    LCD_WriteCmd(0x2c);     //Boost step gathering
    delay_ms(5);

    LCD_WriteCmd(0x2e);     //Boost step gathering
    delay_ms(5);

    LCD_WriteCmd(0x2f);     //Boost step gathering
    delay_ms(5);

    LCD_WriteCmd(0x23);     //Coarse contrast, range can be set 20～27

    LCD_WriteCmd(0x81);     //Fine-tune contrast
    LCD_WriteCmd(0x10);     //Fine-tune the value of the contrast, you can set the range 0x00～0x3F

    LCD_WriteCmd(0xa2);     //1/9 Bias ratio（bias）

#if LCD_INVERTED==1
    LCD_WriteCmd(0xc0);     //Row scan order: from top to bottom
    LCD_WriteCmd(0xa1);     //Column scan order: reverse, from right to left
#else
    LCD_WriteCmd(0xc8);     //Row scan order: from top to bottom
    LCD_WriteCmd(0xa0);     //Column scan order: from left to right
#endif
    LCD_WriteCmd(0x40);     //Starting line: start from the first line
    LCD_WriteCmd(0xaF);     //Open display

    LCD_HeavyRatio(3);      //3
    LCD_Clear(GLOBAL32);
#endif
}

void LCD_Write(unsigned char dat,unsigned char rs) { //rs == 1 data rs==0  cmd
    uint8_t i;
    LCD_SCL_CLR;
    LCD_CS_CLR;
    if (rs) {
        LCD_RS_SET;
    } else {
        LCD_RS_CLR;    //Command
    }

    delay_us(10);

    for (i=0; i<8; i++) {
        if ((dat<<i) & 0x80) {
            LCD_SDA_SET;
        } else {
            LCD_SDA_CLR;
        }
        LCD_SCL_SET;
        delay_us(10);
        LCD_SCL_CLR;
        delay_us(10);
    }
    delay_us(10);
    LCD_CS_SET;
    LCD_RS_SET;
    delay_us(10);
}

void LCD_WriteCmd(unsigned char cmd) {
    LCD_Write(cmd,0);
}

void LCD_WriteData(unsigned char data) {
    LCD_Write(data,1);
}

//Coarse contrast  //0x20-0x27 Internal resistance ratio
void LCD_HeavyRatio(unsigned char cmd) {
    LCD_CS_CLR;
    LCD_RS_CLR;
    LCD_WriteCmd(0x20+(cmd&0x07));  //0x2x
    LCD_CS_SET;
}
//Fine-tune contrast   //0-0x3F Internal resistance fine-tuning
void LCD_LightRatio(unsigned char cmd) {
    LCD_CS_CLR;
    LCD_RS_CLR;
    LCD_WriteCmd(0x81);
    LCD_WriteCmd(cmd & 0x3f);   //00-3F---》0~48+15=63
    LCD_CS_SET;
}
void LCD_Setxy(unsigned char l, unsigned char p) {
    LCD_WriteCmd(0xb0+p);               //Set Page Address                                                          x
    LCD_WriteCmd(((l>>4)&0x0f)+0x10);   //Set Column Address(MSByte) = 0            0001 l&f   （High four）          y
    LCD_WriteCmd(l&0x0f);               //Colum(LSByte) from S1 -> S128 auto add    0000 l&0x0f（Lower four digits）
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//Page-based address positioning
//4 columns and 8 lines of characters (16 pages)
void LCD_ShowAscii0408(unsigned char l, unsigned char p, int ch) {
    if (LCD_INVERTED) {
        l+=INVERTED_OFFSET;
    }

    ch-=0x20;
    LCD_Setxy(l, p);
    for (int i=0; i<4; i++) {
        LCD_WriteData(ascii_0408[ch][i]);
    }
}
void LCD_ShowString0408(unsigned char l, unsigned char p, const char * s, unsigned char flag) {
    if (LCD_INVERTED) {
        l+=INVERTED_OFFSET;
    }
    LCD_Setxy(l, p);
    for (u8 i=0; s[i]!='\0'; i++) {
        for (u8 j=0; j<4; j++) {
            if (flag) {
                LCD_WriteData(ascii_0408[s[i]-0x20][j]);
            } else {
                LCD_WriteData(~ascii_0408[s[i]-0x20][j]);
            }
        }
    }
}
//
//6 columns and 8 lines of characters (16 pages)
void LCD_ShowAscii0608(unsigned char l, unsigned char p, int ch, unsigned char flag) {
    if (LCD_INVERTED) {
        l+=INVERTED_OFFSET;
    }
    ch-=0x20;
    LCD_Setxy(l, p);
    for (u8 i=0; i<6; i++) {
        if (flag) {
            LCD_WriteData(ascii_0608[ch][i]);
        } else {
            LCD_WriteData(~ascii_0608[ch][i]);
        }
    }
}
void LCD_ShowString0608(unsigned char l, unsigned char p, const char * s, unsigned char flag, unsigned char limit) {
    if (LCD_INVERTED) {
        l+=INVERTED_OFFSET;
    }
    LCD_Setxy(l, p);
    for (u8 j=0; s[j]!='\0'; j++) {         //j word, column i
        for (u8 i=0; i<6; i++) {
            if (j*6+i<=limit) {
                if (flag) {
                    LCD_WriteData(ascii_0608[s[j]-0x20][i]);
                } else {
                    LCD_WriteData(~ascii_0608[s[j]-0x20][i]);
                }
            }
        }
    }
}
//
//10 columns and 16 lines of characters (16 pages)
void LCD_ShowAscii1016(unsigned char l, unsigned char p, int ch, unsigned char flag) {
    if (LCD_INVERTED) {
        l+=INVERTED_OFFSET;
    }
    int i;
    ch-=0x20;
    LCD_Setxy(l, p);
    for (i=0; i<10; i++) {
        if (flag) {
            LCD_WriteData(ascii_1016[ch][i]);
        } else {
            LCD_WriteData(~ascii_1016[ch][i]);
        }
    }

    LCD_Setxy(l, p+1);

    for (i=10; i<20; i++) {
        if (flag) {
            LCD_WriteData(ascii_1016[ch][i]);
        } else {
            LCD_WriteData(~ascii_1016[ch][i]);
        }
    }
}
void LCD_ShowString1016(unsigned char l, unsigned char p, const char * s, unsigned char flag, unsigned char limit) {
    if (LCD_INVERTED) {
        l+=INVERTED_OFFSET;
    }
    int i,j;
    LCD_Setxy(l, p);

    for (j=0; s[j]!='\0'; j++) {
        for (i=0; i<10; i++) {
            if (j*10+i<=limit) {
                if (flag) {
                    LCD_WriteData(ascii_1016[s[j]-0x20][i]);
                } else {
                    LCD_WriteData(~ascii_1016[s[j]-0x20][i]);
                }
            }
        }
    }
    //

    LCD_Setxy(l, p+1);
    for (j=0; s[j]!='\0'; j++) {
        for (i=10; i<20; i++) {
            if (j*10+i<=limit) {
                if (flag) {
                    LCD_WriteData(ascii_1016[s[j]-0x20][i]);
                } else {
                    LCD_WriteData(~ascii_1016[s[j]-0x20][i]);
                }
            }
        }
    }
    //
}
//
void LCD_ShowPICALL(const unsigned char * pic) {
    u8 l = 0;
    u8 p = 0;

#if _LCD == LCD12864
    p+=2;
#endif
    if (LCD_INVERTED) {
        l+=INVERTED_OFFSET;
    }

    for (u8 i=p; i<p+4; i++) {
        LCD_Setxy(0, i);
        for (u8 j=0; j<128; j++) {
            LCD_WriteData(*pic++);
        }
    }
    //delay_ms(500);
}

//Up and down arrows, small volume black block
void LCD_ShowPIC0408(unsigned char l, unsigned char p, unsigned char ch) {
    if (LCD_INVERTED) {
        l+=INVERTED_OFFSET;
    }

    LCD_Setxy(l, p);
    for (u8 i=0; i<4; i++) {
        LCD_WriteData(pic_0408[ch][i]);
    }
}
//
void LCD_ShowPIC0608(unsigned char l, unsigned char p, int ch, unsigned char flag) {
    if (LCD_INVERTED) {
        l+=INVERTED_OFFSET;
    }

    LCD_Setxy(l, p);
    for (u8 i=0; i<6; i++)
        if (flag) {
            LCD_WriteData(pic_0608[ch][i]);
        } else {
            LCD_WriteData(~pic_0608[ch][i]);
        }
}
//
void LCD_ShowPIC0808(unsigned char l, unsigned char p, int ch) {
    if (LCD_INVERTED) {
        l+=INVERTED_OFFSET;
    }

    LCD_Setxy(l, p);
    for (u8 i=0; i<8; i++) {
        LCD_WriteData(pic_0808[ch][i]);
    }
}
//
void LCD_ShowPIC1616(unsigned char l, unsigned char p, int ch, unsigned char flag) {
    if (LCD_INVERTED) {
        l+=INVERTED_OFFSET;
    }

    ch*=32;
    LCD_Setxy(l, p);
    for (int i=ch; i<ch+16; i++)
        if (flag) {
            LCD_WriteData(pic_1616[i]);
        } else {
            LCD_WriteData(~pic_1616[i]);
        }

    LCD_Setxy(l, p+1);
    for (int i=ch+16; i<ch+32; i++)
        if (flag) {
            LCD_WriteData(pic_1616[i]);
        } else {
            LCD_WriteData(~pic_1616[i]);
        }
}
//Display of buttons 0-9
void LCD_ShowPIC2516(unsigned char l, unsigned char p, int ch) {
    if (LCD_INVERTED) {
        l+=INVERTED_OFFSET;
    }

    LCD_Setxy(l, p);
    for (int i=0; i<25; i++) {
        LCD_WriteData(pic2516[ch][i]);
    }

    LCD_Setxy(l, p+1);
    for (int i=25; i<50; i++) {
        LCD_WriteData(pic2516[ch][i]);
    }
}
//
void LCD_Clear(_ClearScope clean_area) {
    u8 i, j, start_page=0, stop_page=0;
    switch (clean_area) {
    case GLOBAL32:
        start_page = 0;
        stop_page  = 3;
        break;

    case EDITZONE32:
        start_page = 1;
        stop_page  = 3;
        break;

    case GLOBAL64:
//      LCD_ShowString0608(0, 0, "                      ", 1, 128);
//      LCD_ShowString0608(0, 1, "                      ", 1, 128);
//      LCD_ShowString0608(0, 6, "                      ", 1, 128);
//      LCD_ShowString0608(0, 7, "                      ", 1, 128);
//      for (i = 0; i < 5; i++)
//      {
//          LCD_ShowPIC2516(26 * i, 0, i);
//          LCD_ShowPIC2516(26 * i, 6, i + 5);
//          if(i>0)
//              LCD_ShowString0608(0, i+1, "                      ", 1, 128);
//      }
        for (u8 i=0; i<8; i++) {
            LCD_Setxy(LCD_INVERTED?INVERTED_OFFSET:0, i);
            for (u8 j=0; j<128; j++) {
                LCD_WriteData(pic_KDUClear[i*128+j]);
            }
        }
        return;

    case EDITZONE64:
        start_page = 3;
        stop_page  = 5;
        break;
    }
    for (j=start_page; j<=stop_page; j++) {
        LCD_Setxy(0, j);
        for (i=0; i<128; i++) {
            LCD_WriteData(0);
        }
    }



}
//

//Upgrade process bar display
void LCD_ShowProcessBar(unsigned char l, unsigned char p, unsigned char num) {
    if (LCD_INVERTED) {
        l+=INVERTED_OFFSET;
    }

    LCD_Setxy(l, p);
    for (int i=0; i<num; i++) {
        LCD_WriteData(0xff);
    }

    char buf[4]= {0};
    sprintf(buf, "%d%%", num);
    LCD_ShowString0608(l+102, p, buf, 1, 128);

}
//Solid percentage display (including upper and lower borders)
void LCD_ShowPercentBar(unsigned char l, unsigned char p, unsigned char all_level, unsigned char level, unsigned char length) {
    if (level>all_level) {
        return;
    }

    int sum_draw = length*level/all_level;

    if (LCD_INVERTED) {
        l+=INVERTED_OFFSET;
    }

    LCD_Setxy(l, p);
//////////////////////////////////////////////////
    LCD_WriteData(0x3e);                        //Border

    for (u8 i=0; i<sum_draw; i++) {
        LCD_WriteData(0x3e);
    }

    for (u8 y=0; y<length-sum_draw; y++) {
        LCD_WriteData(0x22);
    }

    LCD_WriteData(0x3e);                        //Border
//////////////////////////////////////////////////
}
//Battery level display
void LCD_ShowBattery(int energy) {
    energy = (energy + 10) / 20;
    LCD_ShowPercentBar(21, 0+_LCD, 5, energy, 24);
}
//Signal power display
void LCD_ShowSignal(int signal) {
    LCD_ShowPercentBar(102, 1+_LCD, 5, signal / 20, 24);
}
//Backlit display
void LCD_ShowBackLight(int bl) {
    LCD_ShowPercentBar(34, 2+_LCD, 10, bl / 10, 60);
}
//Contrast display
void LCD_ShowContrast(int contrast) {
    LCD_ShowPercentBar(34, 2+_LCD, 6, contrast, 60);
}
//



//num:      total number of lines, 
//pos:      selected items, 
//per_page: number of lines per page
void LCD_ShowPageBar(int num, int sel_pos, int per_page_num) {
    u8 l=121;
    if (LCD_INVERTED) {
        l+=INVERTED_OFFSET;
    }

//  printf("num:%d, pos:%d, per_page_num:%d\n", num, pos, per_page_num);
    int sum_page = (num+per_page_num-1)/per_page_num;                   //Total pages
    int bar_row  = sum_page>8 ? 1: (int)(8+sum_page-1)/sum_page;        //Divide the length bar equally by the number of pages

    u8 con_para = 0;
    for (u8 i=0; i<bar_row; i++) {                                      //Get the length bar to move the square
        con_para |= (0x01<<i);
    }

    int sel_page = num>23 ? sel_pos*8/num : sel_pos/per_page_num*bar_row;
    u8 show_bar = con_para<<sel_page;

//  printf("sel_page:%d, sum_page:%d, bar_row:%d, con_para:%0#x, show_bar:%0#x\n", sel_page, sum_page, bar_row, con_para,show_bar);
    u8 page_array[] = {0x00, show_bar, show_bar, 0xff, show_bar, show_bar, 0x00};
#if _LCD==LCD12832
    LCD_ShowPIC0608(121, 1, 3, 1);
    LCD_ShowPIC0608(121, 3, 4, 1);

    LCD_Setxy(l, 2);
    for (u8 i=0; i<7; i++) {
        LCD_WriteData(page_array[i]);
    }
#else
    LCD_ShowPIC0608(121, 3, 3, 1);
    LCD_ShowPIC0608(121, 5, 4, 1);

    LCD_Setxy(l, 4);
    for (u8 i=0; i<7; i++) {
        LCD_WriteData(page_array[i]);
    }
#endif
}
//
void LCD_ShowMenu31(const char * menu[], int item, int sel_pos) {
    u8 p = 1;
#if _LCD == LCD12864
    p+=2;
#endif

    u8 page = sel_pos/3;    //page
    u8 pos = 3*page;        //The start menu item corresponding to the page

    LCD_ShowString0608(0, p+0, "                      ", pos+0==sel_pos?0:1, 120);
    LCD_ShowString0608(0, p+1, "                      ", pos+1==sel_pos?0:1, 120);
    LCD_ShowString0608(0, p+2, "                      ", pos+2==sel_pos?0:1, 120);
    LCD_ShowPageBar(item, sel_pos, 3);

    if (item>2) {
        LCD_ShowString0608(0, p+0, menu[pos+0],                                     pos+0==sel_pos?0:1, 120);
        LCD_ShowString0608(0, p+1, pos+1<item?menu[pos+1]:"                      ", pos+1==sel_pos?0:1, 120);
        LCD_ShowString0608(0, p+2, pos+2<item?menu[pos+2]:"                      ", pos+2==sel_pos?0:1, 120);
        return;
    }
    //
    else if (item>1) {
        LCD_ShowString0608(0, p+0, menu[0],  sel_pos, 120);
        LCD_ShowString0608(0, p+1, menu[1], !sel_pos, 120);
        return;
    } else {
        LCD_ShowString0608(0, p+0, menu[0], 0, 120);
    }
    //
}
//
void LCD_ShowMenu41(const char * menu[],  int item, int sel_pos) {          //Menu, OK, select the nth one
    u8 p = 0;
#if _LCD == LCD12864
    p+=2;
#endif

    u8 page = sel_pos/4;    //page
    u8 pos = 4*page;        //The start menu item corresponding to the page

//  LCD_ShowString0608(0, p+0, "                      ", pos+0==sel_pos?0:1, 120);
//  LCD_ShowString0608(0, p+1, "                      ", pos+1==sel_pos?0:1, 120);
//  LCD_ShowString0608(0, p+2, "                      ", pos+2==sel_pos?0:1, 120);
//  LCD_ShowString0608(0, p+3, "                      ", pos+3==sel_pos?0:1, 120);
    LCD_ShowPageBar(item, sel_pos, 4);

    if (item>3) {
        LCD_ShowString0608(0, p+0, menu[pos+0],                                     pos+0==sel_pos?0:1, 120);
        LCD_ShowString0608(0, p+1, pos+1<item?menu[pos+1]:"                      ", pos+1==sel_pos?0:1, 120);
        LCD_ShowString0608(0, p+2, pos+2<item?menu[pos+2]:"                      ", pos+2==sel_pos?0:1, 120);
        LCD_ShowString0608(0, p+3, pos+3<item?menu[pos+3]:"                      ", pos+3==sel_pos?0:1, 120);
        return;
    }
    if (item>2) {
        LCD_ShowString0608(0, p+0, menu[pos+0],                                     pos+0==sel_pos?0:1, 120);
        LCD_ShowString0608(0, p+1, pos+1<item?menu[pos+1]:"                      ", pos+1==sel_pos?0:1, 120);
        LCD_ShowString0608(0, p+2, pos+2<item?menu[pos+2]:"                      ", pos+2==sel_pos?0:1, 120);
        return;
    }
    //
    else if (item>1) {
        LCD_ShowString0608(0, p+0, menu[0],  sel_pos, 120);
        LCD_ShowString0608(0, p+1, menu[1], !sel_pos, 120);
        return;
    } else {
        LCD_ShowString0608(0, p,   menu[0], 0, 120);
    }
    //
}
//
//Single-line dual-item menu display
//Figure 1: Display the various options of the sending and receiving settings
void LCD_ShowMatrixMenu22(char menu[][2][12], int item, int sel_pos) {
    u8 p = 1;

#if _LCD == LCD12864
    p+=2;
#endif

    LCD_ShowString0608(0, p, "                        ", 1, 128);
    LCD_ShowString0608(0, p+1, "                        ", 1, 128);
    LCD_ShowString0408(0, p+2, "      PUSH   OR   SET & ENT     ", 1);
    LCD_ShowPIC0408(44,   p+2, 0);
    LCD_ShowPIC0408(64,   p+2, 1);

    int page = sel_pos/4;               //Current page
    u8 start_pos = page*4;

    LCD_ShowPageBar(item, sel_pos, 4);

    LCD_ShowString0608(0, p,                     menu[page*2+0][0],              start_pos+0==sel_pos?0:1, 128);
    LCD_ShowString0608(68, p,                     menu[page*2+0][1],              start_pos+1==sel_pos?0:1, 128);

    LCD_ShowString0608(0, p+1, start_pos+2<item?menu[page*2+1][0] : "        ",  start_pos+2==sel_pos?0:1, 128);
    LCD_ShowString0608(68, p+1, start_pos+3<item?menu[page*2+1][1] : "        ",  start_pos+3==sel_pos?0:1, 128);
}
//Single-line three-item menu display
//Subsonic display (?)
void LCD_ShowMatrixMenu33(const char * menu[][3], int item, int sel_pos) {
    u8 l = 0;
    u8 p = 1;
#if _LCD == LCD12864
    p+=2;
#endif
    if (LCD_INVERTED) {
        l+=INVERTED_OFFSET;
    }

    int page = sel_pos/9;           //Current page
    u8 start_pos = page*9;

    LCD_ShowPageBar(item, sel_pos, 9);

    LCD_ShowString0608(0 +l, p, menu[page*3+0][0],  start_pos+0==sel_pos?0:1, 128);
    LCD_ShowString0608(40+l, p, menu[page*3+0][1],  start_pos+1==sel_pos?0:1, 128);
    LCD_ShowString0608(80+l, p, menu[page*3+0][2],  start_pos+2==sel_pos?0:1, 128);

    LCD_ShowString0608(0 +l, p+1, start_pos+3<item?menu[page*3+1][0]:"      ",  start_pos+3==sel_pos?0:1, 128);
    LCD_ShowString0608(40+l, p+1, start_pos+4<item?menu[page*3+1][1]:"      ",  start_pos+4==sel_pos?0:1, 128);
    LCD_ShowString0608(80+l, p+1, start_pos+5<item?menu[page*3+1][2]:"      ",  start_pos+5==sel_pos?0:1, 128);

    LCD_ShowString0608(0 +l, p+2, start_pos+6<item?menu[page*3+2][0]:"      ",  start_pos+6==sel_pos?0:1, 128);
    LCD_ShowString0608(40+l, p+2, start_pos+7<item?menu[page*3+2][1]:"      ",  start_pos+7==sel_pos?0:1, 128);
    LCD_ShowString0608(80+l, p+2, start_pos+8<item?menu[page*3+2][2]:"      ",  start_pos+8==sel_pos?0:1, 128);
}
//

//Volume display
void LCD_ShowVolume(unsigned char vol) {
    u8 p = 0;
    u8 l = 88;
#if _LCD == LCD12864
    p+=2;
#endif
//  if(LCD_INVERTED)
//      l+=INVERTED_OFFSET;

    if (vol<8) {
        LCD_ShowString0408(l, p,"--------",1);
        do {
            LCD_ShowPIC0408(l+vol*4, p, 2);
        } while (vol--);
    }
}

//Switch options, display "YES", "NO", etc. in the center
void LCD_ShowOption(unsigned char l, unsigned char p, const char * buf[], unsigned char item, unsigned char sel_pos) {
    u8 max_length=0;
    u8 length = strlen(buf[sel_pos])-1;
    for (u8 i=0; i<item; i++) {
        max_length = max_length>strlen(buf[i])?max_length:strlen(buf[i]);
    }

    LCD_ShowString0608(l, p, "                      ", 1, l+max_length);
    LCD_ShowString0608(l+(max_length-length)*6/2, p, buf[sel_pos],  0, l+max_length);
}
//
void LCD_ShowFreq(unsigned  char l,unsigned char p, double f, unsigned char flag) {
    char buf[9]= {0};
    sprintf(buf, "%3.4f", f);
    LCD_ShowString0608(l, p, buf, flag, l+48);
}
//
//Channel number display on the main page
void LCD_ShowChan(unsigned char l, unsigned char p, unsigned char chan, unsigned char flag) {
    char buf[4]= {0};
    sprintf(buf, "%03d", chan);
    LCD_ShowString0608(l, p,  buf, flag, 128);
}
//
void LCD_SetPos(unsigned char l, unsigned char p, int ch) {
    // static uint8_t flag = 0;
    // if (!bsp_CheckTimer(TMR_POS_BLINK))
    //     return;
    // flag = (flag + 1) % 2;
    // if (ch == 0)
    //     ch = ' ';
    // LCD_ShowAscii0608(l, p, ch, flag);
}
//
void LCD_Show_Strloop(uint8_t l, uint8_t p, const char * str, int flag, int limit) {
    // static int i = 0;
    // static char str_old[22];
    // int len = strlen(str);
    // if (strcmp(str, str_old))
    // {
    //     i = 0;
    //     memset(str_old, 0, 22);
    //     memcpy(str_old, str, strlen(str));
    // }
    // if (bsp_CheckTimer(TMR_POS_BLINK))
    // {
    //     if (i == 0)
    //         LCD_Clear(EDITZONE32);
    //     LCD_ShowAscii0608(l + i * 6, p, str_old[i], 1);
    //     i = (i + 1) % len;
    // }
}
