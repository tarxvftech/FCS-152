#ifndef __IAP_H__
#define __IAP_H__
#include "FCS152_KDU.h"

extern uint8_t	usart1_recv_end_flag;
extern uint16_t rx1_len;
extern char		rx1_buf[USART1_BUF_SIZE];

typedef  void (*iapfun)(void);				        //Define the parameters of a function type

void Iap_load(uint32_t addr);
void KDUCheckRunMaster(uint32_t addr);

void Iap_Write(void);
void EN_Recv(void);




#endif

