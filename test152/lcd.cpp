#include "lcd.h"
#include "bsp_lcd.h"                //驱动引脚初始化
#include "bsp_delay.h"

#define INVERTED_OFFSET 4
u8 LCD_INVERTED = OFF;              //液晶颠倒显示

void LCD_Init(void) {
    LCD_GPIO_Init();
#if _LCD == LCD12864
    LCD_CS_SET;
    LCD_RST_CLR;
    delay_ms(10); //hard reset for 10ms
    //物理重置
    LCD_RST_SET;
    delay_ms(10);
    //软件重置
    LCD_Write(0xE2,0);
    delay_ms(10);//Soft Reset

    LCD_Write(0xA2,0); //Duty bias set  0xA2 is light 0xA3 is dark
    //LCD_Write(0xA7,0); //0xA7 reverse 0xA6 normal default normal

    LCD_Write(0xA0,0); //ADC select S0->S131(从S1-S128)  a0:左->右 a1:反转，右到左
    LCD_Write(0xC8,0); //com1 --> com64                  C8:普通顺序 c0:反向扫描

    //粗调对比度    //0x20-0x27内部电阻比例
    LCD_Write(0x20+(5&0x07), 0); //V0 Voltage Regulator Internal Resistor Ratio Set 0x20~0x27//1
    delay_ms(2);

    //微调对比度       //0-0x3f内部电阻微调
    LCD_Write(0x81,0); //V0 voltage set first cmd
    LCD_Write(36 & 0x3F, 0); //following V0 voltage set 0x00~0x3F
    delay_ms(2);

    //升压倍数 00:4   01:5   10:6
    LCD_Write(0xF8,0); //Booster Ratio Select Mode Set first cmd
    LCD_Write(0x00 & 0x03,0); //following Booset Ratio Register Set 0x00~0x03
    delay_ms(2);

    //选择内部电压供应操作模式 0010 低四位：1 VB VR VF
    //LCD_Write(0x2F,0); //power control all on
    LCD_Write(0x28+(0x07&7),0);
    delay_ms(2);

    //设置显示存储器的显示初始行, 可设置为 0x40~0x7F, 分别代表第 0~63 行
    LCD_Write(0x40 + (0x3f&0), 0); //0x40 + Display Start Line(0)

    //清屏
    LCD_Clear(GLOBAL64);

    //开启显示
    LCD_Write(0xAF,0); //Display ON
#else
    LCD_RST_CLR;
    delay_us(10);
    LCD_RST_SET;
    delay_us(10);

    LCD_WriteCmd(0xe2);//软件复位
    delay_ms(10);

    LCD_WriteCmd(0x2c);//升压步聚
    delay_ms(5);

    LCD_WriteCmd(0x2e);//升压步聚
    delay_ms(5);

    LCD_WriteCmd(0x2f);//升压步聚
    delay_ms(5);

    LCD_WriteCmd(0x23);//粗调对比度，可设置范围 20～27

    LCD_WriteCmd(0x81);//微调对比度
    LCD_WriteCmd(0x10);//微调对比度的值，可设置范围 0x00～0x3f

    LCD_WriteCmd(0xa2);//1/9 偏压比（bias）

#if LCD_INVERTED==1
    LCD_WriteCmd(0xc0);//行扫描顺序：从上到下
    LCD_WriteCmd(0xa1);//列扫描顺序：反转, 从右到左
#else
    LCD_WriteCmd(0xc8);//行扫描顺序：从上到下
    LCD_WriteCmd(0xa0);//列扫描顺序：从左到右
#endif
    LCD_WriteCmd(0x40);//起始行：从第一行开始
    LCD_WriteCmd(0xaF);//开显示

    LCD_HeavyRatio(3);//3
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

//粗调对比度  //0x20-0x27内部电阻比例
void LCD_HeavyRatio(unsigned char cmd) {
    LCD_CS_CLR;
    LCD_RS_CLR;
    LCD_WriteCmd(0x20+(cmd&0x07));  //0x2x
    LCD_CS_SET;
}
//微调对比度   //0-0x3f内部电阻微调
void LCD_LightRatio(unsigned char cmd) {
    LCD_CS_CLR;
    LCD_RS_CLR;
    LCD_WriteCmd(0x81);
    LCD_WriteCmd(cmd & 0x3f);   //00-3f---》0~48+15=63
    LCD_CS_SET;
}
void LCD_Setxy(unsigned char l, unsigned char p) {
    LCD_WriteCmd(0xb0+p);               //Set Page Address                                                          x
    LCD_WriteCmd(((l>>4)&0x0f)+0x10);   //Set Column Address(MSByte) = 0            0001 l&f   （高四位）          y
    LCD_WriteCmd(l&0x0f);               //Colum(LSByte) from S1 -> S128 auto add    0000 l&0x0f（低四位）
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//以页为地址定位
//4列8行 字符(16页)
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
//6列8行 字符(16页)
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
    for (u8 j=0; s[j]!='\0'; j++) { //j字，i列
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
//10列16行 字符(16页)
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

//上下箭头，音量小黑块
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
//按键0-9的显示
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

//升级显示
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
//实心百分比显示(含上下边框)
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
//电池显示
void LCD_ShowBattery(int energy) {
    energy = (energy + 10) / 20;
    LCD_ShowPercentBar(21, 0+_LCD, 5, energy, 24);
}
//信号显示
void LCD_ShowSignal(int signal) {
    LCD_ShowPercentBar(102, 1+_LCD, 5, signal / 20, 24);
}
//背光显示
void LCD_ShowBackLight(int bl) {
    LCD_ShowPercentBar(34, 2+_LCD, 10, bl / 10, 60);
}
//对比度显示
void LCD_ShowContrast(int contrast) {
    LCD_ShowPercentBar(34, 2+_LCD, 6, contrast, 60);
}
//



//num:总行数, pos:选中项, per_page:每一页的行数
void LCD_ShowPageBar(int num, int sel_pos, int per_page_num) {
    u8 l=121;
    if (LCD_INVERTED) {
        l+=INVERTED_OFFSET;
    }

//  printf("num:%d, pos:%d, per_page_num:%d\n", num, pos, per_page_num);
    int sum_page = (num+per_page_num-1)/per_page_num;                   //总页数
    int bar_row  = sum_page>8 ? 1: (int)(8+sum_page-1)/sum_page;        //将长度条按页数平分

    u8 con_para = 0;
    for (u8 i=0; i<bar_row; i++) { //获取长度条移动方块
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

    u8 page = sel_pos/3;    //页
    u8 pos = 3*page;        //页对应的起始菜单项

    LCD_ShowString0608(0, p+0, "                      ", pos+0==sel_pos?0:1, 120);
    LCD_ShowString0608(0, p+1, "                      ", pos+1==sel_pos?0:1, 120);
    LCD_ShowString0608(0, p+2, "                      ", pos+2==sel_pos?0:1, 120);
    LCD_ShowPageBar(item, sel_pos, 3);

    if (item>2) {
        LCD_ShowString0608(0, p+0, menu[pos+0],                                      pos+0==sel_pos?0:1, 120);
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
void LCD_ShowMenu41(const char * menu[],  int item, int sel_pos) { //菜单, 行, 选中第n个
    u8 p = 0;
#if _LCD == LCD12864
    p+=2;
#endif

    u8 page = sel_pos/4;    //页
    u8 pos = 4*page;        //页对应的起始菜单项

//  LCD_ShowString0608(0, p+0, "                      ", pos+0==sel_pos?0:1, 120);
//  LCD_ShowString0608(0, p+1, "                      ", pos+1==sel_pos?0:1, 120);
//  LCD_ShowString0608(0, p+2, "                      ", pos+2==sel_pos?0:1, 120);
//  LCD_ShowString0608(0, p+3, "                      ", pos+3==sel_pos?0:1, 120);
    LCD_ShowPageBar(item, sel_pos, 4);

    if (item>3) {
        LCD_ShowString0608(0, p+0, menu[pos+0],                                      pos+0==sel_pos?0:1, 120);
        LCD_ShowString0608(0, p+1, pos+1<item?menu[pos+1]:"                      ", pos+1==sel_pos?0:1, 120);
        LCD_ShowString0608(0, p+2, pos+2<item?menu[pos+2]:"                      ", pos+2==sel_pos?0:1, 120);
        LCD_ShowString0608(0, p+3, pos+3<item?menu[pos+3]:"                      ", pos+3==sel_pos?0:1, 120);
        return;
    }
    if (item>2) {
        LCD_ShowString0608(0, p+0, menu[pos+0],                                        pos+0==sel_pos?0:1, 120);
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
//单行双项菜单显示
//图形1：显示收发设置的各个选项
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

    int page = sel_pos/4;               //当前页
    u8 start_pos = page*4;

    LCD_ShowPageBar(item, sel_pos, 4);

    LCD_ShowString0608(0, p,                     menu[page*2+0][0],              start_pos+0==sel_pos?0:1, 128);
    LCD_ShowString0608(68, p,                     menu[page*2+0][1],              start_pos+1==sel_pos?0:1, 128);

    LCD_ShowString0608(0, p+1, start_pos+2<item?menu[page*2+1][0] : "        ",  start_pos+2==sel_pos?0:1, 128);
    LCD_ShowString0608(68, p+1, start_pos+3<item?menu[page*2+1][1] : "        ",  start_pos+3==sel_pos?0:1, 128);
}
//单行三项菜单显示
//亚音显示
void LCD_ShowMatrixMenu33(const char * menu[][3], int item, int sel_pos) {
    u8 l = 0;
    u8 p = 1;
#if _LCD == LCD12864
    p+=2;
#endif
    if (LCD_INVERTED) {
        l+=INVERTED_OFFSET;
    }

    int page = sel_pos/9;           //当前页
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

//音量显示
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

//选项切换,居中显示"YES", "NO",等等
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
//主页面信道号显示
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
