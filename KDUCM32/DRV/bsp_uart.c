/*
  ******************************************************************************
  *
  * COPYRIGHT(c) 2020, China Mobile IOT
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of China Mobile IOT nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */ 

/**
 * @file log.c
 * @author CMIOT Solution Team
 * @version v1.0.0
 *
 * @COPYRIGHT(c) 2020, China Mobile IOT. All rights reserved.
 */
#include "FCS152_KDU.h"
#include "bsp_uart.h"

#if LOG_ENABLE
#include "cm32m101a.h"
#include "cm32m101a_gpio.h"
#include "cm32m101a_usart.h"
#include "cm32m101a_rcc.h"

#define LOG_USARTx      USART1
#define LOG_PERIPH      RCC_APB2_PERIPH_USART1
#define LOG_GPIO        GPIOA
#define LOG_PERIPH_GPIO RCC_APB2_PERIPH_GPIOA
#define LOG_TX_PIN      GPIO_PIN_9
#define LOG_RX_PIN      GPIO_PIN_10

uint8_t	 usart1_recv_end_flag=0;			//Reception flag, data reception completion flag
uint32_t rx1_len=0;					//Received data length
char  	 rx1_buf[USART1_BUF_SIZE] = {0};		//Receive buffer

//Periodic crossover switch 32 000 000
void TIM6_Init(void)
{
	TIM_TimeBaseInitType htim6;
	NVIC_InitType NVIC_InitStructure;
	
	//RCC_ConfigPclk1(RCC_HCLK_DIV4);
	RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_TIM6, ENABLE);
	
	/* Enable the TIM6 global Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel                   = TIM6_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
		
	uint16_t PrescalerValue = 0;
	uint16_t timFreq = 50000;
	
	RCC_ClocksType RCC_ClockFreq;
	RCC_GetClocksFreqValue(&RCC_ClockFreq);
	
	//You need to judge the size of HclkFreq. If it exceeds 27M, you need to divide the frequency. At the same time, the frequency doubling of the timer 
	//requires x2 when the frequency division coefficient is not 1.
	
	//SystemCoreClock
	PrescalerValue = (uint16_t)( (RCC_ClockFreq.HclkFreq>27000000?RCC_ClockFreq.Pclk1Freq*2:RCC_ClockFreq.Pclk1Freq) / timFreq) - 1;
	//10 000Hz 		 0.1 ms/Hz
	htim6.Prescaler	= PrescalerValue;			
	//printf("PrescalerValue:%d\n", PrescalerValue);
	htim6.Period	= timFreq/1000*5;			
	htim6.ClkDiv   	= 0;
	htim6.CntMode	= TIM_CNT_MODE_UP;			//TIM_COUNTERMODE_DOWN;//
	
	TIM_InitTimeBase(TIM6, &htim6);
	//TIM_ConfigPrescaler(TIM6, PrescalerValue, TIM_PSC_RELOAD_MODE_IMMEDIATE);
	
	TIM_ClrIntPendingBit(TIM6, TIM_INT_UPDATE);
	//TIM_ClearFlag(TIM6, TIM_FLAG_UPDATE);
	//printf("TIM_INT:%0#x, %d, %d\n", TIM6->STS, usart1_recv_end_flag, __LINE__);
	TIM6->CNT=1;
	TIM_Enable(TIM6, DISABLE);
    TIM_ConfigInt(TIM6, TIM_INT_UPDATE, ENABLE);  /* TIM6 enable update irq */
	
	//TIM_Enable(TIM6, ENABLE);  /* TIM6 enable counter */

}
void log_init(void)
{
	GPIO_InitType	GPIO_InitStructure;
	USART_InitType	USART_InitStructure;
	NVIC_InitType	NVIC_InitStructure;
	 /* Initialize GPIO_InitStructure */
	GPIO_InitStruct(&GPIO_InitStructure);
	
	//Enable GPIO&AFIO clock
	RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO | LOG_PERIPH_GPIO, ENABLE);
	//Enable Pripherial(Usart) clock
	RCC_EnableAPB2PeriphClk(LOG_PERIPH, ENABLE);
