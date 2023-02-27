#ifndef __BSP_UART_H__
#define __BSP_UART_H__

#include "FCS152_KDU.h"

extern volatile unsigned char  rx1_buf[USART1_BUF_SIZE];
extern volatile unsigned char  rx2_buf[USART2_BUF_SIZE];

//Serial port 1 KDU/firmware upgrade
void bsp_UART1_Init(int baud);
void UART1_EnRCV(void);                     //Start serial port 1 to receive
int  UART1_getRcvFlag(void);                //Determine the receiving flag
int  UART1_dataPreProcess(void);            //Data preprocessing

//Serial port 2 A20 module
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
void A002_CALLBACK(void);           //A20 data interaction, must be processed before a20 data is returned

#endif
