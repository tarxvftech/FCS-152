#ifndef __BSP_UART_H__ 
#define __BSP_UART_H__ 

#include "FCS152_KDU.h"

extern volatile unsigned char  rx1_buf[USART1_BUF_SIZE];
extern volatile unsigned char  rx2_buf[USART2_BUF_SIZE];

//串口1 KDU/固件升级
void bsp_UART1_Init(int baud); 
void UART1_EnRCV(void);	                    //启动串口1接收
int  UART1_getRcvFlag(void);                //判断接收标志位
int  UART1_dataPreProcess(void);            //数据预处理

//串口2 A20模块
void bsp_UART2_Init(int baud); 

void UART1_Put_Char(unsigned char ch); 
void UART2_Put_Char(unsigned char ch); 
////////////////////////////////////////////////////////////////

void UART1_Init(void);
void UART1_Put_String(uint8_t s[]);  
void UART1_Send_Message(char s[], int size); 
void UART2_Put_String(uint8_t s[]);  
void UART2_Send_Message(char s[], int size);  
void Set_A20(CHAN_ARV set, uint8_t sq);  
void Set_A20_MIC(uint8_t miclvl, uint8_t scramlvl, uint8_t tot);  
int  Get_A20_RSSI(void);  
void A002_CALLBACK(void);           //A20数据交互,必须处理了才有a20数据返回

#endif