/////////////////////////////////////////////////////////////////////////	
	
    /* Enable the USARTz Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel                   = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
/////////////////////////////////////////////////////////////////////////
	//GPIO Config
	GPIO_InitStructure.Pin            		= LOG_TX_PIN;
	GPIO_InitStructure.GPIO_Mode      		= GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Alternate 		= GPIO_AF4_USART1;		
	GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.Pin            		= LOG_RX_PIN;
	GPIO_InitStructure.GPIO_Pull      		= GPIO_Pull_Up;
	GPIO_InitStructure.GPIO_Alternate 		= GPIO_AF4_USART1;
	GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);
////////////////////////////////////////////////////////////////////////
	//Pripherial(Usart) Config
	USART_InitStructure.BaudRate            = 115200;
	USART_InitStructure.WordLength          = USART_WL_8B;
	USART_InitStructure.StopBits            = USART_STPB_1;
	USART_InitStructure.Parity              = USART_PE_NO;
	USART_InitStructure.HardwareFlowControl = USART_HFCTRL_NONE;
	USART_InitStructure.Mode                = USART_MODE_TX | USART_MODE_RX;
	
	USART_ConfigInt(LOG_USARTx, USART_INT_RXDNE, ENABLE);
	
    // init uart
    USART_Init(LOG_USARTx, &USART_InitStructure);
	
	//Clear the INT FLAG before INT config
	//USART_ClrFlag(LOG_USARTx, USART_FLAG_RXDNE);
	//INT Config
	
	
    // enable uart
    USART_Enable(LOG_USARTx, ENABLE);
}

static int is_lr_sent = 0;

int fputc(int ch, FILE* f)
{
    if (ch == '\r')
    {
        is_lr_sent = 1;
    }
    else if (ch == '\n')
    {
        if (!is_lr_sent)
        {
            USART_SendData(LOG_USARTx, (uint8_t)'\r');
            /* Loop until the end of transmission */
            while (USART_GetFlagStatus(LOG_USARTx, USART_FLAG_TXC) == RESET)
            {
            }
        }
        is_lr_sent = 0;
    }
    else
    {
        is_lr_sent = 0;
    }
    USART_SendData(LOG_USARTx, (uint8_t)ch);
    /* Loop until the end of transmission */
    while (USART_GetFlagStatus(LOG_USARTx, USART_FLAG_TXC) == RESET)
    {
    }
    return ch;
}

void UART1_Put_Char(unsigned char ch)
{
	while(!(USART1->STS&(1<<7))){}
	USART1->DAT=ch;
}
void UART1_Put_String(unsigned char s[])
{
	int i=0;
	while(s[i]!='\0')
	{
		UART1_Put_Char(s[i]);
		i++;
	}
}
void UART1_Send_Message(char s[], int size)
{
	for(int i=0; i<size; i++)
		UART1_Put_Char(s[i]);
}

void UART1_PrintBack()
{
	if(usart1_recv_end_flag)
	{
		printf("RCV:\n");
		UART1_Send_Message(rx1_buf, rx1_len);
		memset(rx1_buf, 0, USART1_BUF_SIZE);
		USART_ConfigInt(USART1, USART_INT_RXDNE, ENABLE);
		rx1_len = 0;
		usart1_recv_end_flag = 0;
	}
}
#ifdef USE_FULL_ASSERT

__WEAK void assert_failed(const uint8_t* expr, const uint8_t* file, uint32_t line)
{
    log_error("assertion failed: `%s` at %s:%d", expr, file, line);
    while (1)
    {
    }
}
#endif // USE_FULL_ASSERT

/****************************Interrupt response*********************************/	
void USART1_IRQHandler()
{
//	printf("USART_INT:%0#x, %d\n", LOG_USARTx->STS, __LINE__);
	if(USART_GetIntStatus(LOG_USARTx, USART_INT_RXDNE) != RESET)
	{
		//USART_ClrIntPendingBit(LOG_USARTx, USART_INT_RXDNE);		//Clear flag
		rx1_buf[rx1_len]=USART_ReceiveData(LOG_USARTx);			//Get data
		rx1_len++;							//Update the number of received
		//Timer cleared & enabled
		TIM6->CNT=0;												
		TIM6->CTRL1 |= TIM_CTRL1_CNTEN;
	}
}
void TIM6_IRQHandler()
{
	//printf("TIM_INT:%0#x, %d, %d\n", TIM6->STS, usart1_recv_end_flag, __LINE__);
	if(TIM_GetIntStatus(TIM6, TIM_INT_UPDATE))
	{
		TIM_ClrIntPendingBit(TIM6, TIM_INT_UPDATE);					//Clear flag
		TIM_Enable(TIM6, DISABLE);							//Timer off
		//Turn off the serial port to receive and wait for the data to be processed
		USART_ConfigInt(LOG_USARTx, USART_INT_RXDNE, DISABLE);			
		usart1_recv_end_flag=1;								//Set processing data	
	}
}

#endif // LOG_ENABLE
