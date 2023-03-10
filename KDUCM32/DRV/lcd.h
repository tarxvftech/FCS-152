#ifndef __MYLCD_H__
#define __MYLCD_H__
#include "FCS152_KDU.h"

//Clear area definition
typedef enum
{
	GLOBAL32,
	EDITZONE32,
	GLOBAL64,
	EDITZONE64
}_ClearScope; 


void LCD_Init(void); 
void LCD_Write(unsigned char dat,unsigned char rs); 						//rs == 1 data rs==0  cmd 
void LCD_WriteCmd(unsigned char cmd); 
void LCD_WriteData(unsigned char data); 
void LCD_HeavyRatio(unsigned char cmd);								//Coarse contrast //0x20-0x27 internal resistance ratio
void LCD_LightRatio(unsigned char cmd);								//Fine-tuning contrast//0-0x3F internal resistance fine-tuning

void LCD_Setxy(unsigned char l, unsigned char p);						//Positioning function
void LCD_ShowAscii0408(unsigned char l, unsigned char p, int ch);				//4 columns and 8 lines of characters (16 pages)
void LCD_ShowAscii0608(unsigned char l, unsigned char p, int ch, unsigned char flag);		//6 columns and 8 lines of characters (16 pages)
void LCD_ShowAscii1016(unsigned char l, unsigned char p, int ch, unsigned char flag);		//10 columns and 16 lines of characters (16 pages) //Radio frequency display

void LCD_ShowString0408(unsigned char l, unsigned char p, const char *s, unsigned char flag);				//4 columns and 8 rows of strings
void LCD_ShowString0608(unsigned char l, unsigned char p, const char *s, unsigned char flag, unsigned char limit);	//6 columns and 8 rows of strings
void LCD_ShowString1016(unsigned char l, unsigned char p, const char *s, unsigned char flag, unsigned char limit); 	//10 columns and 16 rows of strings

void LCD_ShowPICALL(const unsigned char *pic); 
void LCD_ShowPIC0408(unsigned char l, unsigned char p, unsigned char  ch);												 //上下箭头，音量小黑块
void LCD_ShowPIC0608(unsigned char l, unsigned char p, int ch, unsigned char flag); 
void LCD_ShowPIC0808(unsigned char l, unsigned char p, int ch); 
void LCD_ShowPIC1616(unsigned char l, unsigned char p, int ch,unsigned char flag);					//Radio icon
void LCD_ShowPIC2516(unsigned char l, unsigned char p, int ch);					 			//Display of buttons 0-9		

void LCD_ShowProcessBar(unsigned char l, unsigned char p, unsigned char num); 
void LCD_ShowPercentBar (unsigned char l, unsigned char p, unsigned char all_level, unsigned char level, unsigned char length); 
void LCD_ShowBattery(int energy);
void LCD_ShowSignal(int signal);
void LCD_ShowBackLight(int bl);
void LCD_ShowContrast(int contrast);

void LCD_ShowPageBar(int num, int sel_pos, int per_page_num);  

void LCD_ShowMenu31(const char *menu[], int item, int sel_pos);  
void LCD_ShowMenu41(const char *menu[], int item, int sel_pos);  
void LCD_ShowMatrixMenu22(char menu[][2][12], int item, int sel_pos);  
void LCD_ShowMatrixMenu33(const char *menu[][3],    int item, int sel_pos);  

void LCD_ShowVolume(unsigned char vol); 
void LCD_ShowOption(unsigned char l, unsigned char p, const char *buf[], unsigned char item, unsigned char sel_pos);  
void LCD_ShowFreq(unsigned char l, unsigned char p, double f, unsigned char flag);  
void LCD_ShowChan(unsigned char l, unsigned char p, unsigned char chan, unsigned char flag);  

void LCD_Clear(_ClearScope clean_area); 


#endif
